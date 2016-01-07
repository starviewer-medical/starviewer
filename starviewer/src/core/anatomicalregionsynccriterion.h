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

#ifndef UDG_ANATOMICALREGIONSYNCCRITERION_H
#define UDG_ANATOMICALREGIONSYNCCRITERION_H

#include "synccriterion.h"
#include "anatomicalplane.h"

namespace udg {

class Q2DViewer;


/// Implements the SyncCriterion for anatomical region restriction.

class AnatomicalRegionSyncCriterion : public SyncCriterion
{
public:
    AnatomicalRegionSyncCriterion();
    ~AnatomicalRegionSyncCriterion();

protected:
    bool criterionIsMet(QViewer *viewer1, QViewer *viewer2);

private:
    bool criterionIsMet(Q2DViewer *viewer1, Q2DViewer *viewer2);
    int getYIndex(const AnatomicalPlane &originalPlane, const AnatomicalPlane &currentPlane);
};

} // namespace udg

#endif // UDG_ANATOMICALREGIONSYNCCRITERION_H
