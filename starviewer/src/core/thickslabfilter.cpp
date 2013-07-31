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
    m_filter->SetInput(input);
}

void ThickSlabFilter::setInput(FilterOutput input)
{
    m_filter->SetInputConnection(input.getVtkAlgorithmOutput());
}

void ThickSlabFilter::setProjectionAxis(OrthogonalPlane::OrthogonalPlaneType axis)
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
