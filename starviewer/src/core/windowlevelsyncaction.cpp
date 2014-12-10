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

#include "windowlevelsyncaction.h"

#include "qviewer.h"
#include "q2dviewer.h"
#include "voilutpresetstooldata.h"
#include "inputsynccriterion.h"

namespace udg {

WindowLevelSyncAction::WindowLevelSyncAction()
 : SyncAction()
{
}

WindowLevelSyncAction::~WindowLevelSyncAction()
{
}

void WindowLevelSyncAction::setWindowLevel(const WindowLevel &windowLevel)
{
    m_windowLevel = windowLevel;
}

void WindowLevelSyncAction::setVolume(Volume *volume)
{
    m_volume = volume;
}

void WindowLevelSyncAction::run(QViewer *viewer)
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(viewer);

    if (viewer2D)
    {
        int index = viewer2D->indexOfVolume(m_volume);
        int group;
        VoiLutPresetsToolData *currentWindowLevelPresetsToolData = viewer2D->getWindowLevelDataForVolume(index);

        // If the stored window level is in a group and isn't custom, then select it; otherwise, just set it
        if (currentWindowLevelPresetsToolData->getGroup(m_windowLevel, group) && group != VoiLutPresetsToolData::CustomPreset)
        {
            WindowLevel wl;
            currentWindowLevelPresetsToolData->getFromDescription(m_windowLevel.getName(), wl);
            viewer2D->setWindowLevelInVolume(index, wl);
        }
        else
        {
            viewer2D->setWindowLevelInVolume(index, m_windowLevel);
        }
    }
    else
    {
        viewer->getWindowLevelData()->setCurrentPreset(m_windowLevel);
    }
}

void WindowLevelSyncAction::setupMetaData()
{
    m_metaData = SyncActionMetaData("WindowLevelSyncAction", QObject::tr("Window level"), "windowLevel");
}

void WindowLevelSyncAction::setupDefaultSyncCriteria()
{
    m_defaultSyncCriteria << new InputSyncCriterion();
}

} // End namespace udg
