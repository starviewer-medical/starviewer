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

#include "inputsynccriterion.h"

#include "volume.h"
#include "qviewer.h"
#include "q2dviewer.h"

namespace udg {

InputSyncCriterion::InputSyncCriterion()
 : SyncCriterion()
{
}

InputSyncCriterion::~InputSyncCriterion()
{
}

bool InputSyncCriterion::criterionIsMet(QViewer *viewer1, QViewer *viewer2)
{
    // TODO Should also check if some reconstruction is applied, i.e. thick slab?
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(viewer2);

    if (viewer2D)
    {
        return viewer2D->containsVolume(viewer1->getMainInput());
    }
    else
    {
        return viewer1->getMainInput() == viewer2->getMainInput();
    }
}

} // End namespace udg
