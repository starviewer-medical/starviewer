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

#ifndef VOLUMEPIXELDATAREADERSELECTOR_H
#define VOLUMEPIXELDATAREADERSELECTOR_H

#include "volumepixeldatareaderfactory.h"

namespace udg {

class Volume;

/**
 * This class is used by VolumePixelDataReaderFactory to select the appropriate VolumePixelDataReader for a given volume.
 */
class VolumePixelDataReaderSelector {

public:
    virtual ~VolumePixelDataReaderSelector() {}

    /// Chooses and returns the reader implementation most suitable to the given volume.
    virtual VolumePixelDataReaderFactory::PixelDataReaderType selectVolumePixelDataReader(Volume *volume) const = 0;

};

} // namespace udg

#endif
