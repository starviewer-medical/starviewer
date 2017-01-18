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

#include "anglebetweenplanessynccriterion.h"

#include "q2dviewer.h"
#include "imageplane.h"
#include "mathtools.h"
#include "logging.h"

namespace udg {

AngleBetweenPlanesSyncCriterion::AngleBetweenPlanesSyncCriterion()
: SyncCriterion()
{
}

AngleBetweenPlanesSyncCriterion::~AngleBetweenPlanesSyncCriterion()
{
}

bool AngleBetweenPlanesSyncCriterion::criterionIsMet(QViewer *viewer1, QViewer *viewer2)
{
    return criterionIsMet(Q2DViewer::castFromQViewer(viewer1), Q2DViewer::castFromQViewer(viewer2));
}

bool AngleBetweenPlanesSyncCriterion::criterionIsMet(Q2DViewer *viewer1, Q2DViewer *viewer2)
{
    if (!viewer1 || !viewer2)
    {
        return false;
    }
    else
    {
        QSharedPointer<ImagePlane> imagePlane1 = viewer1->getCurrentImagePlane();
        QSharedPointer<ImagePlane> imagePlane2 = viewer2->getCurrentImagePlane();

        if (!imagePlane1 || !imagePlane2)
        {
            return false;
        }
        
        double angle = MathTools::angleInDegrees(imagePlane1->getImageOrientation().getNormalVector(), imagePlane2->getImageOrientation().getNormalVector());
        
        // As reported in ticket #1897, the angle could be NaN when both vectors are equals.
        // In order to treat NaN like zero, this condition is changed.
        // When #1897 is solved, this should be equal to (fabs(angle) <= 45.0 || fabs(angle) >= 135.0).
        return !(fabs(angle) > 45.0 && fabs(angle) < 135.0);
    }
}

} // End namespace udg
