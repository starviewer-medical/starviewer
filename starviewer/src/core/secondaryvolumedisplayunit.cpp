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

#include "orthogonalplane.h"
#include "secondaryvolumedisplayunit.h"
#include "slicehandler.h"
#include "volume.h"

#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>

namespace udg {

SecondaryVolumeDisplayUnit::SecondaryVolumeDisplayUnit()
{
    m_mapper = vtkImageSliceMapper::New();
    m_imageSlice->SetMapper(m_mapper);
}

SecondaryVolumeDisplayUnit::~SecondaryVolumeDisplayUnit()
{
    m_mapper->Delete();
}

void SecondaryVolumeDisplayUnit::updateImageSlice(vtkCamera *camera)
{
    Q_UNUSED(camera)

    if (!m_volume || !m_volume->isPixelDataLoaded())
    {
        return;
    }

    int zIndex = this->getViewPlane().getZIndex();
    m_mapper->SetOrientation(zIndex);
    int imageIndex = m_volume->getImageIndex(m_sliceHandler->getCurrentSlice(), m_sliceHandler->getCurrentPhase());
    m_mapper->SetSliceNumber(imageIndex);
}

} // namespace udg
