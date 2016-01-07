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

#include "imageorientationsyncaction.h"

#include "q2dviewer.h"
#include "anatomicalplanesynccriterion.h"
#include "modalitysynccriterion.h"
#include "singlesliceorvolumetricsynccriterion.h"

namespace udg {

ImageOrientationSyncAction::ImageOrientationSyncAction()
 : SyncAction()
{
}

ImageOrientationSyncAction::~ImageOrientationSyncAction()
{
}

void ImageOrientationSyncAction::setImageOrientation(const PatientOrientation &orientation)
{
    m_orientation = orientation;
}

void ImageOrientationSyncAction::run(QViewer *viewer)
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(viewer);
    if (viewer2D)
    {
        viewer2D->setImageOrientation(m_orientation);
    }
}

void ImageOrientationSyncAction::setupMetaData()
{
    m_metaData = SyncActionMetaData("ImageOrientationSyncAction", QObject::tr("Image orientation"), "imageOrientation");
}

void ImageOrientationSyncAction::setupDefaultSyncCriteria()
{
    m_defaultSyncCriteria << new AnatomicalPlaneSyncCriterion() << new ModalitySyncCriterion() << new SingleSliceOrVolumetricSyncCriterion();
}

} // End namespace udg
