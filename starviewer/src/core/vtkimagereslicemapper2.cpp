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

#include "vtkimagereslicemapper2.h"

#include <vtkImageResliceToColors.h>
#include <vtkObjectFactory.h>

namespace udg {

vtkStandardNewMacro(VtkImageResliceMapper2)

vtkImageData* VtkImageResliceMapper2::getResliceOutput()
{
    this->ImageReslice->UpdateWholeExtent();
    return this->ImageReslice->GetOutput();
}

vtkMatrix4x4* VtkImageResliceMapper2::getSliceToWorldMatrix() const
{
    return this->SliceToWorldMatrix;
}

VtkImageResliceMapper2::VtkImageResliceMapper2()
{
}

VtkImageResliceMapper2::~VtkImageResliceMapper2()
{
}

} // namespace udg
