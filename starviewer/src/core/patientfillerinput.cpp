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
#include "logging.h"
#include "patient.h"
#include "dicomtagreader.h"

namespace udg {

PatientFillerInput::PatientFillerInput(): m_dicomFile(0), m_currentSeries(0)
{
}

PatientFillerInput::~PatientFillerInput()
{
    delete m_dicomFile;
}

void PatientFillerInput::addPatient(Patient *patient)
{
    if (patient)
    {
        m_patientList << patient;
    }
    else
    {
        DEBUG_LOG("S'ha passat un pacient NUL, per tant no s'ha afegit res a la llista");
    }
}

Patient *PatientFillerInput::getPatient(int index)
{
    Patient *patient = 0;
    if (index < m_patientList.size())
    {
        patient = m_patientList.at(index);
    }
    else
    {
        DEBUG_LOG("Índex fora de rang");
    }

    return patient;
}

Patient *PatientFillerInput::getPatientByName(QString name)
{
    bool found = false;
    int i = 0;
    Patient *patient = 0;

    while (i < m_patientList.size() && !found)
    {
        if (m_patientList.at(i)->getFullName() == name)
        {
            patient = m_patientList[i];
            found = true;
        }
        i++;
    }

    return patient;
}

Patient *PatientFillerInput::getPatientByID(QString id)
{
    bool found = false;
    int i = 0;
    Patient *patient = 0;

    while (i < m_patientList.size() && !found)
    {
        if (m_patientList.at(i)->getID() == id)
        {
            patient = m_patientList[i];
            found = true;
        }
        i++;
    }

    return patient;
}

unsigned int PatientFillerInput::getNumberOfPatients()
{
    return m_patientList.size();
}

void PatientFillerInput::setFile(QString file)
{
    m_file = file;
}

QList<Patient*> PatientFillerInput::getPatientsList()
{
    return m_patientList;
}

QString PatientFillerInput::getFile() const
{
    return m_file;
}

void PatientFillerInput::setDICOMFile(const DICOMTagReader *dicomTagReader)
{
    if (m_dicomFile)
    {
        delete m_dicomFile;
    }

    m_dicomFile = dicomTagReader;
}

const DICOMTagReader* PatientFillerInput::getDICOMFile()
{
    return m_dicomFile;
}

void PatientFillerInput::setCurrentImages(const QList<Image*> &images, bool addToHistory)
{
    m_currentImages = images;

    if (addToHistory)
    {
        m_perSeriesCurrentImagesList[m_currentSeries].append(images);

        if (images.size() > 1)
        {
            m_seriesContainsMultiframeImages[m_currentSeries] = true;
        }
    }
}

QList<Image*> PatientFillerInput::getCurrentImages()
{
    return m_currentImages;
}

QList<QList<Image*>> PatientFillerInput::getCurrentImagesList() const
{
    return m_perSeriesCurrentImagesList[m_currentSeries];
}

void PatientFillerInput::setCurrentSeries(Series *series)
{
    m_currentSeries = series;

    if (!m_perSeriesCurrentVolumeNumber.contains(series))
    {
        m_perSeriesCurrentVolumeNumber[series] = 1;
        m_seriesContainsMultiframeImages[series] = false;
    }
}

Series* PatientFillerInput::getCurrentSeries()
{
    return m_currentSeries;
}

bool PatientFillerInput::currentSeriesContainsMultiframeImages() const
{
    return m_seriesContainsMultiframeImages[m_currentSeries];
}

void PatientFillerInput::setCurrentVolumeNumber(int volumeNumber)
{
    m_perSeriesCurrentVolumeNumber[m_currentSeries] = volumeNumber;
}

int PatientFillerInput::getCurrentVolumeNumber() const
{
    return m_perSeriesCurrentVolumeNumber[m_currentSeries];
}

void PatientFillerInput::setDICOMSource(const DICOMSource &imagesDICOMSource)
{
    m_imagesDICOMSource = imagesDICOMSource;
}

DICOMSource PatientFillerInput::getDICOMSource() const
{
    return m_imagesDICOMSource;
}

}
