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
        defaultVolume = selectedSeries.at(0)->getFirstVolume();
    }
    else
        DEBUG_LOG("EI! No tenim cap sèrie seleccionada!");

    return defaultVolume;
}

}

