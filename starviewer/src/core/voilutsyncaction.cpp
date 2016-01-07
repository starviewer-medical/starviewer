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

#include "voilutsyncaction.h"

#include "qviewer.h"
#include "q2dviewer.h"
#include "voilutpresetstooldata.h"
#include "inputsynccriterion.h"

namespace udg {

VoiLutSyncAction::VoiLutSyncAction()
 : SyncAction()
{
}

VoiLutSyncAction::~VoiLutSyncAction()
{
}

void VoiLutSyncAction::setVoiLut(const VoiLut &voiLut)
{
    m_voiLut = voiLut;
}

void VoiLutSyncAction::setVolume(Volume *volume)
{
    m_volume = volume;
}

void VoiLutSyncAction::run(QViewer *viewer)
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(viewer);

    if (viewer2D)
    {
        int index = viewer2D->indexOfVolume(m_volume);
        VoiLutPresetsToolData *currentVoiLutPresetsToolData = viewer2D->getVoiLutDataForVolume(index);

        // If the stored VOI LUT is in a group and isn't custom, then select it; otherwise, just set it
        if (currentVoiLutPresetsToolData->containsPreset(m_voiLut.getExplanation()) &&
                currentVoiLutPresetsToolData->getGroup(m_voiLut.getExplanation()) != VoiLutPresetsToolData::CustomPreset)
        {
            VoiLut voiLut = currentVoiLutPresetsToolData->getFromDescription(m_voiLut.getExplanation());
            viewer2D->setVoiLutInVolume(index, voiLut);
        }
        else
        {
            viewer2D->setVoiLutInVolume(index, m_voiLut);
        }
    }
    else
    {
        viewer->getVoiLutData()->setCurrentPreset(m_voiLut);
    }
}

void VoiLutSyncAction::setupMetaData()
{
    // Keeping "windowLevel" settings name for backward compatibility.
    m_metaData = SyncActionMetaData("VoiLutSyncAction", QObject::tr("VOI LUT"), "windowLevel");
}

void VoiLutSyncAction::setupDefaultSyncCriteria()
{
    m_defaultSyncCriteria << new InputSyncCriterion();
}

} // End namespace udg
