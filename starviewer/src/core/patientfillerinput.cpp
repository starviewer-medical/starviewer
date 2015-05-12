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

PatientFillerInput::PatientFillerInput(): m_dicomFile(0), m_currentSeries(0), m_currentVolumeNumber(0)
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

void PatientFillerInput::addLabel(QString label)
{
    if (!m_globalLabels.contains(label))
    {
        m_globalLabels << label;
        // Afegim a la llista de tots també
        m_allLabels << label;
    }
}

void PatientFillerInput::addLabelToSeries(QString label, Series *series)
{
    if (!m_seriesLabels.values(series).contains(label))
    {
        m_seriesLabels.insert(series, label);
    }
    // Aquí ho separem perquè podria ser que la serie que especifiquem no tingui aquella label i una altre sí i s'hagi afegit ja abans
    if (!m_allLabels.contains(label))
    {
        m_allLabels << label;
    }
}

QStringList PatientFillerInput::getLabels() const
{
    return m_allLabels;
}

bool PatientFillerInput::hasAllLabels(QStringList requiredLabelsList) const
{
    foreach (const QString &requiredLabel, requiredLabelsList)
    {
        if (!getLabels().contains(requiredLabel))
        {
            return false;
        }
    }
    return true;
}

void PatientFillerInput::initializeAllLabels()
{
    while (!m_allLabels.isEmpty())
    {
        m_allLabels.removeFirst();
    }
    while (!m_globalLabels.isEmpty())
    {
        m_globalLabels.removeFirst();
    }
    foreach (Series *key, m_seriesLabels.keys())
    {
        m_seriesLabels.remove(key);
    }
}

void PatientFillerInput::setDICOMFile(DICOMTagReader *dicomTagReader)
{
    if (m_dicomFile)
    {
        delete m_dicomFile;
    }

    m_dicomFile = dicomTagReader;
}

DICOMTagReader* PatientFillerInput::getDICOMFile()
{
    return m_dicomFile;
}

void PatientFillerInput::setCurrentImages(const QList<Image*> &images)
{
    m_currentImages = images;
}

QList<Image*> PatientFillerInput::getCurrentImages()
{
    return m_currentImages;
}

void PatientFillerInput::setCurrentSeries(Series *series)
{
    m_currentSeries = series;
}

Series* PatientFillerInput::getCurrentSeries()
{
    return m_currentSeries;
}

void PatientFillerInput::increaseCurrentMultiframeVolumeNumber()
{
    if (m_currentSeries)
    {
        m_currentMultiframeVolumeNumber.insert(m_currentSeries, getCurrentMultiframeVolumeNumber() + 1);
    }
}

int PatientFillerInput::getCurrentMultiframeVolumeNumber()
{
    if (m_currentSeries)
    {
        // Insert default value if it doesn't exist
        if (!m_currentMultiframeVolumeNumber.contains(m_currentSeries))
        {
            m_currentMultiframeVolumeNumber[m_currentSeries] = 1;
        }

        return m_currentMultiframeVolumeNumber[m_currentSeries];
    }
    else
    {
        return -1;
    }
}

bool PatientFillerInput::currentSeriesContainsAMultiframeVolume() const
{
    return m_currentMultiframeVolumeNumber.contains(m_currentSeries);
}

void PatientFillerInput::increaseCurrentSingleFrameVolumeNumber()
{
    if (m_currentSeries)
    {
        m_currentSingleFrameVolumeNumber.insert(m_currentSeries, getCurrentSingleFrameVolumeNumber() + 1);
    }
}

int PatientFillerInput::getCurrentSingleFrameVolumeNumber()
{
    if (m_currentSeries)
    {
        // Insert default value if it doesn't exist
        if (!m_currentSingleFrameVolumeNumber.contains(m_currentSeries))
        {
            m_currentSingleFrameVolumeNumber[m_currentSeries] = 100;
        }

        return m_currentSingleFrameVolumeNumber[m_currentSeries];
    }
    else
    {
        return -1;
    }
}

void PatientFillerInput::setCurrentVolumeNumber(int volumeNumber)
{
    m_currentVolumeNumber = volumeNumber;
}

int PatientFillerInput::getCurrentVolumeNumber() const
{
    return m_currentVolumeNumber;
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
