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

#include "anatomicalplanesynccriterion.h"

#include "q2dviewer.h"

namespace udg {

AnatomicalPlaneSyncCriterion::AnatomicalPlaneSyncCriterion()
 : SyncCriterion()
{
}

AnatomicalPlaneSyncCriterion::~AnatomicalPlaneSyncCriterion()
{
}

bool AnatomicalPlaneSyncCriterion::criterionIsMet(QViewer *viewer1, QViewer *viewer2)
{
    return criterionIsMet(Q2DViewer::castFromQViewer(viewer1), Q2DViewer::castFromQViewer(viewer2));
}

bool AnatomicalPlaneSyncCriterion::criterionIsMet(Q2DViewer *viewer1, Q2DViewer *viewer2)
{
    if (!viewer1 || !viewer2)
    {
        return false;
    }
    else
    {
        AnatomicalPlane anatomicalPlane = viewer1->getCurrentAnatomicalPlane();
        if (anatomicalPlane != AnatomicalPlane::Axial && anatomicalPlane != AnatomicalPlane::Sagittal && anatomicalPlane != AnatomicalPlane::Coronal)
        {
            return false;
        }
        else
        {
            return anatomicalPlane == viewer2->getCurrentAnatomicalPlane();
        }
    }
}

} // End namespace udg
