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

#include "layoutmanager.h"

#include "patient.h"
#include "study.h"
#include "hangingprotocolmanager.h"
#include "viewerslayout.h"
#include "coresettings.h"
#include "studylayoutconfigsloader.h"
#include "studylayoutmapper.h"
#include "studylayoutconfig.h"
#include "studylayoutconfigsettingsmanager.h"
#include "hangingprotocol.h"

namespace udg {

LayoutManager::LayoutManager(Patient *patient, ViewersLayout *layout, QObject *parent)
 : QObject(parent)
{
    m_patient = patient;
    m_layout = layout;
    m_hangingProtocolManager = new HangingProtocolManager(this);
    m_currentHangingProtocolApplied = 0;
    
    connect(m_patient, SIGNAL(studyAdded(Study*)), SLOT(onStudyAdded(Study*)));
}

LayoutManager::~LayoutManager()
{
}

void LayoutManager::initialize()
{
    searchHangingProtocols();
    applyProperLayoutChoice();
}

bool LayoutManager::hasCurrentPatientAnyModalityWithHangingProtocolPriority()
{
    Settings settings;

    QStringList modalitiesWithHPPriority = settings.getValueAsQStringList(CoreSettings::ModalitiesToApplyHangingProtocolsAsFirstOption);

    QSet<QString> matchingModalities = modalitiesWithHPPriority.toSet().intersect(m_patient->getModalities().toSet());

    return !matchingModalities.isEmpty();
}

void LayoutManager::applyProperLayoutChoice()
{
    bool layoutApplied = false;
    if (hasCurrentPatientAnyModalityWithHangingProtocolPriority())
    {
        layoutApplied = applyBestHangingProtocol();
    }

    if (!layoutApplied)
    {
        Study *mostRecentStudy = m_patient->getStudies().first();
        applyLayoutCandidates(getLayoutCandidates(mostRecentStudy), mostRecentStudy);
    }
}

void LayoutManager::searchHangingProtocols()
{
    m_hangingProtocolCandidates = m_hangingProtocolManager->searchHangingProtocols(m_patient);
    emit hangingProtocolCandidatesFound(m_hangingProtocolCandidates);
}

bool LayoutManager::applyBestHangingProtocol()
{
    if (m_hangingProtocolCandidates.size() > 0)
    {
        m_currentHangingProtocolApplied = m_hangingProtocolManager->setBestHangingProtocol(m_patient, m_hangingProtocolCandidates, m_layout);
        return true;
    }
    else
    {
        m_currentHangingProtocolApplied = 0;
        return false;
    }
}

void LayoutManager::applyNextHangingProtocol()
{
    if (m_currentHangingProtocolApplied)
    {
        int index = m_hangingProtocolCandidates.indexOf(m_currentHangingProtocolApplied);

        if (index + 1 < m_hangingProtocolCandidates.count())
        {
            this->setHangingProtocol(m_hangingProtocolCandidates.at(index + 1));
        }
    }
}

void LayoutManager::applyPreviousHangingProtocol()
{
    if (m_currentHangingProtocolApplied)
    {
        int index = m_hangingProtocolCandidates.indexOf(m_currentHangingProtocolApplied);

        if (index > 0)
        {
            this->setHangingProtocol(m_hangingProtocolCandidates.at(index - 1));
        }
    }
}

void LayoutManager::cancelOngoingOperations()
{
    m_hangingProtocolManager->cancelAllHangingProtocolsDownloading();
}

QList<StudyLayoutConfig> LayoutManager::getLayoutCandidates(Study *study)
{
    QList<StudyLayoutConfig> configurationCandidates;
    
    if (!study)
    {
        return configurationCandidates;
    }
    
    QStringList studyModalities = study->getModalities();
    StudyLayoutConfigSettingsManager settingsManager;
    
    foreach (const StudyLayoutConfig &currentConfig, settingsManager.getConfigList())
    {
        if (studyModalities.contains(currentConfig.getModality(), Qt::CaseInsensitive))
        {
            configurationCandidates << currentConfig;
        }
    }

    return configurationCandidates;
}

void LayoutManager::applyLayoutCandidates(const QList<StudyLayoutConfig> &candidates, Study *study)
{
    StudyLayoutConfig layoutToApply = getBestLayoutCandidate(candidates, study);

    StudyLayoutMapper mapper;
    mapper.applyConfig(layoutToApply, m_layout, study);
}

StudyLayoutConfig LayoutManager::getBestLayoutCandidate(const QList<StudyLayoutConfig> &candidates, Study *study)
{
    StudyLayoutConfig bestLayout;

    if (!candidates.isEmpty())
    {
        if (candidates.count() > 1)
        {
            // If we different modalities/candidates we take the simple choice of apply a default generic layout
            // TODO This could be enhanced making smarter decisions, which can include decisions based on user settings i.e.
            bestLayout = getMergedStudyLayoutConfig(candidates);
        }
        else
        {
            bestLayout = candidates.first();
        }
    }
    else
    {
        // If no candidate found, we choose a default configuration.
        // This default configuration is not yet configurable through settings, could be done in a future enhancement.
        bestLayout = StudyLayoutConfigsLoader::getDefaultConfigForModality(study->getModalities().first());
    }

    return bestLayout;
}

StudyLayoutConfig LayoutManager::getMergedStudyLayoutConfig(const QList<StudyLayoutConfig> &configurations)
{
    if (configurations.isEmpty())
    {
        return StudyLayoutConfig();
    }

    StudyLayoutConfig mergedLayout = configurations.first();
    QStringList modalities;
    modalities << mergedLayout.getModality();
    
    for (int i = 1; i < configurations.count(); ++i)
    {
        foreach (const StudyLayoutConfig::ExclusionCriteriaType &criteria, configurations.at(i).getExclusionCriteria())
        {
            mergedLayout.addExclusionCriteria(criteria);
        }
        
        if (configurations.at(i).getMaximumNumberOfViewers() < mergedLayout.getMaximumNumberOfViewers() && configurations.at(i).getMaximumNumberOfViewers() > 0)
        {
            mergedLayout.setMaximumNumberOfViewers(configurations.at(i).getMaximumNumberOfViewers());
        }

        if (configurations.at(i).getUnfoldDirection() != mergedLayout.getUnfoldDirection())
        {
            mergedLayout.setUnfoldDirection(StudyLayoutConfig::DefaultUnfoldDirection);
        }
        
        if (configurations.at(i).getUnfoldType() != mergedLayout.getUnfoldType())
        {
            mergedLayout.setUnfoldType(StudyLayoutConfig::DefaultUnfoldType);
        }
        
        modalities << configurations.at(i).getModality();
    }

    // This step is needed to apply the configuration whatever the modality of the study
    mergedLayout.setModality("");
    return mergedLayout;
}

void LayoutManager::setHangingProtocol(int hangingProtocolNumber)
{
    HangingProtocol *hangingProtocol = 0;
    bool found = false;
    QListIterator<HangingProtocol*> iterator(m_hangingProtocolCandidates);

    while (!found && iterator.hasNext())
    {
        HangingProtocol *candidate = iterator.next();
        if (candidate->getIdentifier() == hangingProtocolNumber)
        {
            found = true;
            hangingProtocol = candidate;
        }
    }

    if (found)
    {
        this->setHangingProtocol(hangingProtocol);
    }
}

void LayoutManager::setHangingProtocol(HangingProtocol *hangingProtocol)
{
    if (hangingProtocol)
    {
        m_hangingProtocolManager->applyHangingProtocol(hangingProtocol, m_layout, m_patient);
        m_currentHangingProtocolApplied = hangingProtocol;
    }
}

void LayoutManager::addHangingProtocolsWithPrevious(QList<Study*> studies)
{
    m_hangingProtocolCandidates = m_hangingProtocolManager->searchHangingProtocols(m_patient, studies);
    emit hangingProtocolCandidatesFound(m_hangingProtocolCandidates);
    // HACK To notify we ended searching related studies and thus we have all the hanging protocols available
    emit previousStudiesSearchEnded();
}

void LayoutManager::onStudyAdded(Study *study)
{
    addHangingProtocolsWithPrevious(QList<Study*>() << study);
}

} // end namespace udg
