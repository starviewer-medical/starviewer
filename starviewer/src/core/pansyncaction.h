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

#include "vector3.h"

namespace udg {

/**
    Implementation of a SyncAction for a pan action
    The set source focal point on setSourceFocalPoint() will be applied when run() is called
 */
class PanSyncAction : public SyncAction {
public:
    PanSyncAction();
    ~PanSyncAction();

    /// Sets source focal point to be synched
    void setSourceFocalPoint(Vector3 vector);
    
    void run(QViewer *viewer);

protected:
    void setupMetaData();
    void setupDefaultSyncCriteria();

protected:
    /// New focal point of the source viewer that will be applied to the receiving viewer on run()
    Vector3 m_sourceFocalPoint;
};

} // End namespace udg

#endif
