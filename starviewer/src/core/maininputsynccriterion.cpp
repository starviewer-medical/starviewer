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

#include "maininputsynccriterion.h"

#include "q2dviewer.h"

namespace udg {

bool MainInputSyncCriterion::criterionIsMet(QViewer *sourceViewer, QViewer *targetViewer)
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(targetViewer);

    if (viewer2D)
    {
        return viewer2D->containsVolume(sourceViewer->getMainInput());
    }
    else
    {
        return sourceViewer->getMainInput() == targetViewer->getMainInput();
    }
}

} // End namespace udg
