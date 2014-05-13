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

#include "phasesyncaction.h"

#include "inputsynccriterion.h"
#include "q2dviewer.h"

namespace udg {

PhaseSyncAction::PhaseSyncAction()
 : SyncAction(), m_phase(0), m_volume(0)
{
}

PhaseSyncAction::~PhaseSyncAction()
{
}

void PhaseSyncAction::setPhase(int phase)
{
    m_phase = phase;
}

void PhaseSyncAction::setVolume(Volume *volume)
{
    m_volume = volume;
}

void PhaseSyncAction::run(QViewer *viewer)
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(viewer);

    // Limit action to 2D viewers and to secondary inputs
    if (viewer2D && viewer2D->getMainInput() != m_volume)
    {
        viewer2D->setPhaseInVolume(viewer2D->indexOfVolume(m_volume), m_phase);
    }
}

void PhaseSyncAction::setupMetaData()
{
    m_metaData = SyncActionMetaData("PhaseSyncAction", QObject::tr("Phase"), "phase");
}

void PhaseSyncAction::setupDefaultSyncCriteria()
{
    m_defaultSyncCriteria << new InputSyncCriterion();
}

}
