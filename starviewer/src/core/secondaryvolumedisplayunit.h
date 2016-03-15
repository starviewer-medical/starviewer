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

#ifndef UDG_SECONDARYVOLUMEDISPLAYUNIT_H
#define UDG_SECONDARYVOLUMEDISPLAYUNIT_H

#include "volumedisplayunit.h"

class vtkImageSliceMapper;

namespace udg {

/**
 * @brief The SecondaryVolumeDisplayUnit class is a subclass of VolumeDisplayUnit intended for secondary volumes (e.g. PET).
 *
 * This class uses a vtkImageSliceMapper instead of a vtkImageResliceMapper so that it is able to update the slice without modifying the camera, which should be
 * modified for the main volume.
 */
class SecondaryVolumeDisplayUnit : public VolumeDisplayUnit
{
public:
    SecondaryVolumeDisplayUnit();
    virtual ~SecondaryVolumeDisplayUnit();

    /// Updates the displayed image in the image slice.
    virtual void updateImageSlice(vtkCamera *camera);

private:
    vtkImageSliceMapper *m_mapper;

};

} // namespace udg

#endif // UDG_SECONDARYVOLUMEDISPLAYUNIT_H
