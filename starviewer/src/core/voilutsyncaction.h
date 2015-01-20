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

#ifndef UDGVOILUTSYNCACTION_H
#define UDGVOILUTSYNCACTION_H

#include "syncaction.h"
#include "voilut.h"

namespace udg {

class Volume;

/**
    Implementation of a SyncAction for a VOI LUT
    The set VOI LUT on setVoiLut() will be applied when run() is called
 */
class VoiLutSyncAction : public SyncAction {
public:
    VoiLutSyncAction();
    ~VoiLutSyncAction();

    /// Sets the VOI LUT to be synched
    void setVoiLut(const VoiLut &voiLut);
    void setVolume(Volume *volume);
    
    void run(QViewer *viewer);

protected:
    void setupMetaData();
    void setupDefaultSyncCriteria();

protected:
    /// VOI LUT that will be applied on run()
    VoiLut m_voiLut;
    Volume *m_volume;
};

} // End namespace udg

#endif
