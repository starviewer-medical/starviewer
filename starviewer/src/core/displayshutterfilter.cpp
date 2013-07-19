#include "displayshutterfilter.h"

#include "filteroutput.h"
#include "volume.h"

#include <vtkImageMask.h>

namespace udg {

DisplayShutterFilter::DisplayShutterFilter()
{
    m_imageMask = vtkImageMask::New();
    m_imageMask->SetMaskAlpha(1.0);
    m_imageMask->SetMaskedOutputValue(0);
    m_imageMask->NotMaskOn();
}

DisplayShutterFilter::~DisplayShutterFilter()
{
    m_imageMask->Delete();
}

void DisplayShutterFilter::setInput(Volume *input)
{
    m_imageMask->SetImageInput(input->getVtkData());
}

void DisplayShutterFilter::setInput(vtkImageData *input)
{
    m_imageMask->SetImageInput(input);
}

void DisplayShutterFilter::setInput(FilterOutput input)
{
    this->setInput(input.getVtkImageData());
}

void DisplayShutterFilter::setDisplayShutter(vtkImageData *displayShutter)
{
    m_imageMask->SetMaskInput(displayShutter);
}

void DisplayShutterFilter::setDisplayShutter(FilterOutput displayShutter)
{
    this->setDisplayShutter(displayShutter.getVtkImageData());
}

vtkAlgorithm* DisplayShutterFilter::getVtkAlgorithm() const
{
    return m_imageMask;
}

}
