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
    m_imageBlend->SetInput(0, base);
}

void BlendFilter::setOverlay(vtkImageData *overlay)
{
    m_imageBlend->SetInput(1, overlay);
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
