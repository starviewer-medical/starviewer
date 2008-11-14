/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "localdatabaseimagedal.h"

#include <QString>
#include <sqlite3.h>

#include "study.h"
#include "series.h"
#include "image.h"
#include "status.h"
#include "databaseconnection.h"
#include "dicommask.h"
#include "logging.h"

#include "starviewersettings.h" //TODO arrrgghh!!

namespace udg {

LocalDatabaseImageDAL::LocalDatabaseImageDAL()
{
}

void LocalDatabaseImageDAL::insert(Image *newImage, int orderNumberInSeries)
{
    m_lastSqliteError = sqlite3_exec( m_dbConnection->getConnection(), qPrintable(buildSqlInsert(newImage, orderNumberInSeries)), 0, 0, 0);

    if (getLastError() != SQLITE_OK) logError(buildSqlInsert(newImage, orderNumberInSeries)); 
}

void LocalDatabaseImageDAL::del(const DicomMask &imageMaskToDelete)
{
    m_lastSqliteError = sqlite3_exec(m_dbConnection->getConnection(), qPrintable(buildSqlDelete(imageMaskToDelete)), 0, 0, 0);

    if (getLastError() != SQLITE_OK) logError(buildSqlDelete(imageMaskToDelete)); 
}

void LocalDatabaseImageDAL::update(Image *imageToUpdate, int orderNumberInSeries)
{
    m_lastSqliteError = sqlite3_exec(m_dbConnection->getConnection(), qPrintable(buildSqlUpdate(imageToUpdate, orderNumberInSeries)), 0, 0, 0);

    if (getLastError() != SQLITE_OK) logError(buildSqlUpdate(imageToUpdate, orderNumberInSeries));
}

QList<Image*> LocalDatabaseImageDAL::query(const DicomMask &imageMask)
{
    int columns , rows;
    char **reply = NULL , **error = NULL;
    QList<Image*> imageList;

    m_lastSqliteError = sqlite3_get_table(m_dbConnection->getConnection(),
                                      qPrintable(buildSqlSelect(imageMask)),
                                    &reply, &rows, &columns, error);
    if (getLastError() != SQLITE_OK)
    {
        logError (buildSqlSelect(imageMask));
        return imageList;
    }

    //index = 1 ignorem les capçaleres
    for (int index = 1; index <= rows ; index++)
    {
        imageList.append(fillImage(reply, index, columns));
    }

    return imageList;
}

int LocalDatabaseImageDAL::count(const DicomMask &imageMaskToCount)
{
    int columns , rows;
    char **reply = NULL , **error = NULL;

    m_lastSqliteError = sqlite3_get_table(m_dbConnection->getConnection(),
                                      qPrintable(buildSqlSelectCountImages(imageMaskToCount)),
                                    &reply, &rows, &columns, error);

    if (getLastError() != SQLITE_OK)
    {
        logError (buildSqlSelectCountImages(imageMaskToCount));
        return -1;
    }

    return QString(reply[1]).toInt();
}

void LocalDatabaseImageDAL::setDatabaseConnection(DatabaseConnection *dbConnect)
{
    m_dbConnection = dbConnect;
}

int LocalDatabaseImageDAL::getLastError()
{
    return m_lastSqliteError;
}

Image* LocalDatabaseImageDAL::fillImage(char **reply, int row, int columns)
{
    Image *image = new Image();

    image->setSOPInstanceUID(reply[0 + row * columns]);
    image->setInstanceNumber(reply[3 + row * columns]);
    image->setImageOrientationPatient(getImageOrientationPatientAsDouble(reply[4 + row * columns ]));
    image->setPatientOrientation(reply[5 + row * columns]);
    image->setPixelSpacing(getPixelSpacingAsDouble(reply[6 + row * columns])[0], getPixelSpacingAsDouble(reply[6 + row * columns])[1]);
    image->setSliceThickness(QString(reply[7 + row * columns]).toDouble());
    image->setImagePositionPatient(getPatientPositionAsDouble(reply[8 + row * columns]));
    image->setSamplesPerPixel(QString(reply[9 + row * columns]).toInt());
    image->setRows(QString(reply[10 + row * columns]).toInt());
    image->setColumns(QString(reply[11 + row * columns]).toInt());
    image->setBitsAllocated(QString(reply[12 + row * columns]).toInt());
    image->setBitsStored(QString(reply[13 + row * columns]).toInt());
    image->setPixelRepresentation(QString(reply[14 + row * columns]).toInt());
    image->setRescaleSlope(QString(reply[15 + row * columns]).toDouble());
    setWindowLevel(image, reply[16 + row * columns], reply[17 + row * columns]);
    image->setWindowLevelExplanations(getWindowLevelExplanationAsQStringList(reply[18 + row * columns]));
    image->setSliceLocation(reply[20 + row * columns]);
    image->setRescaleIntercept(QString(reply[21 + row * columns]).toDouble());
    image->setNumberOfFrames(QString(reply[22 + row * columns]).toInt());
    image->setPhotometricInterpretation(reply[23 + row * columns]);
    image->setRetrievedDate(QDate().fromString(reply[25 + row * columns], "yyyyMMdd"));
    image->setRetrievedTime(QTime().fromString(reply[26 + row * columns], "hhmmss"));

    //TODO argghh!!! Això només hauria d'estar en un únic lloc, no aquí i en retrieveimages.cpp
    StarviewerSettings settings;
    image->setPath( settings.getCacheImagePath() + reply[1 + row * columns] + "/" + reply[2 + row * columns] + "/" + reply[0 + row * columns] );

    return image;
}

QString LocalDatabaseImageDAL::buildSqlSelect(const DicomMask &imageMaskToSelect)
{
    QString selectSentence, orderSentence;


    selectSentence = "Select SOPInstanceUID, StudyInstanceUID, SeriesInstanceUID, InstanceNumber,"
                                    "ImageOrientationPatient, PatientOrientation, PixelSpacing, SliceThickness," 
                                    "PatientPosition, SamplesPerPixel, Rows, Columns, BitsAllocated, BitsStored,"
                                    "PixelRepresentation, RescaleSlope, WindowLevelWidth, WindowLevelCenter,"
                                    "WindowLevelExplanations, SOPInstanceReferenceImage, SliceLocation, RescaleIntercept,"
                                    "NumberOfFrames, PhotometricInterpretation, OrderNumberInSeries, RetrievedDate, "
                                    "RetrievedTime, State "
                            "from Image ";

    orderSentence = " order by OrderNumberInSeries";

    return selectSentence + buildWhereSentence(imageMaskToSelect) + orderSentence;
}

QString LocalDatabaseImageDAL::buildSqlSelectCountImages(const DicomMask &imageMaskToSelect)
{
    QString selectSentence;

    selectSentence = "Select count(*) "
                     "from Image ";

    return selectSentence + buildWhereSentence(imageMaskToSelect);
}


QString LocalDatabaseImageDAL::buildSqlInsert(Image *newImage, int orderNumberInSeries)
{
    QString insertSentence;

    insertSentence = QString("Insert into Image (SOPInstanceUID, StudyInstanceUID, SeriesInstanceUID, InstanceNumber,"
                                             "ImageOrientationPatient, PatientOrientation, PixelSpacing, SliceThickness," 
                                             "PatientPosition, SamplesPerPixel, Rows, Columns, BitsAllocated, BitsStored,"
                                             "PixelRepresentation, RescaleSlope, WindowLevelWidth, WindowLevelCenter,"
                                             "WindowLevelExplanations, SOPInstanceReferenceImage, SliceLocation,"
                                             "RescaleIntercept, NumberOfFrames, PhotometricInterpretation, OrderNumberInSeries,"
                                             "RetrievedDate, RetrievedTime, State) "
                                     "values ('%1','%2','%3','%4','%5','%6','%7',%8,'%9', %10, %11, %12, %13, %14, %15, %16,"
                                              "'%17', '%18', '%19', '%20', '%21', %22, %23, '%24', %25, '%26' , '%27', %28)" )
                            .arg(newImage->getSOPInstanceUID())
                            .arg(newImage->getParentSeries()->getParentStudy()->getInstanceUID())
                            .arg(newImage->getParentSeries()->getInstanceUID())
                            .arg(newImage->getInstanceNumber())
                            .arg(getImageOrientationPatientAsQString(newImage))
                            .arg(newImage->getPatientOrientation())
                            .arg(getPixelSpacingAsQString(newImage))
                            .arg(newImage->getSliceThickness())
                            .arg(getPatientPositionAsQString(newImage))
                            .arg(newImage->getSamplesPerPixel())
                            .arg(newImage->getRows())
                            .arg(newImage->getColumns())
                            .arg(newImage->getBitsAllocated())
                            .arg(newImage->getBitsStored())
                            .arg(newImage->getPixelRepresentation())
                            .arg(newImage->getRescaleSlope())
                            .arg(getWindowWidthAsQString(newImage))
                            .arg(getWindowCenterAsQString(newImage))
                            .arg(getWindowLevelExplanationAsQString(newImage))
                            .arg("")
                            .arg(newImage->getSliceLocation())
                            .arg(newImage->getRescaleIntercept())
                            .arg(newImage->getNumberOfFrames())
                            .arg(newImage->getPhotometricInterpretation())
                            .arg(orderNumberInSeries)
                            .arg(newImage->getRetrievedDate().toString("yyyyMMdd"))
                            .arg(newImage->getRetrievedTime().toString("hhmmss"))
                            .arg(0);



    return insertSentence;
}

QString LocalDatabaseImageDAL::buildSqlUpdate(Image *imageToUpdate, int orderNumberInSeries)
{
    QString updateSentence;

    updateSentence = QString("Update Image set StudyInstanceUID = '%1'," 
                                              "SeriesInstanceUID = '%2',"
                                              "InstanceNumber = '%3',"
                                              "ImageOrientationPatient = '%4',"
                                              "PatientOrientation = '%5',"
                                              "PixelSpacing = '%6',"
                                              "SliceThickness = '%7',"
                                              "PatientPosition = '%8',"
                                              "SamplesPerPixel = '%9',"
                                              "Rows = '%10',"
                                              "Columns = '%11',"
                                              "BitsAllocated = '%12',"
                                              "BitsStored = '%13',"
                                              "PixelRepresentation = '%14',"
                                              "RescaleSlope = '%15',"
                                              "WindowLevelWidth = '%16',"
                                              "WindowLevelCenter = '%17',"
                                              "WindowLevelExplanations = '%18',"
                                              "SOPInstanceReferenceImage = '%19',"
                                              "SliceLocation = '%20',"
                                              "RescaleIntercept = '%21', "
                                              "NumberOfFrames = '%22', "
                                              "PhotometricInterpretation = '%23', "
                                              "OrderNumberInSeries = '%24', "
                                              "RetrievedDate = '%25', "
                                              "RetrievedTime = '%26', "
                                              "State = '%27' "
                                     "Where SOPInstanceUID = '%28'")
                            .arg(imageToUpdate->getParentSeries()->getParentStudy()->getInstanceUID())
                            .arg(imageToUpdate->getParentSeries()->getInstanceUID())
                            .arg(imageToUpdate->getInstanceNumber())
                            .arg(getImageOrientationPatientAsQString(imageToUpdate))
                            .arg(imageToUpdate->getPatientOrientation())
                            .arg(getPixelSpacingAsQString(imageToUpdate))
                            .arg(imageToUpdate->getSliceThickness())
                            .arg(getPatientPositionAsQString(imageToUpdate))
                            .arg(imageToUpdate->getSamplesPerPixel())
                            .arg(imageToUpdate->getRows())
                            .arg(imageToUpdate->getColumns())
                            .arg(imageToUpdate->getBitsAllocated())
                            .arg(imageToUpdate->getBitsStored())
                            .arg(imageToUpdate->getPixelRepresentation())
                            .arg(imageToUpdate->getRescaleSlope())
                            .arg(getWindowWidthAsQString(imageToUpdate))
                            .arg(getWindowCenterAsQString(imageToUpdate))
                            .arg(getWindowLevelExplanationAsQString(imageToUpdate))
                            .arg("")
                            .arg(imageToUpdate->getSliceLocation())
                            .arg(imageToUpdate->getRescaleIntercept())
                            .arg(imageToUpdate->getNumberOfFrames())
                            .arg(imageToUpdate->getPhotometricInterpretation())
                            .arg(orderNumberInSeries)
                            .arg(imageToUpdate->getRetrievedDate().toString("yyyyMMdd"))
                            .arg(imageToUpdate->getRetrievedTime().toString("hhmmss"))
                            .arg(0)
                            .arg(imageToUpdate->getSOPInstanceUID());

    return updateSentence;
}

QString LocalDatabaseImageDAL::buildSqlDelete(const DicomMask &imageMaskToDelete)
{
    return "delete from Image " + buildWhereSentence(imageMaskToDelete);
}

QString LocalDatabaseImageDAL::buildWhereSentence(const DicomMask &imageMask)
{
    QString whereSentence = "";

    if (!imageMask.getStudyUID().isEmpty()) 
        whereSentence = QString("where StudyInstanceUID = '%1'").arg(imageMask.getStudyUID());

    if (!imageMask.getSeriesUID().isEmpty())
    {
        if (whereSentence.isEmpty()) 
            whereSentence = "where";
        else
            whereSentence += " and ";

        whereSentence += QString(" SeriesInstanceUID = '%1'").arg(imageMask.getSeriesUID());
    }

    if (!imageMask.getSOPInstanceUID().isEmpty())
    {
        if (whereSentence.isEmpty()) 
            whereSentence = "where";
        else
            whereSentence += " and ";

        whereSentence += QString(" SOPInstanceUID = '%1'").arg(imageMask.getSOPInstanceUID());
    }

    return whereSentence;
}

QString LocalDatabaseImageDAL::getPixelSpacingAsQString(Image *newImage)
{
    QString imagePixelSpacing = "", value;

    imagePixelSpacing += value.setNum(newImage->getPixelSpacing()[0], 'g', 10) + "\\";
    imagePixelSpacing += value.setNum(newImage->getPixelSpacing()[1], 'g', 10);

    return imagePixelSpacing;
}

double* LocalDatabaseImageDAL::getPixelSpacingAsDouble(const QString &pixelSpacing)
{
    QStringList list = pixelSpacing.split( "\\" );

    if (list.size() == 2)
    {
        m_pixelSpacing[0] = list.at(0).toDouble();
        m_pixelSpacing[1] = list.at(1).toDouble();
    }
    else
    {
        m_pixelSpacing[0] = 0;
        m_pixelSpacing[1] = 0;
    }

    return m_pixelSpacing;
}

double* LocalDatabaseImageDAL::getImageOrientationPatientAsDouble(const QString &ImageOrientationPatient)
{
    QStringList list = ImageOrientationPatient.split( "\\" );

    for (int index = 0; index < 6; index++)
    {
        if (list.size() == 6)
            m_imageOrientationPatient[index] = list.at(index).toDouble();
        else
            m_imageOrientationPatient[index] = 0;
    }

    return m_imageOrientationPatient;
}

QString LocalDatabaseImageDAL::getImageOrientationPatientAsQString(Image *newImage)
{
    QString ImageOrientationPatient = "", value;

    ImageOrientationPatient += value.setNum(newImage->getImageOrientationPatient()[0], 'g', 10) + "\\";
    ImageOrientationPatient += value.setNum(newImage->getImageOrientationPatient()[1], 'g', 10) + "\\";
    ImageOrientationPatient += value.setNum(newImage->getImageOrientationPatient()[2], 'g', 10) + "\\";
    ImageOrientationPatient += value.setNum(newImage->getImageOrientationPatient()[3], 'g', 10) + "\\";
    ImageOrientationPatient += value.setNum(newImage->getImageOrientationPatient()[4], 'g', 10) + "\\";
    ImageOrientationPatient += value.setNum(newImage->getImageOrientationPatient()[5], 'g', 10);

    return ImageOrientationPatient;
}

double* LocalDatabaseImageDAL::getPatientPositionAsDouble(const QString &patientPosition)
{
    QStringList list = patientPosition.split( "\\" );

    for (int index = 0; index < 3; index++)
    {
        if (list.size() == 3)
            m_patientPosition[index] = list.at(index).toDouble();
        else
            m_patientPosition[index] = 0;
    }

    return m_patientPosition;
}

QString LocalDatabaseImageDAL::getPatientPositionAsQString(Image *newImage)
{
    QString patientPosition = "", value;

    patientPosition += value.setNum(newImage->getImagePositionPatient()[0], 'g', 10) + "\\";
    patientPosition += value.setNum(newImage->getImagePositionPatient()[1], 'g', 10) + "\\";
    patientPosition += value.setNum(newImage->getImagePositionPatient()[2], 'g', 10);

    return patientPosition;
}

QString LocalDatabaseImageDAL::getWindowWidthAsQString(Image *newImage)
{
    QString windowWidth = "", value;

    for (int index = 0 ; index < newImage->getNumberOfWindowLevels(); index ++)
    {
        windowWidth += value.setNum(newImage->getWindowLevel(index).first, 'g', 10) + "\\";
    }

    return windowWidth.left(windowWidth.length() -1); //treiem l'últim "\\" afegit
}

QString LocalDatabaseImageDAL::getWindowCenterAsQString(Image *newImage)
{
    QString windowCenter = "", value;

    for (int index = 0 ; index < newImage->getNumberOfWindowLevels(); index ++)
    {
        windowCenter += value.setNum(newImage->getWindowLevel(index).second, 'g', 10) + "\\";
    }

    return windowCenter.left(windowCenter.length() -1); //treiem l'últim "\\" afegit
}

QString LocalDatabaseImageDAL::getWindowLevelExplanationAsQString(Image *newImage)
{
    QString windowLevelExplanation = "";

    for (int index = 0 ; index < newImage->getNumberOfWindowLevels(); index ++)
    {
        windowLevelExplanation += newImage->getWindowLevelExplanation(index) + "\\";
    }

    return windowLevelExplanation.left(windowLevelExplanation.length() -1); //treiem l'últim "\\" afegit
}

QStringList LocalDatabaseImageDAL::getWindowLevelExplanationAsQStringList(const QString &explanationList)
{
    return explanationList.split("\\");
}


void LocalDatabaseImageDAL::setWindowLevel(Image *selectedImage, const QString &windowLevelWidth, const QString &windowLevelCenter)
{
    QStringList listWindowLevelWidth = windowLevelWidth.split("\\"), listWindowLevelCenter = windowLevelCenter.split("\\");

    if (listWindowLevelWidth.size() == listWindowLevelCenter.size())
    {
        for (int index = 0; index < listWindowLevelWidth.size(); index++)
        {
            selectedImage->addWindowLevel(listWindowLevelWidth.at(index).toDouble(), listWindowLevelCenter.at(index).toDouble());
        }
    }
}

void LocalDatabaseImageDAL::logError(const QString &sqlSentence)
{
    //Ingnorem l'error de clau duplicada
    if (getLastError() != SQLITE_CONSTRAINT)
    {
        ERROR_LOG("S'ha produït l'error: " + QString().setNum(getLastError()) + ", " + m_dbConnection->getLastErrorMessage() + ", al executar la següent sentència sql " + sqlSentence);
    }
}


LocalDatabaseImageDAL::~LocalDatabaseImageDAL()
{
}

}
