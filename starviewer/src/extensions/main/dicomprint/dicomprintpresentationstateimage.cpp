#include "dicomprintpresentationstateimage.h"

namespace udg {

DICOMPrintPresentationStateImage::DICOMPrintPresentationStateImage()
{
    m_windowWidth = 0;
    m_windowCenter = 0;

    m_applyDefaultWindowLevelToImage = true;
}

double DICOMPrintPresentationStateImage::getWindowCenter() const
{
    return m_windowCenter;
}

double DICOMPrintPresentationStateImage::getWindowWidth() const
{
    return m_windowWidth;
}

void DICOMPrintPresentationStateImage::setWindowLevel(double windowWidth, double windowCenter)
{
    m_applyDefaultWindowLevelToImage = false;

    m_windowCenter = windowCenter;
    m_windowWidth = windowWidth;
}

bool DICOMPrintPresentationStateImage::applyDefaultWindowLevelToImage() const
{
    return m_applyDefaultWindowLevelToImage;
}


}
