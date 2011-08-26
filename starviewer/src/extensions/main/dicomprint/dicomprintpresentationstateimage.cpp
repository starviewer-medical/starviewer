#include "dicomprintpresentationstateimage.h"

namespace udg {

DICOMPrintPresentationStateImage::DICOMPrintPresentationStateImage()
{
    m_windowWidth = 0;
    m_windowCenter = 0;

    m_applyDefaultWindowLevelToImage = true;
    m_horitzontalFlip = false;

    m_rotateClockWiseTimes = 0;
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

bool DICOMPrintPresentationStateImage::getIsFlipped() const
{
    return m_horitzontalFlip;
}

void DICOMPrintPresentationStateImage::setIsFlipped(bool horitzontalFlip)
{
    m_horitzontalFlip = horitzontalFlip;
}

void DICOMPrintPresentationStateImage::setRotateClockWise(int times)
{
    m_rotateClockWiseTimes = times;
}

int DICOMPrintPresentationStateImage::getRotateClockWise() const
{
    return m_rotateClockWiseTimes;
}

}
