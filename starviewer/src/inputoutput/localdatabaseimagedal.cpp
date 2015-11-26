/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "localdatabaseimagedal.h"

#include <QSqlQuery>
#include <QString>
#include <QVariant>
#include <QVector2D>

#include "study.h"
#include "series.h"
#include "image.h"
#include "databaseconnection.h"
#include "dicommask.h"
#include "localdatabasemanager.h"
#include "imageorientation.h"
#include "localdatabasedisplayshutterdal.h"
#include "dicomsource.h"
#include "localdatabasepacsretrievedimagesdal.h"
#include "dicomformattedvaluesconverter.h"
#include "dicomvaluerepresentationconverter.h"
#include "localdatabasevoilutdal.h"

namespace udg {

LocalDatabaseImageDAL::LocalDatabaseImageDAL(DatabaseConnection &dbConnection)
 : LocalDatabaseBaseDAL(dbConnection)
{
}

bool LocalDatabaseImageDAL::insert(Image *newImage)
{
    QSqlQuery query;

    if (!query.exec(buildSqlInsert(newImage)))
    {
        logError(query);
        return false;
    }

    return true;
}

bool LocalDatabaseImageDAL::del(const DicomMask &imageMaskToDelete)
{
    QSqlQuery query;

    if (!query.exec(buildSqlDelete(imageMaskToDelete)))
    {
        logError(query);
        return false;
    }

    return true;
}

void LocalDatabaseImageDAL::update(Image *imageToUpdate)
{
    QSqlQuery query;

    if (!query.exec(buildSqlUpdate(imageToUpdate)))
    {
        logError(query);
    }
}

QList<Image*> LocalDatabaseImageDAL::query(const DicomMask &imageMask)
{
    QList<Image*> imageList;
    QSqlQuery query;

    if (!query.exec(buildSqlSelect(imageMask)))
    {
        logError(query);
        return imageList;
    }

    LocalDatabaseDisplayShutterDAL shutterDAL(m_databaseConnection);
    LocalDatabaseVoiLutDAL voiLutDal(m_databaseConnection);

    while (query.next())
    {
        Image *newImage = fillImage(query);
            
        // Obtenim els shutters de l'imatge actual
        DicomMask mask;
        mask.setSOPInstanceUID(newImage->getSOPInstanceUID());
        mask.setImageNumber(QString::number(newImage->getFrameNumber()));
        newImage->setDisplayShutters(shutterDAL.query(mask));

        // Get VOI LUTs
        QList<VoiLut> voiLuts = voiLutDal.query(mask);

        foreach (const VoiLut &voiLut, voiLuts)
        {
            newImage->addVoiLut(voiLut);
        }
        
        imageList << newImage;
    }

    return imageList;
}

int LocalDatabaseImageDAL::count(const DicomMask &imageMaskToCount)
{
    QSqlQuery query;

    if (!query.exec(buildSqlSelectCountImages(imageMaskToCount)))
    {
        logError(query);
        return -1;
    }

    query.next();

    return query.value(0).toInt();
}

Image* LocalDatabaseImageDAL::fillImage(const QSqlQuery &query)
{
    Image *image = new Image();

    image->setSOPInstanceUID(query.value("SOPInstanceUID").toString());
    image->setFrameNumber(query.value("FrameNumber").toInt());
    image->setInstanceNumber(query.value("InstanceNumber").toString());
    ImageOrientation imageOrientation;
    imageOrientation.setDICOMFormattedImageOrientation(query.value("ImageOrientationPatient").toString());
    image->setImageOrientationPatient(imageOrientation);
    
    PatientOrientation patientOrientation;
    patientOrientation.setDICOMFormattedPatientOrientation(query.value("PatientOrientation").toString());
    image->setPatientOrientation(patientOrientation);
    
    image->setPixelSpacing(getPixelSpacingAsDouble(query.value("PixelSpacing").toString())[0],
                           getPixelSpacingAsDouble(query.value("PixelSpacing").toString())[1]);
    image->setSliceThickness(query.value("SliceThickness").toString().toDouble());
    image->setImagePositionPatient(getPatientPositionAsDouble(query.value("PatientPosition").toString()));
    image->setSamplesPerPixel(query.value("SamplesPerPixel").toInt());
    image->setRows(query.value("Rows").toInt());
    image->setColumns(query.value("Columns").toInt());
    image->setBitsAllocated(query.value("BitsAllocated").toInt());
    image->setBitsStored(query.value("BitsStored").toInt());
    image->setPixelRepresentation(query.value("PixelRepresentation").toInt());
    image->setRescaleSlope(query.value("RescaleSlope").toString().toDouble());
    QList<WindowLevel> windowLevelList = DICOMFormattedValuesConverter::parseWindowLevelValues(query.value("WindowLevelWidth").toString(),
                                                                                               query.value("WindowLevelCenter").toString(),
                                                                                               convertToQString(query.value("WindowLevelExplanations")));
    QList<VoiLut> voiLutList;
    foreach (const WindowLevel &windowLevel, windowLevelList)
    {
        voiLutList.append(windowLevel);
    }
    image->setVoiLutList(voiLutList);
    image->setSliceLocation(query.value("SliceLocation").toString());
    image->setRescaleIntercept(query.value("RescaleIntercept").toString().toDouble());
    image->setPhotometricInterpretation(query.value("PhotometricInterpretation").toString());
    image->setImageType(query.value("ImageType").toString());
    image->setViewPosition(query.value("ViewPosition").toString());
    // ImageLaterality sempre és un Char
    image->setImageLaterality(query.value("ImageLaterality").toString()[0]);
    image->setViewCodeMeaning(convertToQString(query.value("ViewCodeMeaning")));
    image->setPhaseNumber(query.value("PhaseNumber").toInt());
    image->setImageTime(query.value("ImageTime").toString());
    image->setVolumeNumberInSeries(query.value("VolumeNumberInSeries").toInt());
    image->setOrderNumberInVolume(query.value("OrderNumberInVolume").toInt());
    image->setRetrievedDate(QDate().fromString(query.value("RetrievedDate").toString(), "yyyyMMdd"));
    image->setRetrievedTime(QTime().fromString(query.value("RetrievedTime").toString(), "hhmmss"));
    image->setNumberOfOverlays(query.value("NumberOfOverlays").toUInt());
    image->setDICOMSource(getImageDICOMSourceByIDPACSInDatabase(query.value("RetrievedPACSID")));
    QVector2D imagerPixelSpacing = getImagerPixelSpacingAs2DVector(query.value("ImagerPixelSpacing").toString());
    image->setImagerPixelSpacing(imagerPixelSpacing.x(), imagerPixelSpacing.y());
    image->setEstimatedRadiographicMagnificationFactor(query.value("EstimatedRadiographicMagnificationFactor").toString().toDouble());
    image->setTransferSyntaxUID(query.value("TransferSyntaxUID").toString());

    // TODO argghh!!! Això només hauria d'estar en un únic lloc, no aquí i en retrieveimages.cpp
    image->setPath(LocalDatabaseManager::getCachePath() + query.value("StudyInstanceUID").toString() + "/" + query.value("SeriesInstanceUID").toString() + "/" +
                   query.value("SOPInstanceUID").toString());

    return image;
}

QString LocalDatabaseImageDAL::buildSqlSelect(const DicomMask &imageMaskToSelect)
{
    QString selectSentence = "Select SOPInstanceUID, FrameNumber, StudyInstanceUID, SeriesInstanceUID, InstanceNumber,"
                                    "ImageOrientationPatient, PatientOrientation, PixelSpacing, SliceThickness,"
                                    "PatientPosition, SamplesPerPixel, Rows, Columns, BitsAllocated, BitsStored,"
                                    "PixelRepresentation, RescaleSlope, WindowLevelWidth, WindowLevelCenter,"
                                    "WindowLevelExplanations, SliceLocation, RescaleIntercept,"
                                    "PhotometricInterpretation, ImageType, ViewPosition,"
                                    "ImageLaterality, ViewCodeMeaning , PhaseNumber, ImageTime,  VolumeNumberInSeries,"
                                    "OrderNumberInVolume, RetrievedDate, RetrievedTime, State, NumberOfOverlays, RetrievedPACSID,"
                                    "ImagerPixelSpacing, EstimatedRadiographicMagnificationFactor, TransferSyntaxUID "
                            "from Image ";

    QString orderSentence = " order by VolumeNumberInSeries, OrderNumberInVolume";

    return selectSentence + buildWhereSentence(imageMaskToSelect) + orderSentence;
}

QString LocalDatabaseImageDAL::buildSqlSelectCountImages(const DicomMask &imageMaskToSelect)
{
    QString selectSentence = "Select count(*) from Image ";

    return selectSentence + buildWhereSentence(imageMaskToSelect);
}

QString LocalDatabaseImageDAL::buildSqlInsert(Image *newImage)
{
    QString windowWidth, windowCenter, windowExplanation;
    getWindowLevelInformationAsQString(newImage, windowWidth, windowCenter, windowExplanation);
    
    QString insertSentence = QString("Insert into Image (SOPInstanceUID, FrameNumber, StudyInstanceUID, SeriesInstanceUID, InstanceNumber,"
                                             "ImageOrientationPatient, PatientOrientation, PixelSpacing, SliceThickness,"
                                             "PatientPosition, SamplesPerPixel, Rows, Columns, BitsAllocated, BitsStored,"
                                             "PixelRepresentation, RescaleSlope, WindowLevelWidth, WindowLevelCenter,"
                                             "WindowLevelExplanations, SliceLocation,"
                                             "RescaleIntercept, PhotometricInterpretation, ImageType, ViewPosition,"
                                             "ImageLaterality, ViewCodeMeaning, PhaseNumber, ImageTime, VolumeNumberInSeries,"
                                             "OrderNumberInVolume, RetrievedDate, RetrievedTime, State, NumberOfOverlays, RetrievedPACSID,"
                                             "ImagerPixelSpacing, EstimatedRadiographicMagnificationFactor, TransferSyntaxUID) "
                                     "values ('%1', %2, '%3', '%4', '%5', "
                                             "'%6', '%7', '%8', %9,"
                                             "'%10', %11, %12, %13, %14, %15, "
                                             "%16, %17,'%18', '%19', "
                                             "'%20', '%21', "
                                             "%22, '%23', '%24', '%25',"
                                             "'%26',  '%27', %28, '%29', %30,"
                                             "%31, '%32', '%33', %34, %35, %36, '%37', %38, '%39')")
                            .arg(formatTextToValidSQLSyntax(newImage->getSOPInstanceUID()))
                            .arg(newImage->getFrameNumber())
                            .arg(formatTextToValidSQLSyntax(newImage->getParentSeries()->getParentStudy()->getInstanceUID()))
                            .arg(formatTextToValidSQLSyntax(newImage->getParentSeries()->getInstanceUID()))
                            .arg(formatTextToValidSQLSyntax(newImage->getInstanceNumber()))
                            .arg(formatTextToValidSQLSyntax(newImage->getImageOrientationPatient().getDICOMFormattedImageOrientation()))
                            .arg(formatTextToValidSQLSyntax(newImage->getPatientOrientation().getDICOMFormattedPatientOrientation()))
                            .arg(formatTextToValidSQLSyntax(getPixelSpacingAsQString(newImage)))
                            .arg(newImage->getSliceThickness())
                            .arg(formatTextToValidSQLSyntax(getPatientPositionAsQString(newImage)))
                            .arg(newImage->getSamplesPerPixel())
                            .arg(newImage->getRows())
                            .arg(newImage->getColumns())
                            .arg(newImage->getBitsAllocated())
                            .arg(newImage->getBitsStored())
                            .arg(newImage->getPixelRepresentation())
                            .arg(newImage->getRescaleSlope())
                            .arg(formatTextToValidSQLSyntax(windowWidth))
                            .arg(formatTextToValidSQLSyntax(windowCenter))
                            .arg(formatTextToValidSQLSyntax(windowExplanation))
                            .arg(formatTextToValidSQLSyntax(newImage->getSliceLocation()))
                            .arg(newImage->getRescaleIntercept())
                            .arg(formatTextToValidSQLSyntax(newImage->getPhotometricInterpretation().getAsQString()))
                            .arg(formatTextToValidSQLSyntax(newImage->getImageType()))
                            .arg(formatTextToValidSQLSyntax(newImage->getViewPosition()))
                            .arg(formatTextToValidSQLSyntax(newImage->getImageLaterality()))
                            .arg(formatTextToValidSQLSyntax(newImage->getViewCodeMeaning()))
                            .arg(newImage->getPhaseNumber())
                            .arg(formatTextToValidSQLSyntax(newImage->getImageTime()))
                            .arg (newImage->getVolumeNumberInSeries())
                            .arg(newImage->getOrderNumberInVolume())
                            .arg(newImage->getRetrievedDate().toString("yyyyMMdd"))
                            .arg(newImage->getRetrievedTime().toString("hhmmss"))
                            .arg(0)
                            .arg(newImage->getNumberOfOverlays())
                            .arg(getIDPACSInDatabaseFromDICOMSource(newImage->getDICOMSource()))
                            .arg(formatTextToValidSQLSyntax(getImagerPixelSpacingAsQString(newImage)))
                            .arg(newImage->getEstimatedRadiographicMagnificationFactor())
                            .arg(formatTextToValidSQLSyntax(newImage->getTransferSyntaxUID()));

    return insertSentence;
}

QString LocalDatabaseImageDAL::buildSqlUpdate(Image *imageToUpdate)
{
    QString windowWidth, windowCenter, windowExplanation;
    getWindowLevelInformationAsQString(imageToUpdate, windowWidth, windowCenter, windowExplanation);
    
    QString updateSentence = QString("Update Image set StudyInstanceUID = '%1',"
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
                                              "State = %32, "
                                              "NumberOfOverlays = %33, "
                                              "RetrievedPACSID = %34, "
                                              "ImagerPixelSpacing = '%37', "
                                              "EstimatedRadiographicMagnificationFactor = %38, "
                                              "TransferSyntaxUID = '%39' "
                                     "Where SOPInstanceUID = '%35' And "
                                           "FrameNumber = %36")
                            .arg(formatTextToValidSQLSyntax(imageToUpdate->getParentSeries()->getParentStudy()->getInstanceUID()))
                            .arg(formatTextToValidSQLSyntax(imageToUpdate->getParentSeries()->getInstanceUID()))
                            .arg(formatTextToValidSQLSyntax(imageToUpdate->getInstanceNumber()))
                            .arg(formatTextToValidSQLSyntax(imageToUpdate->getImageOrientationPatient().getDICOMFormattedImageOrientation()))
                            .arg(formatTextToValidSQLSyntax(imageToUpdate->getPatientOrientation().getDICOMFormattedPatientOrientation()))
                            .arg(formatTextToValidSQLSyntax(getPixelSpacingAsQString(imageToUpdate)))
                            .arg(imageToUpdate->getSliceThickness())
                            .arg(formatTextToValidSQLSyntax(getPatientPositionAsQString(imageToUpdate)))
                            .arg(imageToUpdate->getSamplesPerPixel())
                            .arg(imageToUpdate->getRows())
                            .arg(imageToUpdate->getColumns())
                            .arg(imageToUpdate->getBitsAllocated())
                            .arg(imageToUpdate->getBitsStored())
                            .arg(imageToUpdate->getPixelRepresentation())
                            .arg(imageToUpdate->getRescaleSlope())
                            .arg(formatTextToValidSQLSyntax(windowWidth))
                            .arg(formatTextToValidSQLSyntax(windowCenter))
                            .arg(formatTextToValidSQLSyntax(windowExplanation))
                            .arg(formatTextToValidSQLSyntax(imageToUpdate->getSliceLocation()))
                            .arg(imageToUpdate->getRescaleIntercept())
                            .arg(formatTextToValidSQLSyntax(imageToUpdate->getPhotometricInterpretation().getAsQString()))
                            .arg(formatTextToValidSQLSyntax(imageToUpdate->getImageType()))
                            .arg(formatTextToValidSQLSyntax(imageToUpdate->getViewPosition()))
                            .arg(formatTextToValidSQLSyntax(imageToUpdate->getImageLaterality()))
                            .arg(formatTextToValidSQLSyntax(imageToUpdate->getViewCodeMeaning()))
                            .arg (imageToUpdate->getPhaseNumber())
                            .arg(formatTextToValidSQLSyntax(imageToUpdate->getImageTime()))
                            .arg (imageToUpdate->getVolumeNumberInSeries())
                            .arg(imageToUpdate->getOrderNumberInVolume())
                            .arg(imageToUpdate->getRetrievedDate().toString("yyyyMMdd"))
                            .arg(imageToUpdate->getRetrievedTime().toString("hhmmss"))
                            .arg(0)
                            .arg(imageToUpdate->getNumberOfOverlays())
                            .arg(getIDPACSInDatabaseFromDICOMSource(imageToUpdate->getDICOMSource()))
                            .arg(formatTextToValidSQLSyntax(imageToUpdate->getSOPInstanceUID()))
                            .arg(imageToUpdate->getFrameNumber())
                            .arg(formatTextToValidSQLSyntax(getImagerPixelSpacingAsQString(imageToUpdate)))
                            .arg(imageToUpdate->getEstimatedRadiographicMagnificationFactor())
                            .arg(formatTextToValidSQLSyntax(imageToUpdate->getTransferSyntaxUID()));

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
    {
        whereSentence = QString("where StudyInstanceUID = '%1'").arg(formatTextToValidSQLSyntax(imageMask.getStudyInstanceUID()));
    }

    if (!imageMask.getSeriesInstanceUID().isEmpty())
    {
        if (whereSentence.isEmpty())
        {
            whereSentence = "where";
        }
        else
        {
            whereSentence += " and ";
        }

        whereSentence += QString(" SeriesInstanceUID = '%1'").arg(formatTextToValidSQLSyntax(imageMask.getSeriesInstanceUID()));
    }

    if (!imageMask.getSOPInstanceUID().isEmpty())
    {
        if (whereSentence.isEmpty())
        {
            whereSentence = "where";
        }
        else
        {
            whereSentence += " and ";
        }

        whereSentence += QString(" SOPInstanceUID = '%1'").arg(formatTextToValidSQLSyntax(imageMask.getSOPInstanceUID()));
    }

    return whereSentence;
}

QString LocalDatabaseImageDAL::getPixelSpacingAsQString(Image *newImage)
{
    QString imagePixelSpacing = "";
    QString value;
    // TODO Add method to PixelSpacing2D getAsDICOMString? or maybe in a helper class
    imagePixelSpacing += value.setNum(newImage->getPixelSpacing().x(), 'g', 10) + "\\";
    imagePixelSpacing += value.setNum(newImage->getPixelSpacing().y(), 'g', 10);

    return imagePixelSpacing;
}

double* LocalDatabaseImageDAL::getPixelSpacingAsDouble(const QString &pixelSpacing)
{
    QStringList list = pixelSpacing.split("\\");

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

QString LocalDatabaseImageDAL::getImagerPixelSpacingAsQString(Image *newImage) const
{
    PixelSpacing2D imagerPixelSpacing = newImage->getImagerPixelSpacing();
    // TODO Add method to PixelSpacing2D getAsDICOMString? or maybe in a helper class
    return QString("%1\\%2").arg(imagerPixelSpacing.x(), 0, 'g', 10).arg(imagerPixelSpacing.y(), 0, 'g', 10);
}

QVector2D LocalDatabaseImageDAL::getImagerPixelSpacingAs2DVector(const QString &imagerPixelSpacing) const
{
    return DICOMValueRepresentationConverter::decimalStringTo2DDoubleVector(imagerPixelSpacing);
}

double* LocalDatabaseImageDAL::getPatientPositionAsDouble(const QString &patientPosition)
{
    QStringList list = patientPosition.split("\\");

    for (int index = 0; index < 3; index++)
    {
        if (list.size() == 3)
        {
            m_patientPosition[index] = list.at(index).toDouble();
        }
        else
        {
            m_patientPosition[index] = 0;
        }
    }

    return m_patientPosition;
}

QString LocalDatabaseImageDAL::getPatientPositionAsQString(Image *newImage)
{
    QString patientPosition = "";
    QString value;

    patientPosition += value.setNum(newImage->getImagePositionPatient()[0], 'g', 10) + "\\";
    patientPosition += value.setNum(newImage->getImagePositionPatient()[1], 'g', 10) + "\\";
    patientPosition += value.setNum(newImage->getImagePositionPatient()[2], 'g', 10);

    return patientPosition;
}

void LocalDatabaseImageDAL::getWindowLevelInformationAsQString(Image *newImage, QString &windowWidth, QString &windowCenter, QString &explanation)
{
    windowWidth.clear();
    windowCenter.clear();
    explanation.clear();
    
    QString value;
    WindowLevel windowLevel;
    for (int index = 0; index < newImage->getNumberOfVoiLuts(); ++index)
    {
        const VoiLut &voiLut = newImage->getVoiLut(index);

        if (voiLut.isWindowLevel())
        {
            windowLevel = voiLut.getWindowLevel();
            windowWidth += value.setNum(windowLevel.getWidth(), 'g', 10) + "\\";
            windowCenter += value.setNum(windowLevel.getCenter(), 'g', 10) + "\\";
            explanation += windowLevel.getName() + "\\";
        }
    }

    // Treiem l'últim "\\" afegit
    windowWidth = windowWidth.left(windowWidth.length() - 1);
    windowCenter = windowCenter.left(windowCenter.length() - 1);
    explanation = explanation.left(explanation.length() - 1);
}

QString LocalDatabaseImageDAL::getIDPACSInDatabaseFromDICOMSource(DICOMSource DICOMSourceRetrievedImage)
{
    if (DICOMSourceRetrievedImage.getRetrievePACS().count() == 0)
    {
        return "null";
    }

    return getIDPACSInDatabase(DICOMSourceRetrievedImage.getRetrievePACS().at(0));
}

QString LocalDatabaseImageDAL::getIDPACSInDatabase(PacsDevice pacsDevice)
{
    QString keyPacsDevice = pacsDevice.getAddress() + QString().setNum(pacsDevice.getQueryRetrieveServicePort());

    if (m_PACSIDCache.contains(keyPacsDevice))
    {
        return m_PACSIDCache[keyPacsDevice];
    }

    LocalDatabasePACSRetrievedImagesDAL localDatabasePACSRetrievedImagesDAL(m_databaseConnection);
    PacsDevice pacsDeviceRetrievedFromDatabase = localDatabasePACSRetrievedImagesDAL.query(pacsDevice.getAETitle(), pacsDevice.getAddress(),
                                                                                pacsDevice.getQueryRetrieveServicePort());
    if (!pacsDeviceRetrievedFromDatabase.getID().isEmpty())
    {
        //El pacs ja està inserit a la base de dades
        m_PACSIDCache[keyPacsDevice] = pacsDeviceRetrievedFromDatabase.getID();
        return pacsDeviceRetrievedFromDatabase.getID();
    }

    //No hem trobat el PACS l'inserir a la base de dades
    qlonglong PACSDInDatabase = localDatabasePACSRetrievedImagesDAL.insert(pacsDevice);

    if (PACSDInDatabase >= 0)
    {
        m_PACSIDCache[keyPacsDevice] = QString().setNum(PACSDInDatabase);
        return m_PACSIDCache[keyPacsDevice];
    }

    return "null";
}

DICOMSource LocalDatabaseImageDAL::getImageDICOMSourceByIDPACSInDatabase(const QVariant &retrievedPACSID)
{
    DICOMSource imageDICOMSource;

    if (retrievedPACSID.isNull())
    {
        // La imatge no té de quin PACS s'ha descarregat
        return imageDICOMSource;
    }

    PacsDevice pacsDevice = getPACSDeviceByIDPACSInDatabase(retrievedPACSID.toInt());

    if (!pacsDevice.getID().isEmpty())
    {
        imageDICOMSource.addRetrievePACS(pacsDevice);
    }

    return imageDICOMSource;
}

PacsDevice LocalDatabaseImageDAL::getPACSDeviceByIDPACSInDatabase(int IDPACSInDatabase)
{
    if (m_PACSDeviceCacheByIDPACSInDatabase.contains(IDPACSInDatabase))
    {
        return m_PACSDeviceCacheByIDPACSInDatabase[IDPACSInDatabase];
    }

    //Sinó el trobem a la caché el recuperem de la base de dades
    LocalDatabasePACSRetrievedImagesDAL localDatabasePACSRetrievedImagesDAL(m_databaseConnection);
    PacsDevice pacsDevice = localDatabasePACSRetrievedImagesDAL.query(IDPACSInDatabase);

    if (!pacsDevice.getID().isEmpty())
    {
        m_PACSDeviceCacheByIDPACSInDatabase[IDPACSInDatabase] = pacsDevice;
    }

    return pacsDevice;
}

}
