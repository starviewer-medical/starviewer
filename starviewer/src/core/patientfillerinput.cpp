/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientfillerinput.h"
#include "logging.h"
#include "patient.h"

namespace udg {

PatientFillerInput::PatientFillerInput()
{
}

PatientFillerInput::~PatientFillerInput()
{
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
            patient = m_patientList.at(i);
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
            patient = m_patientList.at(i);
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

QStringList PatientFillerInput::getFilesList() const
{
    return m_fileList;
}

void PatientFillerInput::addFile( QString filename )
{
    if( !m_fileList.contains(filename) )
        m_fileList << filename;
}

void PatientFillerInput::removeFile( QString filename )
{
    bool found = false;
    QMutableStringListIterator iterator( m_fileList );
    while( iterator.hasNext() && !found )
    {
        if( iterator.next() == filename )
        {
            found = true;
            iterator.remove();
        }
    }
}

void PatientFillerInput::addLabel( QString label )
{
    if( !m_globalLabels.contains(label) )
        m_globalLabels << label;
}

void PatientFillerInput::addLabelToSeries( QString label, Series *series )
{
    if( !m_seriesLabels.values( series ).contains( label ) )
        m_seriesLabels.insert( series, label );
}

QStringList PatientFillerInput::getLabels() const
{
    return m_globalLabels;
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

QList<Series *> PatientFillerInput::getSeriesWithLabels( QStringList labels )
{
    QList<Series *> resultSeries;
    bool ok;
    foreach( Series *series, m_seriesLabels.uniqueKeys() )
    {
        QStringList currentSeriesLabelList = m_seriesLabels.values( series );
        ok = true;
        foreach( QString value, labels ) // en comptes de fer un foreach seria millor fer un while i quan la condició no es dóna aturar
        {
            if( !currentSeriesLabelList.contains(value) )
            {
                ok = false;
                break;
            }
        }
        if( ok )
        {
            // afegir la sèrie a la llista
            resultSeries << series;
        }
    }
    return resultSeries;
}

}
