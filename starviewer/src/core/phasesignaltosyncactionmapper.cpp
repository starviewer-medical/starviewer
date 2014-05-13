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

#include "phasesignaltosyncactionmapper.h"

#include "q2dviewer.h"
#include "phasesyncaction.h"

namespace udg {

PhaseSignalToSyncActionMapper::PhaseSignalToSyncActionMapper(QObject *parent)
 : SignalToSyncActionMapper(parent)
{
    m_mappedSyncAction = new PhaseSyncAction();
}

PhaseSignalToSyncActionMapper::~PhaseSignalToSyncActionMapper()
{
}

void PhaseSignalToSyncActionMapper::mapProperty()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

    if (viewer2D)
    {
        mapToSyncAction(viewer2D->getCurrentPhase());
    }
}

void PhaseSignalToSyncActionMapper::mapSignal()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

    if (viewer2D)
    {
        connect(viewer2D, SIGNAL(phaseChanged(int)), SLOT(mapToSyncAction(int)));
    }
}

void PhaseSignalToSyncActionMapper::unmapSignal()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

    if (viewer2D)
    {
        disconnect(viewer2D, SIGNAL(phaseChanged(int)), this, SLOT(mapToSyncAction(int)));
    }
}

void PhaseSignalToSyncActionMapper::mapToSyncAction(int phase)
{
    PhaseSyncAction *action = static_cast<PhaseSyncAction*>(m_mappedSyncAction);
    action->setVolume(m_viewer->getMainInput());
    action->setPhase(phase);

    emit actionMapped(m_mappedSyncAction);
}

}
