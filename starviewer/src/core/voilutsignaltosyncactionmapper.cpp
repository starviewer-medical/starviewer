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
        for (int i = 0; i < viewer2D->getNumberOfInputs(); i++)
        {
            mapToSyncAction(viewer2D->getInput(i), viewer2D->getVoiLutDataForVolume(i)->getCurrentPreset());
        }
    }
}

void VoiLutSignalToSyncActionMapper::mapSignal()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);
    if (viewer2D)
    {
        for (int i = 0; i < viewer2D->getNumberOfInputs(); i++)
        {
            Volume *volume = viewer2D->getInput(i);
            connections.append(connect(viewer2D->getVoiLutDataForVolume(i), &VoiLutPresetsToolData::currentPresetChanged, [=](const VoiLut &voiLut) {
                mapToSyncAction(volume, voiLut);
            }));
            connections.append(connect(viewer2D->getVoiLutDataForVolume(i), &VoiLutPresetsToolData::presetSelected, [=](const VoiLut &voiLut) {
                mapToSyncAction(volume, voiLut);
            }));
        }
    }
}

void VoiLutSignalToSyncActionMapper::unmapSignal()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);
    if (viewer2D)
    {
        while (!connections.isEmpty())
        {
            disconnect(connections.takeLast());
        }
    }
}

void VoiLutSignalToSyncActionMapper::mapToSyncAction(Volume *volume, const VoiLut &voiLut)
{
    if (!m_mappedSyncAction)
    {
        m_mappedSyncAction = new VoiLutSyncAction();
    }
    static_cast<VoiLutSyncAction*>(m_mappedSyncAction)->setVolume(volume);
    static_cast<VoiLutSyncAction*>(m_mappedSyncAction)->setVoiLut(voiLut);
    
    emit actionMapped(m_mappedSyncAction);
}

} // End namespace udg
