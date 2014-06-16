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

#include "thickslabfilter.h"

#include "filteroutput.h"

#include <vtkImageData.h>
#include "vtkProjectionImageFilter.h"

namespace udg {

ThickSlabFilter::ThickSlabFilter()
{
    m_filter = vtkProjectionImageFilter::New();
}

ThickSlabFilter::~ThickSlabFilter()
{
    m_filter->Delete();
}

void ThickSlabFilter::setInput(vtkImageData *input)
{
    m_filter->SetInputData(input);
}

void ThickSlabFilter::setInput(FilterOutput input)
{
    m_filter->SetInputConnection(input.getVtkAlgorithmOutput());
}

void ThickSlabFilter::setProjectionAxis(const OrthogonalPlane &axis)
{
    m_filter->SetProjectionDimension(static_cast<unsigned int>(axis));
}

void ThickSlabFilter::setFirstSlice(int slice)
{
    m_filter->SetFirstSlice(slice);
}

void ThickSlabFilter::setSlabThickness(int numberOfSlices)
{
    m_filter->SetNumberOfSlicesToProject(numberOfSlices);
}

int ThickSlabFilter::getSlabThickness()
{
    return m_filter->GetNumberOfSlicesToProject();
}

void ThickSlabFilter::setStride(int stride)
{
    m_filter->SetStep(stride);
}

void ThickSlabFilter::setAccumulatorType(AccumulatorFactory::AccumulatorType type)
{
    m_filter->SetAccumulatorType(type);
}

vtkAlgorithm* ThickSlabFilter::getVtkAlgorithm() const
{
    return m_filter;
}

}
