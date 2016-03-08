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

#include "patientfillerinput.h"

#include "dicomtagreader.h"
#include "logging.h"
#include "patient.h"

namespace udg {

PatientFillerInput::PatientFillerInput()
    : m_dicomFile(nullptr), m_currentSeries(nullptr)
{
}

PatientFillerInput::~PatientFillerInput()
{
    delete m_dicomFile;
}

const DICOMSource& PatientFillerInput::getDICOMSource() const
{
    return m_imagesDICOMSource;
}

void PatientFillerInput::setDICOMSource(DICOMSource imagesDICOMSource)
{
    m_imagesDICOMSource = std::move(imagesDICOMSource);
}

const DICOMTagReader* PatientFillerInput::getDICOMFile() const
{
    return m_dicomFile;
}

void PatientFillerInput::setDICOMFile(const DICOMTagReader *dicomTagReader)
{
    delete m_dicomFile;
    m_dicomFile = dicomTagReader;
}

const QString& PatientFillerInput::getFile() const
{
    return m_file;
}

void PatientFillerInput::setFile(QString file)
{
    m_file = std::move(file);
}

void PatientFillerInput::addPatient(Patient *patient)
{
    if (patient)
    {
        m_patientList << patient;
    }
    else
    {
        DEBUG_LOG("Ignoring null patient");
    }
}

Patient* PatientFillerInput::getPatient(int index) const
{
    if (index >= 0 && index < m_patientList.size())
    {
        return m_patientList[index];
    }
    else
    {
        DEBUG_LOG("Index out of range");
        return nullptr;
    }
}

Patient* PatientFillerInput::getPatientByID(const QString &id) const
{
    foreach (Patient *patient, m_patientList)
    {
        if (patient->getID() == id)
        {
            return patient;
        }
    }

    return nullptr;
}

int PatientFillerInput::getNumberOfPatients() const
{
    return m_patientList.size();
}

const QList<Patient*>& PatientFillerInput::getPatientList() const
{
    return m_patientList;
}

Series* PatientFillerInput::getCurrentSeries() const
{
    return m_currentSeries;
}

void PatientFillerInput::setCurrentSeries(Series *series)
{
    m_currentSeries = series;

    // If it's a new series
    if (!m_perSeriesCurrentImagesHistory.contains(series))
    {
        m_seriesContainsMultiframeImages[series] = false;
        m_perSeriesCurrentVolumeNumber[series] = 1;
    }
}

const QList<Image*>& PatientFillerInput::getCurrentImages() const
{
    return m_currentImages;
}

void PatientFillerInput::setCurrentImages(const QList<Image*> &images, bool addToHistory)
{
    m_currentImages = images;

    if (addToHistory)
    {
        m_perSeriesCurrentImagesHistory[m_currentSeries].append(images);

        if (images.size() > 1)
        {
            m_seriesContainsMultiframeImages[m_currentSeries] = true;
        }
    }
}

QList<QList<Image*>> PatientFillerInput::getCurrentImagesHistory() const
{
    return m_perSeriesCurrentImagesHistory[m_currentSeries];
}

bool PatientFillerInput::currentSeriesContainsMultiframeImages() const
{
    return m_seriesContainsMultiframeImages[m_currentSeries];
}

int PatientFillerInput::getCurrentVolumeNumber() const
{
    return m_perSeriesCurrentVolumeNumber[m_currentSeries];
}

void PatientFillerInput::setCurrentVolumeNumber(int volumeNumber)
{
    m_perSeriesCurrentVolumeNumber[m_currentSeries] = volumeNumber;
}

}
