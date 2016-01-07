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

#ifndef UDZOOMFACTORSYNCACTION_H
#define UDZOOMFACTORSYNCACTION_H

#include "syncaction.h"

namespace udg {

/**
    Implementation of a SyncAction for a zoom factor
    The set zoom factor on setZoomFactor() will be applied when run() is called
 */
class ZoomFactorSyncAction : public SyncAction {
public:
    ZoomFactorSyncAction();
    ~ZoomFactorSyncAction();

    /// Sets zoom factor to be synched
    void setZoomFactor(double factor);
    
    void run(QViewer *viewer);

protected:
    void setupMetaData();
    void setupDefaultSyncCriteria();

protected:
    /// Zoom factor that will be applied on run()
    double m_zoomFactor;
};

} // End namespace udg

#endif
