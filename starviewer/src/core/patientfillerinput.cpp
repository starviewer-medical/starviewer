/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientfillerinput.h"
#include "logging.h"
#include "patient.h"
#include "dicomtagreader.h"

namespace udg {

PatientFillerInput::PatientFillerInput(): m_dicomFile(0), m_currentSeries(0), m_currentVolumeNumber(0), m_currentMultiframeVolumeNumber(1)
{
}

PatientFillerInput::~PatientFillerInput()
{
    delete m_dicomFile;
}

void PatientFillerInput::addPatient( Patient *patient )
{
    if( patient )
        m_patientList << patient;
    else
        DEBUG_LOG("S'ha passat un pacient NUL, per tant no s'ha afegit res a la llista");
}

Patient *PatientFillerInput::getPatient( int index )
{
    Patient *patient = 0;
    if( index < m_patientList.size() )
    {
        patient = m_patientList.at( index );
    }
    else
        DEBUG_LOG("Índex fora de rang");

    return patient;
}

Patient *PatientFillerInput::getPatientByName( QString name )
{
    bool found = false;
    int i = 0;
    Patient *patient = 0;

    while( i < m_patientList.size() && !found )
    {
        if( m_patientList.at(i)->getFullName() == name )
        {
            patient = m_patientList[i];
            found = true;
        }
        i++;
    }

    return patient;
}

Patient *PatientFillerInput::getPatientByID( QString id )
{
    bool found = false;
    int i = 0;
    Patient *patient = 0;

    while( i < m_patientList.size() && !found )
    {
        if( m_patientList.at(i)->getID() == id )
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

void PatientFillerInput::setFilesList( QStringList files )
{
    if( !files.isEmpty() )
    {
        // si hi ha dades de pacients buidem la llista
        if( !m_patientList.isEmpty() )
            m_patientList.clear();

        m_fileList = files;
    }
}

QList<Patient*> PatientFillerInput::getPatientsList()
{
    return m_patientList;
}

QStringList PatientFillerInput::getFilesList() const
{
    return m_fileList;
}

void PatientFillerInput::addLabel( QString label )
{
    if( !m_globalLabels.contains(label) )
    {
        m_globalLabels << label;
        m_allLabels << label; // afegim a la llista de tots també
    }
}

void PatientFillerInput::addLabelToSeries( QString label, Series *series )
{
    if( !m_seriesLabels.values( series ).contains( label ) )
        m_seriesLabels.insert( series, label );

    // aquí ho separem perquè podria ser que la serie que especifiquem no tingui aquella label i una altre sí i s'hagi afegit ja abans
    if( !m_allLabels.contains(label) )
        m_allLabels << label;
}

QStringList PatientFillerInput::getLabels() const
{
    return m_allLabels;
}

bool PatientFillerInput::hasAllLabels(QStringList requiredLabelsList) const
{
    foreach (QString requiredLabel, requiredLabelsList)
    {
        if (!getLabels().contains(requiredLabel))
            return false;
    }
    return true;
}

void PatientFillerInput::initializeAllLabels()
{
    while(!m_allLabels.isEmpty())
    {
        m_allLabels.removeFirst();
    }
    while(!m_globalLabels.isEmpty())
    {
        m_globalLabels.removeFirst();
    }
    foreach ( Series *key, m_seriesLabels.keys() )
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

DICOMTagReader * PatientFillerInput::getDICOMFile()
{
    return m_dicomFile;
}

void PatientFillerInput::setCurrentImages(const QList<Image *> &images)
{
    m_currentImages = images;
}

QList<Image *> PatientFillerInput::getCurrentImages()
{
    return m_currentImages;
}

void PatientFillerInput::setCurrentSeries(Series *series)
{
    m_currentSeries = series;
}

Series * PatientFillerInput::getCurrentSeries()
{
    return m_currentSeries;
}

void PatientFillerInput::resetCurrentMultiframeVolumeNumber()
{
    m_currentMultiframeVolumeNumber = 1;
}
    
void PatientFillerInput::increaseCurrentMultiframeVolumeNumber()
{
    m_currentMultiframeVolumeNumber++;
}

int PatientFillerInput::getCurrentMultiframeVolumeNumber() const
{
    return m_currentMultiframeVolumeNumber;
}

int PatientFillerInput::getSingleFrameVolumeNumber() const
{
    // De moment sempre assignarem el mateix número de volum per conjunts single frame
    return 0;
}

void PatientFillerInput::setCurrentVolumeNumber(int volumeNumber)
{
    m_currentVolumeNumber = volumeNumber;
}

int PatientFillerInput::getCurrentVolumeNumber() const
{
    return m_currentVolumeNumber;
}

}
