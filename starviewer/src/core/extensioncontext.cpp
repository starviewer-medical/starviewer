/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "extensioncontext.h"

#include "patient.h"
#include "volume.h"

namespace udg {

ExtensionContext::ExtensionContext()
{
}

ExtensionContext::~ExtensionContext()
{
}

Patient* ExtensionContext::getPatient() const
{
    return m_patient;
}

void ExtensionContext::setPatient(Patient *patient)
{
    m_patient = patient;
}

void ExtensionContext::addDefaultSelectedStudy(QString study)
{
    m_defaultSelectedStudies << study;
}

void ExtensionContext::addDefaultSelectedStudies( QStringList studies )
{
    m_defaultSelectedStudies += studies;
}

void ExtensionContext::setDefaultSelectedStudies( QStringList selectedStudies )
{
    m_defaultSelectedStudies = selectedStudies;
}

QStringList ExtensionContext::getDefaultSelectedStudies() const
{
    return m_defaultSelectedStudies;
}

void ExtensionContext::addDefaultSelectedSeries(QString series)
{
    m_defaultSelectedSeries << series;
}

void ExtensionContext::setDefaultSelectedSeries( QStringList selectedSeries )
{
    m_defaultSelectedSeries = selectedSeries;
}

void ExtensionContext::addDefaultSelectedSeries( QStringList series )
{
    m_defaultSelectedSeries += series;
}

QStringList ExtensionContext::getDefaultSelectedSeries() const
{
    return m_defaultSelectedSeries;
}

Volume *ExtensionContext::getDefaultVolume() const
{
    Volume *defaultVolume = NULL;

    QString studyUID = m_defaultSelectedStudies.at(0);
    QString seriesUID = m_defaultSelectedSeries.at(0);

    Study *study = m_patient->getStudy( studyUID );
    if( !study )// si no trobem l'uid demanat, agafem el primer
        study = m_patient->getStudies().at(0);

    Series *series = study->getSeries( seriesUID );
    if( !series ) // si no trobem l'uid demanat, agafem el primer
        series = study->getSeries().at(0);

    defaultVolume = series->getFirstVolume();

    return defaultVolume;
}

}

