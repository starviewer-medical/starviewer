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

#ifndef PHASESIGNALTOSYNCACTIONMAPPER_H
#define PHASESIGNALTOSYNCACTIONMAPPER_H

#include "signaltosyncactionmapper.h"

namespace udg {

/**
    Implementation of SignalToSyncActionMapper to map phase changes to PhaseSyncAction.
 */
class PhaseSignalToSyncActionMapper : public SignalToSyncActionMapper {

    Q_OBJECT

public:
    PhaseSignalToSyncActionMapper(QObject *parent = 0);
    virtual ~PhaseSignalToSyncActionMapper();

    /// Maps current phase of the viewer
    virtual void mapProperty();

protected:
    /// Maps the phaseChanged(int) signal from the viewer to the actionMapped(SyncAction*) signal.
    virtual void mapSignal();

    /// Unmaps the phaseChanged(int) signal from the viewer to the actionMapped(SyncAction*) signal.
    virtual void unmapSignal();

private slots:
    /// Updates the mapped sync action with the given phase index and the viewer's main volume and emits the actionMapped(SyncAction*) signal.
    void mapToSyncAction(int phase);

};

}

#endif
