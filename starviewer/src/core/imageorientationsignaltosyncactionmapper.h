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

#ifndef UDGIMAGEORIENTATIONSIGNALTOSYNCACTIONMAPPER_H
#define UDGIMAGEORIENTATIONSIGNALTOSYNCACTIONMAPPER_H

#include "signaltosyncactionmapper.h"

namespace udg {

class PatientOrientation;

/**
    Implementation of SignalToSyncActionMapper to map patient orientation changes to ImageOrientationSyncAction
 */
class ImageOrientationSignalToSyncActionMapper : public SignalToSyncActionMapper {
Q_OBJECT
public:
    ImageOrientationSignalToSyncActionMapper(QObject *parent = 0);
    ~ImageOrientationSignalToSyncActionMapper();

    /// Maps current displayed image patient orientation
    virtual void mapProperty();

protected:
    void mapSignal();
    void unmapSignal();

protected slots:
    /// Maps given PatientOrientation to a PatientOrientationSyncAction with the corresponding values
    /// Should be connected to the corresponding signals on Q*Viewer
    void mapToSyncAction(const PatientOrientation &orientation);
};

} // End namespace udg

#endif
