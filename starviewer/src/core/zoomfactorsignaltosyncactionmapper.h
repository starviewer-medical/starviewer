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

#ifndef UDGZOOMFACTORSIGNALTOSYNCACTIONMAPPER_H
#define UDGZOOMFACTORSIGNALTOSYNCACTIONMAPPER_H

#include "signaltosyncactionmapper.h"

namespace udg {

/**
    Implementation of SignalToSyncActionMapper to map zoom factor changes to ZoomFactorSyncAction
 */
class ZoomFactorSignalToSyncActionMapper : public SignalToSyncActionMapper {
Q_OBJECT
public:
    ZoomFactorSignalToSyncActionMapper(QObject *parent = 0);
    ~ZoomFactorSignalToSyncActionMapper();

    /// Maps current zoom factor of the viewer
    virtual void mapProperty();

protected:
    void mapSignal();
    void unmapSignal();

protected slots:
    /// Maps given factor to a ZoomFactorSyncAction with the corresponding values
    /// Should be connected to the corresponding signals on Q*Viewer
    void mapToSyncAction(double factor);
};

} // End namespace udg

#endif
