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
