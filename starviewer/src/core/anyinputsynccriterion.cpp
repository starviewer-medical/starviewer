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

#include "anyinputsynccriterion.h"

#include "q2dviewer.h"

namespace udg {

bool AnyInputSyncCriterion::criterionIsMet(QViewer *sourceViewer, QViewer *targetViewer)
{
    QList<Volume*> sourceInputs, targetInputs;
    Q2DViewer *source2D = Q2DViewer::castFromQViewer(sourceViewer);
    Q2DViewer *target2D = Q2DViewer::castFromQViewer(targetViewer);

    if (source2D)
    {
        sourceInputs = source2D->getInputs();
    }
    else
    {
        sourceInputs.append(sourceViewer->getMainInput());
    }

    if (target2D)
    {
        targetInputs = target2D->getInputs();
    }
    else
    {
        targetInputs.append(targetViewer->getMainInput());
    }

    return sourceInputs.toSet().intersects(targetInputs.toSet());
}

} // namespace udg
