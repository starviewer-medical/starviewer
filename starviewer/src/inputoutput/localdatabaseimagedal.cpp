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

#include "dicomformattedvaluesconverter.h"
#include "dicommask.h"
#include "image.h"
#include "localdatabasedisplayshutterdal.h"
#include "localdatabasemanager.h"
#include "localdatabasepacsretrievedimagesdal.h"
#include "localdatabasevoilutdal.h"
#include "series.h"
#include "study.h"

#include <array>

#include <QDate>
#include <QSqlQuery>
#include <QVariant>
#include <QVector2D>

namespace udg {

namespace {

// Returns pixel spacing formatted as a DICOM string, with values separated by "\\".
QString pixelSpacingToDicomString(const PixelSpacing2D &pixelSpacing)
{
    return QString("%1\\%2").arg(pixelSpacing.x(), 0, 'g', 10).arg(pixelSpacing.y(), 0, 'g', 10);
}

// Returns pixel spacing constructed from a formatted DICOM string, with values separated by "\\".
PixelSpacing2D pixelSpacingFromDicomString(const QString &pixelSpacingString)
{
    QStringList values = pixelSpacingString.split("\\");

    if (values.size() == 2)
    {
        return PixelSpacing2D(values.at(0).toDouble(), values.at(1).toDouble());
    }
    else
    {
        return PixelSpacing2D();
    }
}

// Returns image position patient as a DICOM string, with values separated by "\\".
QString imagePositionPatientToDicomString(const double imagePositionPatient[3])
{
    return QString("%1\\%2\\%3").arg(imagePositionPatient[0], 0, 'g', 10).arg(imagePositionPatient[1], 0, 'g', 10).arg(imagePositionPatient[2], 0, 'g', 10);
}

// Returns image position patient constructed from a formatted DICOM string, with values separated by "\\".
std::array<double, 3> imagePositionPatientFromDicomString(const QString &imagePositionPatientString)
{
    QStringList values = imagePositionPatientString.split("\\");
    std::array<double, 3> imagePositionPatient = { 0, 0, 0 };

    for (int i = 0; i < 3; i++)
    {
        if (values.size() == 3)
        {
            imagePositionPatient[i] = values.at(i).toDouble();
        }
    }

    return imagePositionPatient;
}

// Returns window widths, window centers and explanations formatted as DICOM strings, with values separated by "\\".
void windowLevelInformationToDicomStrings(const Image *image, QString &windowWidth, QString &windowCenter, QString &explanation)
{
    windowWidth.clear();
    windowCenter.clear();
    explanation.clear();

    for (int i = 0; i < image->getNumberOfVoiLuts(); ++i)
    {
        const VoiLut &voiLut = image->getVoiLut(i);

        if (voiLut.isWindowLevel())
        {
            if (!windowWidth.isEmpty())
            {
                windowWidth += "\\";
                windowCenter += "\\";
                explanation += "\\";
            }

            const WindowLevel &windowLevel = voiLut.getWindowLevel();
            windowWidth += QString::number(windowLevel.getWidth(), 'g', 10);
            windowCenter += QString::number(windowLevel.getCenter(), 'g', 10);
            explanation += windowLevel.getName();
        }
    }
}

// Prepares the given query with the given SQL base command followed by the appropriate where clause according to the given mask
// followed by the given SQL continuation (order by, group by, etc.).
void prepareQueryWithMask(QSqlQuery &query, const DicomMask &mask, const QString &sqlCommand, const QString &sqlContinuation = QString())
{
    QString where;

    if (!mask.getStudyInstanceUID().isEmpty())
    {
        where += "StudyInstanceUID = :studyInstanceUID";
    }
    if (!mask.getSeriesInstanceUID().isEmpty())
    {
        if (!where.isEmpty())
        {
            where += " AND ";
        }
        where += "SeriesInstanceUID = :seriesInstanceUID";
    }
    if (!mask.getSOPInstanceUID().isEmpty())
    {
        if (!where.isEmpty())
        {
            where += " AND ";
        }
        where += "SOPInstanceUID = :sopInstanceUID";
    }
    if (!where.isEmpty())
    {
        where = " WHERE " + where;
    }

    query.prepare(sqlCommand + where + sqlContinuation);

    if (!mask.getStudyInstanceUID().isEmpty())
    {
        query.bindValue(":studyInstanceUID", mask.getStudyInstanceUID());
    }
    if (!mask.getSeriesInstanceUID().isEmpty())
    {
        query.bindValue(":seriesInstanceUID", mask.getSeriesInstanceUID());
    }
    if (!mask.getSOPInstanceUID().isEmpty())
    {
        query.bindValue(":sopInstanceUID", mask.getSOPInstanceUID());
    }
}

}

LocalDatabaseImageDAL::LocalDatabaseImageDAL(DatabaseConnection &databaseConnection)
 : LocalDatabaseBaseDAL(databaseConnection)
{
}

bool LocalDatabaseImageDAL::insert(const Image *image)
{
    QSqlQuery query = getNewQuery();
    query.prepare("INSERT INTO Image (SOPInstanceUID, FrameNumber, StudyInstanceUID, SeriesInstanceUID, InstanceNumber, ImageOrientationPatient, "
                                     "PatientOrientation, PixelSpacing, SliceThickness, PatientPosition, SamplesPerPixel, Rows, Columns, BitsAllocated, "
                                     "BitsStored, PixelRepresentation, RescaleSlope, WindowLevelWidth, WindowLevelCenter, WindowLevelExplanations, "
                                     "SliceLocation, RescaleIntercept, PhotometricInterpretation, ImageType, ViewPosition, ImageLaterality, ViewCodeMeaning, "
                                     "PhaseNumber, ImageTime, VolumeNumberInSeries, OrderNumberInVolume, RetrievedDate, RetrievedTime, State, "
                                     "NumberOfOverlays, RetrievedPACSID, ImagerPixelSpacing, EstimatedRadiographicMagnificationFactor, TransferSyntaxUID) "
                  "VALUES (:sopInstanceUID, :frameNumber, :studyInstanceUID, :seriesInstanceUID, :instanceNumber, :imageOrientationPatient, "
                          ":patientOrientation, :pixelSpacing, :sliceThickness, :patientPosition, :samplesPerPixel, :rows, :columns, :bitsAllocated, "
                          ":bitsStored, :pixelRepresentation, :rescaleSlope, :windowLevelWidth, :windowLevelCenter, :windowLevelExplanations, "
                          ":sliceLocation, :rescaleIntercept, :photometricInterpretation, :imageType, :viewPosition, :imageLaterality, :viewCodeMeaning, "
                          ":phaseNumber, :imageTime, :volumeNumberInSeries, :orderNumberInVolume, :retrievedDate, :retrievedTime, :state, "
                          ":numberOfOverlays, :retrievedPacsId, :imagerPixelSpacing, :estimatedRadiographicMagnificationFactor, :transferSyntaxUID)");
    bindValues(query, image);
    return executeQueryAndLogError(query);
}

bool LocalDatabaseImageDAL::update(const Image *image)
{
    QSqlQuery query = getNewQuery();
    query.prepare("UPDATE Image SET StudyInstanceUID = :studyInstanceUID, SeriesInstanceUID = :seriesInstanceUID, InstanceNumber = :instanceNumber, "
                                   "ImageOrientationPatient = :imageOrientationPatient, PatientOrientation = :patientOrientation, "
                                   "PixelSpacing = :pixelSpacing, SliceThickness = :sliceThickness, PatientPosition = :patientPosition, "
                                   "SamplesPerPixel = :samplesPerPixel, Rows = :rows, Columns = :columns, BitsAllocated = :bitsAllocated, "
                                   "BitsStored = :bitsStored, PixelRepresentation = :pixelRepresentation, RescaleSlope = :rescaleSlope, "
                                   "WindowLevelWidth = :windowLevelWidth, WindowLevelCenter = :windowLevelCenter, "
                                   "WindowLevelExplanations = :windowLevelExplanations, SliceLocation = :sliceLocation, RescaleIntercept = :rescaleIntercept, "
                                   "PhotometricInterpretation = :photometricInterpretation, ImageType = :imageType, ViewPosition = :viewPosition, "
                                   "ImageLaterality = :imageLaterality, ViewCodeMeaning = :viewCodeMeaning, PhaseNumber = :phaseNumber, "
                                   "ImageTime = :imageTime, VolumeNumberInSeries = :volumeNumberInSeries, OrderNumberInVolume = :orderNumberInVolume, "
                                   "RetrievedDate = :retrievedDate, RetrievedTime = :retrievedTime, State = :state, NumberOfOverlays = :numberOfOverlays, "
                                   "RetrievedPACSID = :retrievedPacsId, ImagerPixelSpacing = :imagerPixelSpacing, "
                                   "EstimatedRadiographicMagnificationFactor = :estimatedRadiographicMagnificationFactor, "
                                   "TransferSyntaxUID = :transferSyntaxUID "
                  "WHERE SOPInstanceUID = :sopInstanceUID AND FrameNumber = :frameNumber");
    bindValues(query, image);
    return executeQueryAndLogError(query);
}

bool LocalDatabaseImageDAL::del(const DicomMask &mask)
{
    QSqlQuery query = getNewQuery();
    prepareQueryWithMask(query, mask, "DELETE FROM Image");
    return executeQueryAndLogError(query);
}

QList<Image*> LocalDatabaseImageDAL::query(const DicomMask &mask)
{
    QSqlQuery query = getNewQuery();
    QString select("SELECT SOPInstanceUID, FrameNumber, StudyInstanceUID, SeriesInstanceUID, InstanceNumber, ImageOrientationPatient, PatientOrientation, "
                          "PixelSpacing, SliceThickness, PatientPosition, SamplesPerPixel, Rows, Columns, BitsAllocated, BitsStored, PixelRepresentation, "
                          "RescaleSlope, WindowLevelWidth, WindowLevelCenter, WindowLevelExplanations, SliceLocation, RescaleIntercept, "
                          "PhotometricInterpretation, ImageType, ViewPosition, ImageLaterality, ViewCodeMeaning, PhaseNumber, ImageTime, VolumeNumberInSeries, "
                          "OrderNumberInVolume, RetrievedDate, RetrievedTime, State, NumberOfOverlays, RetrievedPACSID, ImagerPixelSpacing, "
                          "EstimatedRadiographicMagnificationFactor, TransferSyntaxUID "
                   "FROM Image");
    QString orderBy(" ORDER BY VolumeNumberInSeries, OrderNumberInVolume");
    prepareQueryWithMask(query, mask, select, orderBy);
    QList<Image*> imageList;

    if (executeQueryAndLogError(query))
    {
        LocalDatabaseDisplayShutterDAL shutterDAL(m_databaseConnection);
        LocalDatabaseVoiLutDAL voiLutDAL(m_databaseConnection);

        while (query.next())
        {
            Image *image = getImage(query);

            // Get display shutters
            DicomMask mask;
            mask.setSOPInstanceUID(image->getSOPInstanceUID());
            mask.setImageNumber(QString::number(image->getFrameNumber()));
            image->setDisplayShutters(shutterDAL.query(mask));

            // Get VOI LUTs
            QList<VoiLut> voiLuts = voiLutDAL.query(mask);
            foreach (const VoiLut &voiLut, voiLuts)
            {
                image->addVoiLut(voiLut);
            }

            imageList << image;
        }
    }

    return imageList;
}

int LocalDatabaseImageDAL::count(const DicomMask &mask)
{
    QSqlQuery query = getNewQuery();
    prepareQueryWithMask(query, mask, "SELECT count(*) FROM Image");

    if (executeQueryAndLogError(query) && query.next())
    {
        return query.value(0).toInt();
    }
    else
    {
        return -1;
    }
}

void LocalDatabaseImageDAL::bindValues(QSqlQuery &query, const Image *image)
{
    query.bindValue(":sopInstanceUID", image->getSOPInstanceUID());
    query.bindValue(":frameNumber", image->getFrameNumber());
    query.bindValue(":studyInstanceUID", image->getParentSeries()->getParentStudy()->getInstanceUID());
    query.bindValue(":seriesInstanceUID", image->getParentSeries()->getInstanceUID());
    query.bindValue(":instanceNumber", image->getInstanceNumber());
    query.bindValue(":imageOrientationPatient", image->getImageOrientationPatient().getDICOMFormattedImageOrientation());
    query.bindValue(":patientOrientation", image->getPatientOrientation().getDICOMFormattedPatientOrientation());
    query.bindValue(":pixelSpacing", pixelSpacingToDicomString(image->getPixelSpacing()));
    query.bindValue(":sliceThickness", image->getSliceThickness());
    query.bindValue(":patientPosition", imagePositionPatientToDicomString(image->getImagePositionPatient()));
    query.bindValue(":samplesPerPixel", image->getSamplesPerPixel());
    query.bindValue(":rows", image->getRows());
    query.bindValue(":columns", image->getColumns());
    query.bindValue(":bitsAllocated", image->getBitsAllocated());
    query.bindValue(":bitsStored", image->getBitsStored());
    query.bindValue(":pixelRepresentation", image->getPixelRepresentation());
    query.bindValue(":rescaleSlope", image->getRescaleSlope());
    QString windowWidth, windowCenter, windowExplanation;
    windowLevelInformationToDicomStrings(image, windowWidth, windowCenter, windowExplanation);
    query.bindValue(":windowLevelWidth", windowWidth);
    query.bindValue(":windowLevelCenter", windowCenter);
    query.bindValue(":windowLevelExplanations", windowExplanation);
    query.bindValue(":sliceLocation", image->getSliceLocation());
    query.bindValue(":rescaleIntercept", image->getRescaleIntercept());
    query.bindValue(":photometricInterpretation", image->getPhotometricInterpretation().getAsQString());
    query.bindValue(":imageType", image->getImageType());
    query.bindValue(":viewPosition", image->getViewPosition());
    query.bindValue(":imageLaterality", convertToQString(image->getImageLaterality()));
    query.bindValue(":viewCodeMeaning", image->getViewCodeMeaning());
    query.bindValue(":phaseNumber", image->getPhaseNumber());
    query.bindValue(":imageTime", image->getImageTime());
    query.bindValue(":volumeNumberInSeries", image->getVolumeNumberInSeries());
    query.bindValue(":orderNumberInVolume", image->getOrderNumberInVolume());
    query.bindValue(":retrievedDate", image->getRetrievedDate().toString("yyyyMMdd"));
    query.bindValue(":retrievedTime", image->getRetrievedTime().toString("hhmmss"));
    query.bindValue(":state", 0);
    query.bindValue(":numberOfOverlays", image->getNumberOfOverlays());
    query.bindValue(":retrievedPacsId", getDatabasePacsId(image->getDICOMSource()));
    query.bindValue(":imagerPixelSpacing", pixelSpacingToDicomString(image->getImagerPixelSpacing()));
    query.bindValue(":estimatedRadiographicMagnificationFactor", QString::number(image->getEstimatedRadiographicMagnificationFactor()));
    query.bindValue(":transferSyntaxUID", image->getTransferSyntaxUID());
}

Image* LocalDatabaseImageDAL::getImage(const QSqlQuery &query)
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
    image->setPixelSpacing(pixelSpacingFromDicomString(query.value("PixelSpacing").toString()));
    image->setSliceThickness(query.value("SliceThickness").toString().toDouble());
    image->setImagePositionPatient(imagePositionPatientFromDicomString(query.value("PatientPosition").toString()).data());
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
    // ImageLaterality is a char
    image->setImageLaterality(query.value("ImageLaterality").toString()[0]);
    image->setViewCodeMeaning(convertToQString(query.value("ViewCodeMeaning")));
    image->setPhaseNumber(query.value("PhaseNumber").toInt());
    image->setImageTime(query.value("ImageTime").toString());
    image->setVolumeNumberInSeries(query.value("VolumeNumberInSeries").toInt());
    image->setOrderNumberInVolume(query.value("OrderNumberInVolume").toInt());
    image->setRetrievedDate(QDate::fromString(query.value("RetrievedDate").toString(), "yyyyMMdd"));
    image->setRetrievedTime(QTime::fromString(query.value("RetrievedTime").toString(), "hhmmss"));
    image->setNumberOfOverlays(query.value("NumberOfOverlays").toUInt());
    image->setDICOMSource(getDicomSource(query.value("RetrievedPACSID")));
    image->setImagerPixelSpacing(pixelSpacingFromDicomString(query.value("ImagerPixelSpacing").toString()));
    image->setEstimatedRadiographicMagnificationFactor(query.value("EstimatedRadiographicMagnificationFactor").toString().toDouble());
    image->setTransferSyntaxUID(query.value("TransferSyntaxUID").toString());

