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

#include "pansignaltosyncactionmapper.h"

#include "q2dviewer.h"
#include "pansyncaction.h"

namespace udg {

PanSignalToSyncActionMapper::PanSignalToSyncActionMapper(QObject *parent)
 : SignalToSyncActionMapper(parent)
{
}

PanSignalToSyncActionMapper::~PanSignalToSyncActionMapper()
{
}

void PanSignalToSyncActionMapper::mapProperty()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);
    if (viewer2D)
    {
        mapToSyncAction(viewer2D->getCurrentFocalPoint());
    }
}


void PanSignalToSyncActionMapper::mapSignal()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);
    if (viewer2D)
    {
        connect(viewer2D, &QViewer::panChanged, this, &PanSignalToSyncActionMapper::mapToSyncAction);
    }
}

void PanSignalToSyncActionMapper::unmapSignal()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);
    if (viewer2D)
    {
        disconnect(viewer2D, &QViewer::panChanged, this, &PanSignalToSyncActionMapper::mapToSyncAction);
    }
}

void PanSignalToSyncActionMapper::mapToSyncAction(const Vector3 &sourceFocalPoint)
{
    if (!m_mappedSyncAction)
    {
        m_mappedSyncAction = new PanSyncAction();
    }

    static_cast<PanSyncAction*>(m_mappedSyncAction)->setSourceFocalPoint(sourceFocalPoint);
    
    emit actionMapped(m_mappedSyncAction);
}

} // End namespace udg
