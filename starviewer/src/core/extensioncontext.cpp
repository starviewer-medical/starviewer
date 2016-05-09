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

#include "extensioncontext.h"

#include "patient.h"
#include "volume.h"
#include "logging.h"

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
    Series *defaultSeries = NULL;
    bool searchForDefaultSeries = false;
    QList<Series*> selectedSeries = m_patient->getSelectedSeries();

    if (selectedSeries.isEmpty())
    {
        searchForDefaultSeries = true;
    }
    else
    {
        // TODO de moment només agafem la primera de les possibles seleccionades
        defaultSeries = selectedSeries.at(0);
        // Necessitem que les sèries siguin visualitzables
        if (!defaultSeries->isViewable())
        {
            searchForDefaultSeries = true;
        }
        else
        {
            defaultVolume = defaultSeries->getFirstVolume();
        }
    }
    // En comptes de searchForDefaultSeries podríem fer servir
    // defaultVolume, però amb la var. booleana el codi és més llegible
    if (searchForDefaultSeries)
    {
        bool ok = false;
        foreach (Study *study, m_patient->getStudies())
        {
            QList<Series*> viewableSeries = study->getViewableSeries();
            if (!viewableSeries.isEmpty())
            {
                ok = true;
                defaultVolume = viewableSeries.at(0)->getFirstVolume();
                break;
            }
        }
        if (!ok)
        {
            DEBUG_LOG("No hi ha cap serie de l'actual pacient que sigui visualitzable. Retornem volum NUL.");
            ERROR_LOG("No hi ha cap serie de l'actual pacient que sigui visualitzable. Retornem volum NUL.");
        }
    }

    return defaultVolume;
}

bool ExtensionContext::hasImages() const
{
    if (!m_patient)
    {
        return false;
    }

    foreach (Study *study, m_patient->getStudies())
    {
        foreach (Series *series, study->getSeries())
        {
            if (series->hasImages())
            {
                return true;
            }
        }
    }

    return false;
}

bool ExtensionContext::hasEncapsulatedDocuments() const
{
    if (!m_patient)
    {
        return false;
    }

    foreach (Study *study, m_patient->getStudies())
    {
        foreach (Series *series, study->getSeries())
        {
            if (series->hasEncapsulatedDocuments())
            {
                return true;
            }
        }
    }

    return false;
}

}
