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

#include "thickslabsignaltosyncactionmapper.h"

#include "q2dviewer.h"
#include "thickslabsyncaction.h"

namespace udg {

ThickSlabSignalToSyncActionMapper::ThickSlabSignalToSyncActionMapper(QObject *parent)
 : SignalToSyncActionMapper(parent)
{
    m_mappedSyncAction = new ThickSlabSyncAction();
}

ThickSlabSignalToSyncActionMapper::~ThickSlabSignalToSyncActionMapper()
{
}

void ThickSlabSignalToSyncActionMapper::mapProperty()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

    if (viewer2D)
    {
        mapThicknessToSyncAction(viewer2D->getSlabThickness());
        mapProjectionModeToSyncAction(viewer2D->getSlabProjectionMode());
    }
}

void ThickSlabSignalToSyncActionMapper::mapSignal()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

    if (viewer2D)
    {
        connect(viewer2D, &Q2DViewer::slabProjectionModeChanged, this, &ThickSlabSignalToSyncActionMapper::mapProjectionModeToSyncAction);
        connect(viewer2D, &Q2DViewer::slabThicknessChanged, this, &ThickSlabSignalToSyncActionMapper::mapThicknessToSyncAction);
    }
}

void ThickSlabSignalToSyncActionMapper::unmapSignal()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

    if (viewer2D)
    {
        disconnect(viewer2D, &Q2DViewer::slabProjectionModeChanged, this, &ThickSlabSignalToSyncActionMapper::mapProjectionModeToSyncAction);
        disconnect(viewer2D, &Q2DViewer::slabThicknessChanged, this, &ThickSlabSignalToSyncActionMapper::mapThicknessToSyncAction);
    }
}

void ThickSlabSignalToSyncActionMapper::mapProjectionModeToSyncAction(VolumeDisplayUnit::SlabProjectionMode slabProjectionMode)
{
    ThickSlabSyncAction *action = static_cast<ThickSlabSyncAction*>(m_mappedSyncAction);
    action->setVolume(m_viewer->getMainInput());
    action->setSlabProjectionMode(slabProjectionMode);

    emit actionMapped(m_mappedSyncAction);
}

void ThickSlabSignalToSyncActionMapper::mapThicknessToSyncAction(double slabThickness)
{
    ThickSlabSyncAction *action = static_cast<ThickSlabSyncAction*>(m_mappedSyncAction);
    action->setVolume(m_viewer->getMainInput());
    action->setSlabThickness(slabThickness);

    emit actionMapped(m_mappedSyncAction);
}

}
