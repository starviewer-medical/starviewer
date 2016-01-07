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

#ifndef UDGIMAGEORIENTATIONSYNCACTION_H
#define UDGIMAGEORIENTATIONSYNCACTION_H

#include "syncaction.h"
#include "patientorientation.h"

namespace udg {

/**
    Implementation of a SyncAction for a patient orientation
    The set patient orientation on setImageOrientation() will be applied when run() is called
 */
class ImageOrientationSyncAction : public SyncAction {
public:
    ImageOrientationSyncAction();
    ~ImageOrientationSyncAction();

    /// Sets the patient orientation to be synched
    void setImageOrientation(const PatientOrientation &orientation);
    
    void run(QViewer *viewer);

protected:
    void setupMetaData();
    void setupDefaultSyncCriteria();

protected:
    /// PatientOrientation that will be applied on run()
    PatientOrientation m_orientation;
};

} // End namespace udg

#endif
