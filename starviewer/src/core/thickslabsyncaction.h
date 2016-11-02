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

#ifndef THICKSLABSYNCACTION_H
#define THICKSLABSYNCACTION_H

#include "syncaction.h"

#include "volumedisplayunit.h"

namespace udg {

class Volume;

/**
    Implementation of a SyncAction for thick slab propagation.
 */
class ThickSlabSyncAction : public SyncAction {

public:
    ThickSlabSyncAction();
    virtual ~ThickSlabSyncAction();

    /// Sets the slab projection mode to be propagated.
    void setSlabProjectionMode(VolumeDisplayUnit::SlabProjectionMode slabProjectionMode);

    /// Sets the slab thickess to be propagated.
    void setSlabThickness(double slabThickness);

    /// Sets the volume to be thick-slab-synced across viewers.
    void setVolume(Volume *volume);

    /// Propagates the stored thick slab properties to the stored volume in the given viewer.
    virtual void run(QViewer *viewer);

protected:
    /// Sets up the metadata of this sync action.
    virtual void setupMetaData();

    /// Sets up the default list of SyncCriterion.
    virtual void setupDefaultSyncCriteria();

private:
    /// Slab projection mode to be propagated.
    VolumeDisplayUnit::SlabProjectionMode m_slabProjectionMode;

    /// Slab thickness to be propagated.
    double m_slabThickness;

    /// Volume to be thick-slab-synced across viewers.
    Volume *m_volume;

};

}

#endif
