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

#include "nondicomfileclassifierfillerstep.h"

#include "image.h"
#include "logging.h"
#include "patient.h"
#include "patientfillerinput.h"

#include <QCryptographicHash>
#include <QFileInfo>
#include <QRegularExpression>

#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkSmartPointer.h>

namespace udg {

namespace {

// Regular expression to detect if a file name may follow a pattern. Currently it just checks for numbers at the end.
QRegularExpression patternRegularExpression("\\d+$");

// Returns true if the given file may contain a pattern in its name.
bool mayHavePattern(const QFileInfo &fileInfo)
{
    return patternRegularExpression.match(fileInfo.completeBaseName()).hasMatch();
}

// Returns the canonical file path having changed the pattern part by asterisks.
QString markPattern(const QFileInfo &fileInfo)
{
    QString base = fileInfo.completeBaseName();
    auto match = patternRegularExpression.match(base);
    base.remove(patternRegularExpression).append(match.captured(0).fill('*'));  // remove the pattern part and append the same number of asterisks in its place
    QString newFileName = fileInfo.fileName().replace(0, base.length(), base);
    return fileInfo.canonicalPath() + "/" + newFileName;    // in Windows canonicalPath() uses '/' as a separator too
}

// Returns a series number derived from the file name: currently the first 4 characters of the MD4 digest of the canonical file path.
QString seriesNumber(const QFileInfo &fileInfo)
{
    return QCryptographicHash::hash(qPrintable(fileInfo.canonicalFilePath()), QCryptographicHash::Md4).toHex().left(4);
}

// Creates an image and fills it from the given file name and reader.
Image* createImage(QString fileName, vtkImageReader2 *reader)
{
    int *extent = reader->GetDataExtent();
    double *spacing = reader->GetDataSpacing();
    Image *image = new Image();
    image->setPath(std::move(fileName));
    image->setRows(extent[3] - extent[2] + 1);
    image->setColumns(extent[1] - extent[0] + 1);
    image->setPixelSpacing(spacing[0], spacing[1]);
    image->setSliceThickness(spacing[2]);
    image->setSamplesPerPixel(reader->GetNumberOfScalarComponents());
    image->setImageOrientationPatient(ImageOrientation(QVector3D(1, 0, 0), QVector3D(0, 1, 0)));
    return image;
}

}

NonDicomFileClassifierFillerStep::NonDicomFileClassifierFillerStep()
{
}

NonDicomFileClassifierFillerStep::~NonDicomFileClassifierFillerStep()
{
}

bool NonDicomFileClassifierFillerStep::fillIndividually()
{
    Q_ASSERT(m_input);

    QString fileName = m_input->getFile();
    vtkSmartPointer<vtkImageReader2> reader = vtkImageReader2Factory::CreateImageReader2(qPrintable(fileName));

    if (!reader)
    {
        WARN_LOG("No suitable vtkImageReader2 subclass found for " + fileName);
        return false;
    }

    reader->SetFileName(qPrintable(fileName));
    reader->UpdateInformation();
    int *extent = reader->GetDataExtent();
    int numberOfFrames = extent[5] - extent[4] + 1;

    if (numberOfFrames == 0)
    {
        WARN_LOG("Empty image (0 frames)");
        return false;
    }

    QString patientId("Non-DICOM Patient");
    QString studyUid(QString("1.2.3.4.%1").arg(reader->GetDescriptiveName()));

    Patient *patient = m_input->getPatientByID(patientId);

    if (!patient)
    {
        patient = new Patient();
        patient->setID(patientId);
        patient->setFullName(QObject::tr("Non-DICOM Patient"));
        m_input->addPatient(patient);
    }

    Study *study = patient->getStudy(studyUid);

    if (!study)
    {
        study = new Study();
        study->setInstanceUID(studyUid);
        study->setID(QString("%1 Study").arg(reader->GetDescriptiveName()));
        study->setDate(QDate::currentDate());
        study->setTime(QTime::currentTime());
        study->setDescription(QString(QObject::tr("%1 Study")).arg(reader->GetDescriptiveName()));
        patient->addStudy(study);
    }

    Image *image = createImage(fileName, reader);
    QFileInfo fileInfo(fileName);
    QString seriesUid;

    if (numberOfFrames == 1 && mayHavePattern(fileInfo))
    {
        // Series will be shared with other files with the same pattern
        seriesUid = markPattern(fileInfo);
    }
    else
    {
        // Create new series for the single file
        seriesUid = fileInfo.canonicalFilePath();
    }

    Series *series = study->getSeries(seriesUid);

    if (!series)
    {
        series = new Series();
        series->setInstanceUID(seriesUid);
        series->setSeriesNumber(seriesNumber(fileInfo));
        series->setDate(fileInfo.lastModified().date());
        series->setTime(fileInfo.lastModified().time());
        series->setDescription(fileInfo.fileName());
        series->setFrameOfReferenceUID(seriesUid);
        study->addSeries(series);
    }

    QString imageUid(QString("%1.%2").arg(seriesUid).arg(series->getNumberOfImages()));
    Vector3 origin(reader->GetDataOrigin());
    QList<Image*> generatedImages;

    for (int i = extent[4]; i <= extent[5]; i++)
    {
        // Reuse the already reated image for the first frame, create new images starting from the second frame
        if (i > extent[4])
        {
            image = createImage(fileName, reader);
        }

        image->setSOPInstanceUID(imageUid);
        image->setFrameNumber(i);
        image->setImagePositionPatient(origin.toArray().data());
        origin.z += image->getSliceThickness();

        if (series->addImage(image))
        {
            generatedImages << image;
        }
        else
        {
            delete image;
        }
    }

    m_input->setCurrentSeries(series);

    if (!generatedImages.isEmpty())
    {
        m_input->setCurrentImages(generatedImages);
    }

    return true;
}

} // namespace udg
