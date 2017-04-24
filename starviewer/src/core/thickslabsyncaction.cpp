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

#include "thickslabsyncaction.h"

#include "anatomicalplanesynccriterion.h"
#include "inputsynccriterion.h"
#include "q2dviewer.h"

namespace udg {

ThickSlabSyncAction::ThickSlabSyncAction()
 : SyncAction(), m_slabProjectionMode(VolumeDisplayUnit::Max), m_slabThickness(0.0), m_volume(0)
{
}

ThickSlabSyncAction::~ThickSlabSyncAction()
{
}

void ThickSlabSyncAction::setSlabProjectionMode(VolumeDisplayUnit::SlabProjectionMode slabProjectionMode)
{
    m_slabProjectionMode = slabProjectionMode;
}

void ThickSlabSyncAction::setSlabThickness(double slabThickness)
{
    m_slabThickness = slabThickness;
}

void ThickSlabSyncAction::setVolume(Volume *volume)
{
    m_volume = volume;
}

void ThickSlabSyncAction::run(QViewer *viewer)
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(viewer);

    // Limit action to 2D viewers and to secondary inputs
    if (viewer2D && viewer2D->getMainInput() != m_volume)
    {
        viewer2D->setSlabProjectionModeInVolume(viewer2D->indexOfVolume(m_volume), m_slabProjectionMode);
        viewer2D->setSlabThicknessInVolume(viewer2D->indexOfVolume(m_volume), m_slabThickness);
    }
}

void ThickSlabSyncAction::setupMetaData()
{
    m_metaData = SyncActionMetaData("ThickSlabSyncAction", QObject::tr("Thick slab"), "thickSlab");
}

void ThickSlabSyncAction::setupDefaultSyncCriteria()
{
    m_defaultSyncCriteria << new AnatomicalPlaneSyncCriterion() << new InputSyncCriterion();
}

}
