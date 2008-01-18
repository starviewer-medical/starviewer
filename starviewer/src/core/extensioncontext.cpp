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

Volume *ExtensionContext::getDefaultVolume() const
{
    Volume *defaultVolume = NULL;

    QList<Series *> selectedSeries = m_patient->getSelectedSeries();
    // TODO de moment només agafem la primera
    if( !selectedSeries.isEmpty() )
    {
        Series *defaultSeries = selectedSeries.at(0);
        QString modality = defaultSeries->getModality();
        // si la modalitat no és una de les suportades, busquem un altre volum per defecte
        if( modality == "PR" || modality == "KO" || modality == "SR" )
        {
            bool ok = false;
            foreach( Study *study, m_patient->getStudies() )
            {
                foreach( Series *series, study->getSeries() )
                {
                    modality = series->getModality();
                    if( modality != "PR" && modality != "KO" && modality != "SR" )
                    {
                        ok = true;
                        defaultSeries = series;
                        break;
                    }
                }
            }
            if( !ok )
                DEBUG_LOG("No hi ha cap serie de l'actual pacient amb modalitat suportada");
        }
        defaultVolume = defaultSeries->getFirstVolume();
    }
    else
        DEBUG_LOG("EI! No tenim cap sèrie seleccionada!");

    return defaultVolume;
}

}

