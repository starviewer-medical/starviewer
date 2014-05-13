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

#ifndef UDGWINDOWLEVELSYNCACTION_H
#define UDGWINDOWLEVELSYNCACTION_H

#include "syncaction.h"
#include "windowlevel.h"

namespace udg {

class Volume;

/**
    Implementation of a SyncAction for a window level
    The set window level on setWindowLevel() will be applied when run() is called
 */
class WindowLevelSyncAction : public SyncAction {
public:
    WindowLevelSyncAction();
    ~WindowLevelSyncAction();

    /// Sets the window level to be synched
    void setWindowLevel(const WindowLevel &windowLevel);
    void setVolume(Volume *volume);
    
    void run(QViewer *viewer);

protected:
    void setupMetaData();
    void setupDefaultSyncCriteria();

protected:
    /// Window level that will be applied on run()
    WindowLevel m_windowLevel;
    Volume *m_volume;
};

} // End namespace udg

#endif
