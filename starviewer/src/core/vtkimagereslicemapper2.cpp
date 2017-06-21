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

#include <vtkImageProperty.h>
#include <vtkImageSlice.h>
#include <vtkImageResliceToColors.h>
#include <vtkObjectFactory.h>
#include <vtkScalarsToColors.h>

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

void VtkImageResliceMapper2::Render(vtkRenderer *renderer, vtkImageSlice *prop)
{
    // Set background level at negative infinity by default, but set it at positive infinity if we detect inverse window level
    double level = -std::numeric_limits<double>::infinity();
    vtkScalarsToColors *lut = prop->GetProperty()->GetLookupTable();

    if (lut)
    {
        double *range = lut->GetRange();
        double lowColor[3], highColor[3];
        lut->GetColor(range[0], lowColor);
        lut->GetColor(range[1], highColor);

        auto isGray = [](double color[3]) {
            return color[0] == color[1] && color[1] == color[2];
        };

        if (isGray(lowColor) && isGray(highColor) && lowColor[0] > highColor[0])
        {
            level = -level;
        }

        if (this->ImageReslice->GetBackgroundLevel() != level)
        {
            this->ImageReslice->SetBackgroundLevel(level);
            this->ResliceNeedUpdate = 1;
        }
    }

    vtkImageResliceMapper::Render(renderer, prop);
}

VtkImageResliceMapper2::VtkImageResliceMapper2()
{
}

VtkImageResliceMapper2::~VtkImageResliceMapper2()
{
}

} // namespace udg
