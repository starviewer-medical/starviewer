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

#include "voilutsignaltosyncactionmapper.h"

#include "q2dviewer.h"
#include "voilutpresetstooldata.h"
#include "voilutsyncaction.h"

namespace udg {

VoiLutSignalToSyncActionMapper::VoiLutSignalToSyncActionMapper(QObject *parent)
 : SignalToSyncActionMapper(parent)
{
}

VoiLutSignalToSyncActionMapper::~VoiLutSignalToSyncActionMapper()
{
}

void VoiLutSignalToSyncActionMapper::mapProperty()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);
    if (viewer2D)
    {
        mapToSyncAction(viewer2D->getVoiLutData()->getCurrentPreset());
    }
}

void VoiLutSignalToSyncActionMapper::mapSignal()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);
    if (viewer2D)
    {
        connect(viewer2D->getVoiLutData(), SIGNAL(currentPresetChanged(VoiLut)), SLOT(mapToSyncAction(VoiLut)));
        connect(viewer2D->getVoiLutData(), SIGNAL(presetSelected(VoiLut)), SLOT(mapToSyncAction(VoiLut)));
    }
}

void VoiLutSignalToSyncActionMapper::unmapSignal()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);
    if (viewer2D)
    {
        disconnect(viewer2D->getVoiLutData(), SIGNAL(currentPresetChanged(VoiLut)), this, SLOT(mapToSyncAction(VoiLut)));
        disconnect(viewer2D->getVoiLutData(), SIGNAL(presetSelected(VoiLut)), this, SLOT(mapToSyncAction(VoiLut)));
    }
}

void VoiLutSignalToSyncActionMapper::mapToSyncAction(const VoiLut &voiLut)
{
    if (!m_mappedSyncAction)
    {
        m_mappedSyncAction = new VoiLutSyncAction();
    }
    static_cast<VoiLutSyncAction*>(m_mappedSyncAction)->setVolume(m_viewer->getMainInput());
    static_cast<VoiLutSyncAction*>(m_mappedSyncAction)->setVoiLut(voiLut);
    
    emit actionMapped(m_mappedSyncAction);
}

} // End namespace udg
