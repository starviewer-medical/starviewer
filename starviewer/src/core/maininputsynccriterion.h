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

#ifndef UDG_MAININPUTSYNCCRITERION_H
#define UDG_MAININPUTSYNCCRITERION_H

#include "synccriterion.h"

namespace udg {

/**
 * @brief The MainInputSyncCriterion class implements a SyncCriterion that is met when the main input from the source viewer is contained in the target viewer.
 */
class MainInputSyncCriterion : public SyncCriterion {
protected:
    bool criterionIsMet(QViewer *sourceViewer, QViewer *targetViewer) override;
};

}

#endif
