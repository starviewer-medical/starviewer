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

#ifndef UDPANSYNCACTION_H
#define UDPANSYNCACTION_H

#include "syncaction.h"

namespace udg {

/**
    Implementation of a SyncAction for a pan action
    The set motion vector on setMotionVector() will be applied when run() is called
 */
class PanSyncAction : public SyncAction {
public:
    PanSyncAction();
    ~PanSyncAction();

    /// Sets motion vector to be synched
    void setMotionVector(double vector[3]);
    
    void run(QViewer *viewer);

protected:
    void setupMetaData();
    void setupDefaultSyncCriteria();

protected:
    /// Motion vector that will be applied on run()
    double m_motionVector[3];
};

} // End namespace udg

#endif
