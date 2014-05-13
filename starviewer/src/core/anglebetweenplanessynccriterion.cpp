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
        ImagePlane *imagePlane1 = viewer1->getCurrentImagePlane();
        ImagePlane *imagePlane2 = viewer2->getCurrentImagePlane();

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
