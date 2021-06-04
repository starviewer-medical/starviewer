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

#ifndef UDGPETCTVOLUMEDISPLAYUNITHANDLER_H
#define UDGPETCTVOLUMEDISPLAYUNITHANDLER_H

#include "pairedvolumedisplayunithandler.h"

namespace udg {

/**
    Specific VolumeDisplayUnit handler to handle two inputs, where the modalities of each one should be CT and PET.
    Maximum number of inputs will be two.
*/
class PETCTVolumeDisplayUnitHandler : public PairedVolumeDisplayUnitHandler {

    Q_OBJECT

public:
    PETCTVolumeDisplayUnitHandler(QObject *parent = nullptr);
    virtual ~PETCTVolumeDisplayUnitHandler();

protected:
    /// Implementation of the corresponding virtual methods to configure the default transfer functions and the main input index
    virtual void setupDefaultTransferFunctions();
    virtual void updateMainDisplayUnitIndex();

private:
    /// Returns the display unit corresponding to the PET input
    VolumeDisplayUnit* getPETDisplayUnit() const;
};

} // End namespace udg

#endif
