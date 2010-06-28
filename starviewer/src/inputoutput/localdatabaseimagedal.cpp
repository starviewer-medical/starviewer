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
#include "localdatabasemanager.h"

namespace udg {

LocalDatabaseImageDAL::LocalDatabaseImageDAL()
{
    m_lastSqliteError = SQLITE_OK;
}

void LocalDatabaseImageDAL::insert(Image *newImage)
{
    m_lastSqliteError = sqlite3_exec( m_dbConnection->getConnection(), qPrintable(buildSqlInsert(newImage)), 0, 0, 0 );

    if (getLastError() != SQLITE_OK) logError(buildSqlInsert(newImage)); 
}

void LocalDatabaseImageDAL::del(const DicomMask &imageMaskToDelete)
{
    m_lastSqliteError = sqlite3_exec(m_dbConnection->getConnection(), qPrintable(buildSqlDelete(imageMaskToDelete)), 0, 0, 0);

    if (getLastError() != SQLITE_OK) logError(buildSqlDelete(imageMaskToDelete)); 
}

void LocalDatabaseImageDAL::update(Image *imageToUpdate)
{
    m_lastSqliteError = sqlite3_exec(m_dbConnection->getConnection(), qPrintable(buildSqlUpdate(imageToUpdate)), 0, 0, 0);

    if (getLastError() != SQLITE_OK) logError(buildSqlUpdate(imageToUpdate));
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
    image->setFrameNumber( QString(reply[1 + row * columns]).toInt() );
    image->setInstanceNumber(reply[4 + row * columns]);
    image->setImageOrientationPatient(getImageOrientationPatientAsDouble(reply[5 + row * columns ]));
    image->setPatientOrientation(reply[6 + row * columns]);
    image->setPixelSpacing(getPixelSpacingAsDouble(reply[7 + row * columns])[0], getPixelSpacingAsDouble(reply[7 + row * columns])[1]);
    image->setSliceThickness(QString(reply[8 + row * columns]).toDouble());
    image->setImagePositionPatient(getPatientPositionAsDouble(reply[9 + row * columns]));
    image->setSamplesPerPixel(QString(reply[10 + row * columns]).toInt());
    image->setRows(QString(reply[11 + row * columns]).toInt());
    image->setColumns(QString(reply[12 + row * columns]).toInt());
    image->setBitsAllocated(QString(reply[13 + row * columns]).toInt());
    image->setBitsStored(QString(reply[14 + row * columns]).toInt());
    image->setPixelRepresentation(QString(reply[15 + row * columns]).toInt());
    image->setRescaleSlope(QString(reply[16 + row * columns]).toDouble());
    setWindowLevel(image, reply[17 + row * columns], reply[18 + row * columns]);
    image->setWindowLevelExplanations(getWindowLevelExplanationAsQStringList(reply[19 + row * columns]));
    image->setSliceLocation(reply[20 + row * columns]);
    image->setRescaleIntercept(QString(reply[21 + row * columns]).toDouble());
    image->setPhotometricInterpretation(reply[22 + row * columns]);
    image->setImageType(reply[23 + row * columns]);
    image->setViewPosition(reply[24 + row * columns]);
    image->setImageLaterality(QChar(reply[25 + row * columns][0]));//ImageLaterality sempre és un Char
    image->setViewCodeMeaning(reply[26 + row * columns]);
    image->setPhaseNumber(QString(reply[27 + row * columns]).toInt());
    image->setImageTime(reply[28 + row * columns]);
    image->setVolumeNumberInSeries(QString(reply [29 + row * columns]).toInt());
    image->setOrderNumberInVolume(QString(reply [30 + row * columns]).toInt());
    image->setRetrievedDate(QDate().fromString(reply[31 + row * columns], "yyyyMMdd"));
    image->setRetrievedTime(QTime().fromString(reply[32 + row * columns], "hhmmss"));

    //TODO argghh!!! Això només hauria d'estar en un únic lloc, no aquí i en retrieveimages.cpp
    image->setPath( LocalDatabaseManager::getCachePath() + reply[2 + row * columns] + "/" + reply[3 + row * columns] + "/" + reply[0 + row * columns] );

    return image;
}

QString LocalDatabaseImageDAL::buildSqlSelect(const DicomMask &imageMaskToSelect)
{
    QString selectSentence, orderSentence;


    selectSentence = "Select SOPInstanceUID, FrameNumber, StudyInstanceUID, SeriesInstanceUID, InstanceNumber,"
                                    "ImageOrientationPatient, PatientOrientation, PixelSpacing, SliceThickness," 
                                    "PatientPosition, SamplesPerPixel, Rows, Columns, BitsAllocated, BitsStored,"
                                    "PixelRepresentation, RescaleSlope, WindowLevelWidth, WindowLevelCenter,"
                                    "WindowLevelExplanations, SliceLocation, RescaleIntercept,"
                                    "PhotometricInterpretation, ImageType, ViewPosition,"
                                    "ImageLaterality, ViewCodeMeaning , PhaseNumber, ImageTime,  VolumeNumberInSeries,"
                                    "OrderNumberInVolume, RetrievedDate, RetrievedTime, State "
                            "from Image ";

    orderSentence = " order by VolumeNumberInSeries, OrderNumberInVolume";

    return selectSentence + buildWhereSentence(imageMaskToSelect) + orderSentence;
}

QString LocalDatabaseImageDAL::buildSqlSelectCountImages(const DicomMask &imageMaskToSelect)
{
    QString selectSentence;

    selectSentence = "Select count(*) "
                     "from Image ";

    return selectSentence + buildWhereSentence(imageMaskToSelect);
}


QString LocalDatabaseImageDAL::buildSqlInsert(Image *newImage)
{
    QString insertSentence;

    insertSentence = QString("Insert into Image (SOPInstanceUID, FrameNumber, StudyInstanceUID, SeriesInstanceUID, InstanceNumber,"
                                             "ImageOrientationPatient, PatientOrientation, PixelSpacing, SliceThickness," 
                                             "PatientPosition, SamplesPerPixel, Rows, Columns, BitsAllocated, BitsStored,"
                                             "PixelRepresentation, RescaleSlope, WindowLevelWidth, WindowLevelCenter,"
                                             "WindowLevelExplanations, SliceLocation,"
                                             "RescaleIntercept, PhotometricInterpretation, ImageType, ViewPosition,"
                                             "ImageLaterality, ViewCodeMeaning, PhaseNumber, ImageTime, VolumeNumberInSeries, "
                                             "OrderNumberInVolume, RetrievedDate, RetrievedTime, State) "
                                     "values ('%1', %2, '%3', '%4', '%5', "
                                             "'%6', '%7', '%8', %9,"
                                             "'%10', %11, %12, %13, %14, %15, "
                                             "%16, %17,'%18', '%19', "
                                             "'%20', '%21', "
                                             "%22, '%23', '%24', '%25',"
                                             "'%26',  '%27', %28, '%29', %30,"
                                             "%31, '%32', '%33', %34)" )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( newImage->getSOPInstanceUID() ) )
                            .arg( newImage->getFrameNumber())
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( newImage->getParentSeries()->getParentStudy()->getInstanceUID() ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( newImage->getParentSeries()->getInstanceUID() ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( newImage->getInstanceNumber() ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( getImageOrientationPatientAsQString(newImage) ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( newImage->getPatientOrientation() ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( getPixelSpacingAsQString(newImage) ) )
                            .arg( newImage->getSliceThickness() )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( getPatientPositionAsQString(newImage) ) )
                            .arg( newImage->getSamplesPerPixel() )
                            .arg( newImage->getRows() )
                            .arg( newImage->getColumns() )
                            .arg( newImage->getBitsAllocated() )
                            .arg( newImage->getBitsStored() )
                            .arg( newImage->getPixelRepresentation() )
                            .arg( newImage->getRescaleSlope() )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( getWindowWidthAsQString(newImage) ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( getWindowCenterAsQString(newImage) ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( getWindowLevelExplanationAsQString(newImage) ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( newImage->getSliceLocation() ) )
                            .arg( newImage->getRescaleIntercept() )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( newImage->getPhotometricInterpretation() ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( newImage->getImageType() ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( newImage->getViewPosition() ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( newImage->getImageLaterality() ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( newImage->getViewCodeMeaning() ) )
                            .arg( newImage->getPhaseNumber() )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( newImage->getImageTime() ) )
                            .arg (newImage->getVolumeNumberInSeries() )
                            .arg( newImage->getOrderNumberInVolume())
                            .arg( newImage->getRetrievedDate().toString("yyyyMMdd") )
                            .arg( newImage->getRetrievedTime().toString("hhmmss") )
                            .arg(0);

    return insertSentence;
}

QString LocalDatabaseImageDAL::buildSqlUpdate(Image *imageToUpdate)
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
                                              "SliceLocation = '%19',"
                                              "RescaleIntercept = '%20', "
                                              "PhotometricInterpretation = '%21', "
                                              "ImageType = '%22', "
                                              "ViewPosition = '%23', "
                                              "ImageLaterality = '%24', "
                                              "ViewCodeMeaning = '%25', "
                                              "PhaseNumber = %26, "
                                              "ImageTime = '%27', "
                                              "VolumeNumberInSeries = %28, "
                                              "OrderNumberInVolume = '%29', "
                                              "RetrievedDate = '%30', "
                                              "RetrievedTime = '%31', "
                                              "State = '%32' "
                                     "Where SOPInstanceUID = '%33' And "
                                           "FrameNumber = %34")
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( imageToUpdate->getParentSeries()->getParentStudy()->getInstanceUID() ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( imageToUpdate->getParentSeries()->getInstanceUID() ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( imageToUpdate->getInstanceNumber() ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( getImageOrientationPatientAsQString(imageToUpdate) ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( imageToUpdate->getPatientOrientation() ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( getPixelSpacingAsQString(imageToUpdate) ) )
                            .arg( imageToUpdate->getSliceThickness() )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( getPatientPositionAsQString(imageToUpdate) ) )
                            .arg( imageToUpdate->getSamplesPerPixel() )
                            .arg( imageToUpdate->getRows() )
                            .arg( imageToUpdate->getColumns() )
                            .arg( imageToUpdate->getBitsAllocated() )
                            .arg( imageToUpdate->getBitsStored() )
                            .arg( imageToUpdate->getPixelRepresentation() )
                            .arg( imageToUpdate->getRescaleSlope() )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( getWindowWidthAsQString(imageToUpdate) ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( getWindowCenterAsQString(imageToUpdate) ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( getWindowLevelExplanationAsQString(imageToUpdate) ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( imageToUpdate->getSliceLocation() ) )
                            .arg( imageToUpdate->getRescaleIntercept() )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( imageToUpdate->getPhotometricInterpretation() ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( imageToUpdate->getImageType() ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( imageToUpdate->getViewPosition() ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( imageToUpdate->getImageLaterality() ) )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( imageToUpdate->getViewCodeMeaning() ) )
                            .arg ( imageToUpdate->getPhaseNumber() )
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( imageToUpdate->getImageTime() ) )
                            .arg ( imageToUpdate->getVolumeNumberInSeries() )
                            .arg( imageToUpdate->getOrderNumberInVolume() )
                            .arg( imageToUpdate->getRetrievedDate().toString("yyyyMMdd"))
                            .arg( imageToUpdate->getRetrievedTime().toString("hhmmss"))
                            .arg(0)
                            .arg( DatabaseConnection::formatTextToValidSQLSyntax( imageToUpdate->getSOPInstanceUID() ) )
                            .arg( imageToUpdate->getFrameNumber() );

    return updateSentence;
}

QString LocalDatabaseImageDAL::buildSqlDelete(const DicomMask &imageMaskToDelete)
{
    return "delete from Image " + buildWhereSentence(imageMaskToDelete);
}

QString LocalDatabaseImageDAL::buildWhereSentence(const DicomMask &imageMask)
{
    QString whereSentence = "";

    if (!imageMask.getStudyInstanceUID().isEmpty()) 
        whereSentence = QString("where StudyInstanceUID = '%1'").arg( DatabaseConnection::formatTextToValidSQLSyntax( imageMask.getStudyInstanceUID() ) );

    if (!imageMask.getSeriesInstanceUID().isEmpty())
    {
        if (whereSentence.isEmpty()) 
            whereSentence = "where";
        else
            whereSentence += " and ";

        whereSentence += QString(" SeriesInstanceUID = '%1'").arg( DatabaseConnection::formatTextToValidSQLSyntax( imageMask.getSeriesInstanceUID() ) );
    }

    if (!imageMask.getSOPInstanceUID().isEmpty())
    {
        if (whereSentence.isEmpty()) 
            whereSentence = "where";
        else
            whereSentence += " and ";

        whereSentence += QString(" SOPInstanceUID = '%1'").arg( DatabaseConnection::formatTextToValidSQLSyntax( imageMask.getSOPInstanceUID() ) );
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
        ERROR_LOG("S'ha produit l'error: " + QString().setNum(getLastError()) + ", " + m_dbConnection->getLastErrorMessage() + ", al executar la seguent sentencia sql " + sqlSentence);
    }
}


LocalDatabaseImageDAL::~LocalDatabaseImageDAL()
{
}

}
