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
#include "q2dviewerwidget.h"
#include "resetviewtoanatomicalplaneqviewercommand.h"
#include "volume.h"

namespace udg {

namespace {

const QRectF WholeGeometry(0.0, 0.0, 1.0, 1.0);
const QRectF LeftHalfGeometry(0.0, 0.0, 0.5, 1.0);
const QRectF RightHalfGeometry(0.5, 0.0, 0.5, 1.0);

void deleteHangingProtocolList(QList<HangingProtocol*> &list)
{
    foreach (HangingProtocol *hangingProtocol, list)
    {
        delete hangingProtocol;
    }
    list.clear();
}

}

LayoutManager::LayoutManager(Patient *patient, ViewersLayout *layout, QObject *parent)
 : QObject(parent)
{
    m_patient = patient;
    m_currentStudy = 0;
    m_priorStudy = 0;
    m_layout = layout;
    m_hangingProtocolManager = new HangingProtocolManager(this);
    m_currentHangingProtocolApplied = 0;
    m_priorHangingProtocolApplied = 0;
    m_combinedHangingProtocolApplied = 0;
}

LayoutManager::~LayoutManager()
{
}

void LayoutManager::initialize()
{
    if (m_patient)
    {
        QString currentStudyUID;
        QString priorStudyUID;

        QList<Study*> studies = m_patient->getStudies();

        if (studies.size() > 0)
        {
            currentStudyUID = studies.first()->getInstanceUID();

            if (studies.size() > 1)
            {
                priorStudyUID = studies[1]->getInstanceUID();
            }
        }
        setWorkingStudies(currentStudyUID, priorStudyUID);
    }
}

bool LayoutManager::hasStudyAnyModalityWithHangingProtocolPriority(Study *study)
{
    Settings settings;

    QStringList modalitiesWithHPPriority = settings.getValueAsQStringList(CoreSettings::ModalitiesToApplyHangingProtocolsAsFirstOption);

    QSet<QString> matchingModalities = modalitiesWithHPPriority.toSet().intersect(study->getModalities().toSet());

    return !matchingModalities.isEmpty();
}

void LayoutManager::setGrid(int rows, int columns)
{
    Study *study = m_currentStudy;

    if (m_priorStudy && RightHalfGeometry.contains(m_layout->getGeometryOfViewer(m_layout->getSelectedViewer())))
    {
        study = m_priorStudy;
    }

    QRectF geometry = prepareToChangeLayoutOfStudy(study);
    applyLayoutCandidates(getLayoutCandidates(study), study, geometry, rows, columns);
}

void LayoutManager::applyProperLayoutChoice(bool changeCurrentStudyLayout, bool changePriorStudyLayout, bool comparativeModeToggled)
{
    if (!m_currentStudy)
    {
        return;
    }

    // If a combined hanging protocol is available, the best one will be applied.
    // Otherwise, a HP or an autolayout will be applied for both studies separatelly.
    // If the prior study is not defined, the current study layout will be applied
    // in all available geometry. Otherwise it will be splitted in two spaces.
    if (m_combinedHangingProtocolCandidates.size() > 0)
    {
        //apply best hanging protocol
        setCombinedHangingProtocolApplied(m_hangingProtocolManager->setBestHangingProtocol(m_patient, m_combinedHangingProtocolCandidates, m_layout,
                                                                                           WholeGeometry));
        setCurrentHangingProtocolApplied(0);
        setPriorHangingProtocolApplied(0);
    }
    else
    {
        setCombinedHangingProtocolApplied(0);

        // We only change current study layout if there is no hanging protocol applied
        if (changeCurrentStudyLayout)
        {
            if (m_priorStudy)
            {
                setCurrentHangingProtocolApplied(applyProperLayoutChoice(m_currentStudy, m_currentStudyHangingProtocolCandidates, LeftHalfGeometry));
            }
            else
            {
                setCurrentHangingProtocolApplied(applyProperLayoutChoice(m_currentStudy, m_currentStudyHangingProtocolCandidates, WholeGeometry));
            }
        }
        else if (comparativeModeToggled)
        {
            if (m_priorStudy)
            {
                m_layout->mapViewersToNewGeometry(WholeGeometry, LeftHalfGeometry);
            }
            else
            {
                m_layout->cleanUp(RightHalfGeometry);
                m_layout->mapViewersToNewGeometry(LeftHalfGeometry, WholeGeometry);
            }
        }

        if (m_priorStudy && changePriorStudyLayout)
        {
            setPriorHangingProtocolApplied(applyProperLayoutChoice(m_priorStudy, m_priorStudyHangingProtocolCandidates, RightHalfGeometry));
        }
    }
}

HangingProtocol* LayoutManager::applyProperLayoutChoice(Study *study, const QList<HangingProtocol*> &hangingProtocols, const QRectF &studyLayoutGeometry)
{
    HangingProtocol *hangingProtocolApplied = 0;
    if (hasStudyAnyModalityWithHangingProtocolPriority(study))
    {
        hangingProtocolApplied = applyBestHangingProtocol(hangingProtocols, studyLayoutGeometry);
    }

    if (!hangingProtocolApplied)
    {
        applyLayoutCandidates(getLayoutCandidates(study), study, studyLayoutGeometry);
    }

    return hangingProtocolApplied;
}


HangingProtocol* LayoutManager::applyBestHangingProtocol(const QList<HangingProtocol*> hangingProtocols, const QRectF &geometry)
{
    if (m_currentStudyHangingProtocolCandidates.size() > 0)
    {
        return m_hangingProtocolManager->setBestHangingProtocol(m_patient, hangingProtocols, m_layout, geometry);
    }
    else
    {
        return NULL;
    }
}

void LayoutManager::applyNextHangingProtocol()
{
    HangingProtocol *hangingProtocolApplied;
    QList<HangingProtocol*> hangingProtocolCandidates;
    // This is a pointer to a method
    void (LayoutManager::*setHangingProtocol)(int);

    getHangingProtocolAppliedCandidatesAndSetterForSelectedViewer(hangingProtocolApplied, hangingProtocolCandidates, setHangingProtocol);

    if (hangingProtocolApplied)
    {
        int index = hangingProtocolCandidates.indexOf(hangingProtocolApplied);

        if (index + 1 < hangingProtocolCandidates.count())
        {
            // This is the way to use a pointer to a method to call the method
            (this->*setHangingProtocol)(hangingProtocolCandidates.at(index + 1)->getIdentifier());
        }
    }
}

void LayoutManager::applyPreviousHangingProtocol()
{
    HangingProtocol *hangingProtocolApplied;
    QList<HangingProtocol*> hangingProtocolCandidates;
    // This is a pointer to a method
    void (LayoutManager::*setHangingProtocol)(int);

    getHangingProtocolAppliedCandidatesAndSetterForSelectedViewer(hangingProtocolApplied, hangingProtocolCandidates, setHangingProtocol);

    if (hangingProtocolApplied)
    {
        int index = hangingProtocolCandidates.indexOf(hangingProtocolApplied);

        if (index > 0)
        {
            // This is the way to use a pointer to a method to call the method
            (this->*setHangingProtocol)(hangingProtocolCandidates.at(index - 1)->getIdentifier());
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

void LayoutManager::applyLayoutCandidates(const QList<StudyLayoutConfig> &candidates, Study *study, const QRectF &geometry)
{
    applyLayoutCandidates(candidates, study, geometry, -1, -1);
}

void LayoutManager::applyLayoutCandidates(const QList<StudyLayoutConfig> &candidates, Study *study, const QRectF &geometry, int rows, int columns)
{
    StudyLayoutConfig layoutToApply = getBestLayoutCandidate(candidates, study);

    StudyLayoutMapper mapper;
    mapper.applyConfig(layoutToApply, m_layout, study, geometry, rows, columns);
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

void LayoutManager::setWorkingStudies(const QString &currentStudyUID, const QString &priorStudyUID)
{
    Study *newCurrentStudy = m_patient->getStudy(currentStudyUID);
    if (!newCurrentStudy)
    {
        return;
    }
    Study *newPriorStudy = m_patient->getStudy(priorStudyUID);

    bool addCurrentStudyHangingProtocols = false;
    bool changeCurrentStudyLayout = false;
    bool changePriorStudyLayout = false;

    // Search hanging protocols for prior study
    QList<Study*> priorStudies;
    if (newPriorStudy)
    {
        priorStudies << newPriorStudy;

        if (m_priorStudy != newPriorStudy)
        {
            deleteHangingProtocolList(m_priorStudyHangingProtocolCandidates);
            m_priorStudyHangingProtocolCandidates = m_hangingProtocolManager->searchHangingProtocols(newPriorStudy);
        }
    }
    else
    {
        deleteHangingProtocolList(m_priorStudyHangingProtocolCandidates);
        setPriorHangingProtocolApplied(0);
    }

    // Search hanging protocols for current study and combined studies
    if (m_currentStudy != newCurrentStudy)
    {
        deleteHangingProtocolList(m_currentStudyHangingProtocolCandidates);
        addCurrentStudyHangingProtocols = true;
    }

    deleteHangingProtocolList(m_combinedHangingProtocolCandidates);
    foreach (HangingProtocol *hp, m_hangingProtocolManager->searchHangingProtocols(newCurrentStudy, priorStudies))
    {
        if (hp->getNumberOfPriors() == 0 && addCurrentStudyHangingProtocols)
        {
            m_currentStudyHangingProtocolCandidates << hp;
        }
        else if (hp->getNumberOfPriors() > 0)
        {
            m_combinedHangingProtocolCandidates << hp;
        }
    }

    // check if layouts need to be changed
    if (m_combinedHangingProtocolApplied)
    {
        changeCurrentStudyLayout = true;
        changePriorStudyLayout = true;
    }
    else
    {
        changeCurrentStudyLayout = m_currentStudy != newCurrentStudy;
        changePriorStudyLayout = m_priorStudy != newPriorStudy;
    }

    bool comparativeModeToggled = (newPriorStudy == NULL) ^ (m_priorStudy == NULL);

    m_currentStudy = newCurrentStudy;
    m_priorStudy = newPriorStudy;

    emit hangingProtocolCandidatesFound(m_combinedHangingProtocolCandidates, m_currentStudyHangingProtocolCandidates, m_priorStudyHangingProtocolCandidates);

    // Re-emit the signals of the currently applied hanging protocols to ensure that they are marked as active in the hanging protocols widget
    emit activeCombinedHangingProtocolChanged(m_combinedHangingProtocolApplied);
    emit activeCurrentHangingProtocolChanged(m_currentHangingProtocolApplied);
    emit activePriorHangingProtocolChanged(m_priorHangingProtocolApplied);

    applyProperLayoutChoice(changeCurrentStudyLayout, changePriorStudyLayout, comparativeModeToggled);
}


HangingProtocol* LayoutManager::setHangingProtocol(int hangingProtocolNumber, const QList<HangingProtocol*> &hangingProtocols, const QRectF &geometry)
{
    HangingProtocol *hangingProtocol = 0;
    bool found = false;
    QListIterator<HangingProtocol*> iterator(hangingProtocols);

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
        m_hangingProtocolManager->applyHangingProtocol(hangingProtocol, m_layout, m_patient, geometry);
    }

    return hangingProtocol;
}

void LayoutManager::setCombinedHangingProtocol(int hangingProtocolNumber)
{
    setCombinedHangingProtocolApplied(setHangingProtocol(hangingProtocolNumber, m_combinedHangingProtocolCandidates, WholeGeometry));
    setCurrentHangingProtocolApplied(0);
    setPriorHangingProtocolApplied(0);
}

void LayoutManager::setCurrentHangingProtocol(int hangingProtocolNumber)
{
    QRectF geometry = (!m_priorStudy)? WholeGeometry : LeftHalfGeometry;

    setCurrentHangingProtocolApplied(setHangingProtocol(hangingProtocolNumber, m_currentStudyHangingProtocolCandidates, geometry));

    if (m_combinedHangingProtocolApplied)
    {
        setCombinedHangingProtocolApplied(0);
        setPriorHangingProtocolApplied(applyProperLayoutChoice(m_priorStudy, m_priorStudyHangingProtocolCandidates, RightHalfGeometry));
    }
}

void LayoutManager::setPriorHangingProtocol(int hangingProtocolNumber)
{
    setPriorHangingProtocolApplied(setHangingProtocol(hangingProtocolNumber, m_priorStudyHangingProtocolCandidates, RightHalfGeometry));

    if (m_combinedHangingProtocolApplied)
    {
        setCombinedHangingProtocolApplied(0);
        setCurrentHangingProtocolApplied(applyProperLayoutChoice(m_currentStudy, m_currentStudyHangingProtocolCandidates, LeftHalfGeometry));
    }
}

void LayoutManager::setFusionLayout2x1First(const QList<Volume*> &volumes, const AnatomicalPlane &anatomicalPlane)
{
    if (volumes.size() != 2)
    {
        return;
    }

    QRectF geometry = prepareToChangeLayoutOfStudy(volumes[0]->getStudy());
    m_layout->cleanUp(geometry);
    m_layout->setGridInArea(1, 2, geometry);
    QList<Q2DViewerWidget*> viewers = m_layout->getViewersInsideGeometry(geometry);

    QList<Volume*> inputs[2];
    inputs[0] << volumes[0];
    inputs[1] << volumes;

    for (int i = 0; i < 2; i++)
    {
        viewers[i]->getViewer()->setInputAsynchronously(inputs[i],
            new ResetViewToAnatomicalPlaneQViewerCommand(viewers[i]->getViewer(), anatomicalPlane, viewers[i]));
    }
}

void LayoutManager::setFusionLayout2x1Second(const QList<Volume*> &volumes, const AnatomicalPlane &anatomicalPlane)
{
    if (volumes.size() != 2)
    {
        return;
    }

    QRectF geometry = prepareToChangeLayoutOfStudy(volumes[0]->getStudy());
    m_layout->cleanUp(geometry);
    m_layout->setGridInArea(1, 2, geometry);
    QList<Q2DViewerWidget*> viewers = m_layout->getViewersInsideGeometry(geometry);

    QList<Volume*> inputs[2];
    inputs[0] << volumes;
    inputs[1] << volumes[1];

    for (int i = 0; i < 2; i++)
    {
        viewers[i]->getViewer()->setInputAsynchronously(inputs[i],
            new ResetViewToAnatomicalPlaneQViewerCommand(viewers[i]->getViewer(), anatomicalPlane, viewers[i]));
    }
}

void LayoutManager::setFusionLayout3x1(const QList<Volume*> &volumes, const AnatomicalPlane &anatomicalPlane)
{
    if (volumes.size() != 2)
    {
        return;
    }

    QRectF geometry = prepareToChangeLayoutOfStudy(volumes[0]->getStudy());
    m_layout->cleanUp(geometry);
    m_layout->setGridInArea(1, 3, geometry);
    QList<Q2DViewerWidget*> viewers = m_layout->getViewersInsideGeometry(geometry);

    QList<Volume*> inputs[3];
    inputs[0] << volumes[0];
    inputs[1] << volumes;
    inputs[2] << volumes[1];

    for (int i = 0; i < 3; i++)
    {
        viewers[i]->getViewer()->setInputAsynchronously(inputs[i],
            new ResetViewToAnatomicalPlaneQViewerCommand(viewers[i]->getViewer(), anatomicalPlane, viewers[i]));
    }
}

void LayoutManager::setFusionLayout2x3First(const QList<Volume*> &volumes)
{
    if (volumes.size() != 2)
    {
        return;
    }

    QRectF geometry = prepareToChangeLayoutOfStudy(volumes[0]->getStudy());
    m_layout->cleanUp(geometry);
    m_layout->setGridInArea(3, 2, geometry);
    QList<Q2DViewerWidget*> viewers = m_layout->getViewersInsideGeometry(geometry);

    QList<Volume*> inputs[2];
    inputs[0] << volumes[0];
    inputs[1] << volumes;

    AnatomicalPlane views[3] = { AnatomicalPlane::Axial, AnatomicalPlane::Coronal, AnatomicalPlane::Sagittal };

    for (int i = 0; i < 6; i++)
    {
        viewers[i]->getViewer()->setInputAsynchronously(inputs[i % 2],
            new ResetViewToAnatomicalPlaneQViewerCommand(viewers[i]->getViewer(), views[i / 2], viewers[i]));
    }
}

void LayoutManager::setFusionLayout2x3Second(const QList<Volume*> &volumes)
{
    if (volumes.size() != 2)
    {
        return;
    }

    QRectF geometry = prepareToChangeLayoutOfStudy(volumes[0]->getStudy());
    m_layout->cleanUp(geometry);
    m_layout->setGridInArea(3, 2, geometry);
    QList<Q2DViewerWidget*> viewers = m_layout->getViewersInsideGeometry(geometry);

    QList<Volume*> inputs[2];
    inputs[0] << volumes;
    inputs[1] << volumes[1];

    AnatomicalPlane views[3] = { AnatomicalPlane::Axial, AnatomicalPlane::Coronal, AnatomicalPlane::Sagittal };

    for (int i = 0; i < 6; i++)
    {
        viewers[i]->getViewer()->setInputAsynchronously(inputs[i % 2],
            new ResetViewToAnatomicalPlaneQViewerCommand(viewers[i]->getViewer(), views[i / 2], viewers[i]));
    }
}

void LayoutManager::setFusionLayout3x3(const QList<Volume*> &volumes)
{
    if (volumes.size() != 2)
    {
        return;
    }

    QRectF geometry = prepareToChangeLayoutOfStudy(volumes[0]->getStudy());
    m_layout->cleanUp(geometry);
    m_layout->setGridInArea(3, 3, geometry);
    QList<Q2DViewerWidget*> viewers = m_layout->getViewersInsideGeometry(geometry);

    QList<Volume*> inputs[3];
    inputs[0] << volumes[0];
    inputs[1] << volumes;
    inputs[2] << volumes[1];

    AnatomicalPlane views[3] = { AnatomicalPlane::Axial, AnatomicalPlane::Coronal, AnatomicalPlane::Sagittal };

    for (int i = 0; i < 9; i++)
    {
        viewers[i]->getViewer()->setInputAsynchronously(inputs[i % 3],
            new ResetViewToAnatomicalPlaneQViewerCommand(viewers[i]->getViewer(), views[i / 3], viewers[i]));
    }
}

void LayoutManager::setFusionLayoutMprRight(const QList<Volume *> &volumes)
{
    if (volumes.size() != 2)
    {
        return;
    }

    QRectF geometry = prepareToChangeLayoutOfStudy(volumes[0]->getStudy());
    m_layout->cleanUp(geometry);

    Q2DViewerWidget *axial = m_layout->addViewer(m_layout->convertGeometry(QRectF(0.5, 0, 0.5, 1), geometry));
    axial->getViewer()->setInputAsynchronously(volumes, new ResetViewToAnatomicalPlaneQViewerCommand(axial->getViewer(), AnatomicalPlane::Axial, axial));

    Q2DViewerWidget *sagittal = m_layout->addViewer(m_layout->convertGeometry(QRectF(0, 0.5, 0.5, 0.5), geometry));
    sagittal->getViewer()->setInputAsynchronously(volumes,
                                                  new ResetViewToAnatomicalPlaneQViewerCommand(sagittal->getViewer(), AnatomicalPlane::Sagittal, sagittal));

    Q2DViewerWidget *coronal = m_layout->addViewer(m_layout->convertGeometry(QRectF(0, 0, 0.5, 0.5), geometry));
    coronal->getViewer()->setInputAsynchronously(volumes,
                                                 new ResetViewToAnatomicalPlaneQViewerCommand(coronal->getViewer(), AnatomicalPlane::Coronal, coronal));
}

void LayoutManager::getHangingProtocolAppliedCandidatesAndSetterForSelectedViewer(HangingProtocol* &hangingProtocolApplied,
                                                                                  QList<HangingProtocol*> &hangingProtocolCandidates,
                                                                                  void (LayoutManager::* &setHangingProtocol)(int)) const
{
    if (m_combinedHangingProtocolApplied)
    {
        hangingProtocolApplied = m_combinedHangingProtocolApplied;
        hangingProtocolCandidates = m_combinedHangingProtocolCandidates;
        setHangingProtocol = &LayoutManager::setCombinedHangingProtocol;
    }

    if (m_currentHangingProtocolApplied)
    {
        QRectF selectedViewerGeometry = m_layout->getGeometryOfViewer(m_layout->getSelectedViewer());

        if (!m_priorStudy || LeftHalfGeometry.contains(selectedViewerGeometry))
        {
            hangingProtocolApplied = m_currentHangingProtocolApplied;
            hangingProtocolCandidates = m_currentStudyHangingProtocolCandidates;
            setHangingProtocol = &LayoutManager::setCurrentHangingProtocol;
        }
    }

    if (m_priorHangingProtocolApplied)
    {
        QRectF selectedViewerGeometry = m_layout->getGeometryOfViewer(m_layout->getSelectedViewer());

        if (RightHalfGeometry.contains(selectedViewerGeometry))
        {
            hangingProtocolApplied = m_priorHangingProtocolApplied;
            hangingProtocolCandidates = m_priorStudyHangingProtocolCandidates;
            setHangingProtocol = &LayoutManager::setPriorHangingProtocol;
        }
    }
}

void LayoutManager::setCombinedHangingProtocolApplied(HangingProtocol *combinedHangingProtocolApplied)
{
    if (m_combinedHangingProtocolApplied != combinedHangingProtocolApplied)
    {
        m_combinedHangingProtocolApplied = combinedHangingProtocolApplied;
        emit activeCombinedHangingProtocolChanged(combinedHangingProtocolApplied);
    }
}

void LayoutManager::setCurrentHangingProtocolApplied(HangingProtocol *currentHangingProtocolApplied)
{
    if (m_currentHangingProtocolApplied != currentHangingProtocolApplied)
    {
        m_currentHangingProtocolApplied = currentHangingProtocolApplied;
        emit activeCurrentHangingProtocolChanged(currentHangingProtocolApplied);
    }
}

void LayoutManager::setPriorHangingProtocolApplied(HangingProtocol *priorHangingProtocolApplied)
{
    if (m_priorHangingProtocolApplied != priorHangingProtocolApplied)
    {
        m_priorHangingProtocolApplied = priorHangingProtocolApplied;
        emit activePriorHangingProtocolChanged(priorHangingProtocolApplied);
    }
}

QRectF LayoutManager::prepareToChangeLayoutOfStudy(Study *study)
{
    if (m_priorStudy)
    {
        if (study == m_currentStudy)
        {
            setCurrentHangingProtocolApplied(0);

            if (m_combinedHangingProtocolApplied)
            {
                setCombinedHangingProtocolApplied(0);
                setPriorHangingProtocolApplied(applyProperLayoutChoice(m_priorStudy, m_priorStudyHangingProtocolCandidates, RightHalfGeometry));
            }

            return LeftHalfGeometry;
        }
        else // study == m_priorStudy
        {
            setPriorHangingProtocolApplied(0);

            if (m_combinedHangingProtocolApplied)
            {
                setCombinedHangingProtocolApplied(0);
                setCurrentHangingProtocolApplied(applyProperLayoutChoice(m_currentStudy, m_currentStudyHangingProtocolCandidates, LeftHalfGeometry));
            }

            return RightHalfGeometry;
        }
    }
    else
    {
        setCurrentHangingProtocolApplied(0);
        return WholeGeometry;
    }
}

} // end namespace udg
