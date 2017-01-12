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

#include "pansyncaction.h"

#include "q2dviewer.h"
#include "anatomicalplanesynccriterion.h"
#include "frameofreferencesynccriterion.h"
#include "anglebetweenplanessynccriterion.h"
#include "anatomicalregionsynccriterion.h"

namespace udg {

PanSyncAction::PanSyncAction()
 : SyncAction()
{
}

PanSyncAction::~PanSyncAction()
{
}

void PanSyncAction::setSourceFocalPoint(Vector3 vector)
{
    m_sourceFocalPoint = std::move(vector);
}

void PanSyncAction::run(QViewer *viewer)
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(viewer);
    if (viewer2D)
    {
        viewer2D->absolutePan(m_sourceFocalPoint);
    }
}

void PanSyncAction::setupMetaData()
{
    m_metaData = SyncActionMetaData("PanSyncAction", QObject::tr("Pan"), "pan");
}

void PanSyncAction::setupDefaultSyncCriteria()
{
    m_defaultSyncCriteria << new AnatomicalPlaneSyncCriterion() << new FrameOfReferenceSyncCriterion() << new AngleBetweenPlanesSyncCriterion() << new AnatomicalRegionSyncCriterion();
}

} // End namespace udg
