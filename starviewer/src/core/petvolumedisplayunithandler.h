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

#ifndef PETVOLUMEDISPLAYUNITHANDLER_H
#define PETVOLUMEDISPLAYUNITHANDLER_H

#include "singlevolumedisplayunithandler.h"

namespace udg {

/**
    VolumeDisplayUnit handler to handle a single PET input.
*/
class PETVolumeDisplayUnitHandler : public SingleVolumeDisplayUnitHandler {

    Q_OBJECT

public:
    PETVolumeDisplayUnitHandler(QObject *parent = nullptr);
    virtual ~PETVolumeDisplayUnitHandler();

protected:
    /// Sets up the default transfer functions for the required inputs.
    virtual void setupDefaultTransferFunctions();

};

}

#endif
