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

#ifndef THICKSLABSIGNALTOSYNCACTIONMAPPER_H
#define THICKSLABSIGNALTOSYNCACTIONMAPPER_H

#include "signaltosyncactionmapper.h"

#include "volumedisplayunit.h"

namespace udg {

/**
    Implementation of SignalToSyncActionMapper to map thick slab changes to ThicSlabSyncAction.
 */
class ThickSlabSignalToSyncActionMapper : public SignalToSyncActionMapper {

    Q_OBJECT

public:
    ThickSlabSignalToSyncActionMapper(QObject *parent = 0);
    virtual ~ThickSlabSignalToSyncActionMapper();

    /// Maps both current projection mode and slab thickness of the given viewer
    virtual void mapProperty();

protected:
    /// Maps the thick slab signals from the viewer to the actionMapped(SyncAction*) signal.
    virtual void mapSignal();

    /// Unmaps the thick slab signals from the viewer to the actionMapped(SyncAction*) signal.
    virtual void unmapSignal();

private slots:
    /// Updates the mapped sync action with the given slab projection mode and the viewer's main volume and emits the actionMapped(SyncAction*) signal.
    void mapProjectionModeToSyncAction(VolumeDisplayUnit::SlabProjectionMode slabProjectionMode);
    /// Updates the mapped sync action with the given slab thickness and the viewer's main volume and emits the actionMapped(SyncAction*) signal.
    void mapThicknessToSyncAction(double slabThickness);

};

}

#endif
