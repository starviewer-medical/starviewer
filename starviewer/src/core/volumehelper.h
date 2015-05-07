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

#ifndef UDGVOLUMEHELPER_H
#define UDGVOLUMEHELPER_H

namespace udg {

class Volume;

/**
    Helper class to get data from Volume
 */
class VolumeHelper {
public:
    VolumeHelper();
    ~VolumeHelper();

    /// Returns true if the volume is a primary CT acquisition.
    static bool isPrimaryCT(Volume *volume);

    /// Returns true if the volume is a primary PET acquisition
    static bool isPrimaryPET(Volume *volume);

    /// Returns true if the volume is a primary NM acquisition
    static bool isPrimaryNM(Volume *volume);
};

} // End namespace udg

#endif