    // TODO argghh!!! Això només hauria d'estar en un únic lloc, no aquí i en retrieveimages.cpp
    image->setPath(LocalDatabaseManager::getCachePath() + query.value("StudyInstanceUID").toString() + "/" + query.value("SeriesInstanceUID").toString() + "/" +
                   query.value("SOPInstanceUID").toString());

    return image;
}

QVariant LocalDatabaseImageDAL::getDatabasePacsId(const DICOMSource &dicomSource)
{
    if (dicomSource.getRetrievePACS().isEmpty())
    {
        return QVariant(QVariant::LongLong);
    }

    return getDatabasePacsId(dicomSource.getRetrievePACS().first());
}

QVariant LocalDatabaseImageDAL::getDatabasePacsId(const PacsDevice &pacsDevice)
{
    QString key = pacsDevice.getAddress() + QString::number(pacsDevice.getQueryRetrieveServicePort());

    if (m_databasePacsIdCache.contains(key))
    {
        return m_databasePacsIdCache[key];
    }

    LocalDatabasePACSRetrievedImagesDAL localDatabasePACSRetrievedImagesDAL(m_databaseConnection);
    PacsDevice pacsDeviceRetrievedFromDatabase = localDatabasePACSRetrievedImagesDAL.query(pacsDevice.getAETitle(), pacsDevice.getAddress(),
                                                                                           pacsDevice.getQueryRetrieveServicePort());

    if (!pacsDeviceRetrievedFromDatabase.getID().isEmpty())
    {
        // PACS is in the database
        m_databasePacsIdCache[key] = pacsDeviceRetrievedFromDatabase.getID().toLongLong();
        return pacsDeviceRetrievedFromDatabase.getID();
    }
    else
    {
        // PACS is not in the database. Let's insert it
        qlonglong databasePacsId = localDatabasePACSRetrievedImagesDAL.insert(pacsDevice);

        if (databasePacsId >= 0)
        {
            m_databasePacsIdCache[key] = databasePacsId;
            return m_databasePacsIdCache[key];
        }
        else
        {
            return QVariant(QVariant::LongLong);
        }
    }
}

DICOMSource LocalDatabaseImageDAL::getDicomSource(const QVariant &retrievedPacsId)
{
    DICOMSource dicomSource;

    if (!retrievedPacsId.isNull())
    {
        PacsDevice pacsDevice = getPacsDevice(retrievedPacsId.toLongLong());

        if (!pacsDevice.getID().isEmpty())
        {
            dicomSource.addRetrievePACS(pacsDevice);
        }
    }

    return dicomSource;
}

PacsDevice LocalDatabaseImageDAL::getPacsDevice(qlonglong retrievedPacsId)
{
    if (m_pacsDeviceCache.contains(retrievedPacsId))
    {
        return m_pacsDeviceCache[retrievedPacsId];
    }

    LocalDatabasePACSRetrievedImagesDAL localDatabasePACSRetrievedImagesDAL(m_databaseConnection);
    PacsDevice pacsDevice = localDatabasePACSRetrievedImagesDAL.query(retrievedPacsId);

    if (!pacsDevice.getID().isEmpty())
    {
        m_pacsDeviceCache[retrievedPacsId] = pacsDevice;
    }

    return pacsDevice;
}

}
