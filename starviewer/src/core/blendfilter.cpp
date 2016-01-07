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

#include "blendfilter.h"

#include "filteroutput.h"

#include <vtkImageBlend.h>
#include <vtkImageData.h>

namespace udg {

BlendFilter::BlendFilter()
{
    m_imageBlend = vtkImageBlend::New();
}

BlendFilter::~BlendFilter()
{
    m_imageBlend->Delete();
}

void BlendFilter::setBase(vtkImageData *base)
{
    m_imageBlend->SetInputData(0, base);
}

void BlendFilter::setOverlay(vtkImageData *overlay)
{
    m_imageBlend->SetInputData(1, overlay);
}

void BlendFilter::setOverlayOpacity(double opacity)
{
    m_imageBlend->SetOpacity(1, opacity);
}

vtkAlgorithm* BlendFilter::getVtkAlgorithm() const
{
    return m_imageBlend;
}

}
