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

#include "zoomfactorsyncaction.h"

#include "qviewer.h"
#include "anatomicalplanesynccriterion.h"
#include "modalitysynccriterion.h"

namespace udg {

ZoomFactorSyncAction::ZoomFactorSyncAction()
 : SyncAction()
{
    m_zoomFactor = 1.0;
}

ZoomFactorSyncAction::~ZoomFactorSyncAction()
{
}

void ZoomFactorSyncAction::setZoomFactor(double factor)
{
    m_zoomFactor = factor;
}

void ZoomFactorSyncAction::setZoomCenter(QPoint zoomCenter)
{
    m_zoomCenter = zoomCenter;
}

void ZoomFactorSyncAction::run(QViewer *viewer)
{
    if (viewer)
    {
        viewer->absoluteZoom(m_zoomFactor, m_zoomCenter);
    }
}

void ZoomFactorSyncAction::setupMetaData()
{
    m_metaData = SyncActionMetaData("ZoomFactorSyncAction", QObject::tr("Zoom"), "zoom");
}

void ZoomFactorSyncAction::setupDefaultSyncCriteria()
{
    m_defaultSyncCriteria << new AnatomicalPlaneSyncCriterion() << new ModalitySyncCriterion();
}

} // End namespace udg
