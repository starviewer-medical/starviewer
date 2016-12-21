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

#ifndef UDGPANSIGNALTOSYNCACTIONMAPPER_H
#define UDGPANSIGNALTOSYNCACTIONMAPPER_H

#include "signaltosyncactionmapper.h"

#include "vector3.h"

namespace udg {

/**
    Implementation of SignalToSyncActionMapper to map motion vector changes to PanSyncAction
 */
class PanSignalToSyncActionMapper : public SignalToSyncActionMapper {
Q_OBJECT
public:
    PanSignalToSyncActionMapper(QObject *parent = 0);
    ~PanSignalToSyncActionMapper();

    /// Maps current pan factor of the viewer
    virtual void mapProperty();

protected:
    void mapSignal();
    void unmapSignal();

protected slots:
    /// Maps given factor to a PanSyncAction with the corresponding values
    /// Should be connected to the corresponding signals on Q*Viewer
    void mapToSyncAction(const Vector3 &vector);
};

} // End namespace udg

#endif
