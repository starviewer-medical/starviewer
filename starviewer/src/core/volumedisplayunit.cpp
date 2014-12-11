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

#include "volumedisplayunit.h"

#include "imagepipeline.h"
#include "slicehandler.h"
#include "volume.h"
#include "voilutpresetstooldata.h"
#include "volumepixeldata.h"
#include "image.h"
#include "windowlevelhelper.h"

#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include <vtkPropPicker.h>
#include <vtkImageProperty.h>

namespace udg {

VolumeDisplayUnit::VolumeDisplayUnit()
 : m_volume(0)
{
    m_imagePipeline = new ImagePipeline();
    m_imageActor = vtkImageActor::New();
    m_imageActor->GetProperty()->SetInterpolationTypeToCubic();
    m_sliceHandler = new SliceHandler();
    m_imagePointPicker =  0;
    m_windowLevelData = 0;
    m_currentThickSlabPixelData = 0;
}

VolumeDisplayUnit::~VolumeDisplayUnit()
{
    delete m_imagePipeline;
    m_imageActor->Delete();
    delete m_sliceHandler;
    
    if (m_imagePointPicker)
    {
        m_imagePointPicker->Delete();
    }
    delete m_currentThickSlabPixelData;
}

Volume* VolumeDisplayUnit::getVolume() const
{
    return m_volume;
}

void VolumeDisplayUnit::setVolume(Volume *volume)
{
    m_volume = volume;
    m_sliceHandler->setVolume(volume);

    resetThickSlab();

    m_imageActor->GetMapper()->SetInputConnection(m_imagePipeline->getOutput().getVtkAlgorithmOutput());
}

void VolumeDisplayUnit::setWindowLevelData(VoiLutPresetsToolData *windowLevelData)
{
    m_windowLevelData = windowLevelData;
    WindowLevelHelper().initializeWindowLevelData(m_windowLevelData, m_volume);
}

VoiLutPresetsToolData *VolumeDisplayUnit::getWindowLevelData()
{
    return m_windowLevelData;
}

ImagePipeline* VolumeDisplayUnit::getImagePipeline() const
{
    return m_imagePipeline;
}

vtkImageActor* VolumeDisplayUnit::getImageActor() const
{
    return m_imageActor;
}

vtkPropPicker* VolumeDisplayUnit::getImagePointPicker()
{
    if (!m_imagePointPicker)
    {
        setupPicker();
    }

    return m_imagePointPicker;
}

const OrthogonalPlane& VolumeDisplayUnit::getViewPlane() const
{
    return m_sliceHandler->getViewPlane();
}

void VolumeDisplayUnit::setViewPlane(const OrthogonalPlane &viewPlane)
{
    m_sliceHandler->setViewPlane(viewPlane);
    m_imagePipeline->setProjectionAxis(viewPlane);
}

double VolumeDisplayUnit::getCurrentSpacingBetweenSlices() const
{
    if (getVolume())
    {
        int zIndex = getViewPlane().getZIndex();
        
        return getVolume()->getSpacing()[zIndex];
    }
    else
    {
        return 0.0;
    }
}

double VolumeDisplayUnit::getCurrentDisplayedImageDepth() const
{
    if (getVolume())
    {
        double zSpacing = getCurrentSpacingBetweenSlices();
        int zIndex = getViewPlane().getZIndex();
        double zOrigin = getVolume()->getOrigin()[zIndex];
        
        return zOrigin + zSpacing * getSlice();
    }
    else
    {
        return 0.0;
    }
}

VolumePixelData* VolumeDisplayUnit::getCurrentPixelData()
{
    if (!m_volume)
    {
        return 0;
    }
    
    if (isThickSlabActive())
    {
        if (!m_currentThickSlabPixelData)
        {
            m_currentThickSlabPixelData = new VolumePixelData;
            m_currentThickSlabPixelData->setNumberOfPhases(m_volume->getNumberOfPhases());
            m_currentThickSlabPixelData->setData(getImagePipeline()->getSlabProjectionOutput());
        }

        return m_currentThickSlabPixelData;
    }
    else
    {
        return m_volume->getPixelData();
    }
}

Image* VolumeDisplayUnit::getCurrentDisplayedImage() const
{
    if (m_volume && getViewPlane() == OrthogonalPlane::XYPlane)
    {
        return m_volume->getImage(m_sliceHandler->getCurrentSlice(), m_sliceHandler->getCurrentPhase());
    }
    else
    {
        return 0;
    }
}

void VolumeDisplayUnit::updateDisplayExtent()
{
    if (!m_volume || !m_volume->isPixelDataLoaded())
    {
        return;
    }

    int imageIndex = m_volume->getImageIndex(m_sliceHandler->getCurrentSlice(), m_sliceHandler->getCurrentPhase());
    int zIndex = this->getViewPlane().getZIndex();
    int displayExtent[6];
    m_volume->getExtent(displayExtent);
    displayExtent[zIndex * 2] = displayExtent[zIndex * 2 + 1] = imageIndex;
    m_imageActor->SetDisplayExtent(displayExtent);
}

int VolumeDisplayUnit::getSlice() const
{
    return m_sliceHandler->getCurrentSlice();
}

void VolumeDisplayUnit::setSlice(int slice)
{
    m_sliceHandler->setSlice(slice);
    m_imagePipeline->setSlice(m_volume->getImageIndex(getSlice(), getPhase()));
}

int VolumeDisplayUnit::getMinimumSlice() const
{
    return m_sliceHandler->getMinimumSlice();
}

int VolumeDisplayUnit::getMaximumSlice() const
{
    return m_sliceHandler->getMaximumSlice();
}

int VolumeDisplayUnit::getNumberOfSlices() const
{
    return m_sliceHandler->getNumberOfSlices();
}

int VolumeDisplayUnit::getPhase() const
{
    return m_sliceHandler->getCurrentPhase();
}

void VolumeDisplayUnit::setPhase(int phase)
{
    m_sliceHandler->setPhase(phase);
    m_imagePipeline->setSlice(m_volume->getImageIndex(getSlice(), getPhase()));
}

int VolumeDisplayUnit::getNumberOfPhases() const
{
    return m_sliceHandler->getNumberOfPhases();
}

int VolumeDisplayUnit::getSlabThickness() const
{
    return m_sliceHandler->getSlabThickness();
}

void VolumeDisplayUnit::setSlabThickness(int thickness)
{
    // Make sure thickness is within valid bounds. Must be between 1 and the maximum number of slices on the curren view.
    int admittedThickness = qBound(1, thickness, getNumberOfSlices());
    
    m_sliceHandler->setSlabThickness(admittedThickness);
    m_imagePipeline->setSlice(m_volume->getImageIndex(getSlice(), getPhase()));
    m_imagePipeline->setSlabThickness(admittedThickness);
}

bool VolumeDisplayUnit::isThickSlabActive() const
{
    return getSlabThickness() > 1;
}

double VolumeDisplayUnit::getSliceThickness() const
{
    return m_sliceHandler->getSliceThickness();
}

void VolumeDisplayUnit::resetThickSlab()
{
    if (m_volume)
    {
        m_imagePipeline->setInput(m_volume->getVtkData());
        m_imagePipeline->setProjectionAxis(this->getViewPlane());
        m_imagePipeline->setSlice(m_volume->getImageIndex(m_sliceHandler->getCurrentSlice(), m_sliceHandler->getCurrentPhase()));
        m_imagePipeline->setSlabThickness(m_sliceHandler->getSlabThickness());
        m_imagePipeline->setSlabStride(m_sliceHandler->getNumberOfPhases());
    }
}

void VolumeDisplayUnit::setupPicker()
{
    m_imagePointPicker = vtkPropPicker::New();
    m_imagePointPicker->InitializePickList();
    m_imagePointPicker->AddPickList(getImageActor());
    m_imagePointPicker->PickFromListOn();
}

void VolumeDisplayUnit::updateCurrentImageDefaultPresets()
{
    if (getViewPlane() == OrthogonalPlane::XYPlane)
    {
        Image *image = getVolume()->getImage(m_sliceHandler->getCurrentSlice(), m_sliceHandler->getCurrentPhase());
        if (image)
        {
            for (int i = 0; i < image->getNumberOfWindowLevels(); ++i)
            {
                WindowLevel windowLevel = WindowLevelHelper().getDefaultWindowLevelForPresentation(image, i);
                m_windowLevelData->updatePreset(windowLevel);
            }
        }
    }
    
    WindowLevel wl = m_windowLevelData->getCurrentPreset().getWindowLevel();
    m_imagePipeline->setWindowLevel(wl.getWidth(), wl.getCenter());
}

void VolumeDisplayUnit::updateWindowLevel(const WindowLevel &windowLevel)
{
    m_windowLevelData->setCurrentPreset(windowLevel);
    m_imagePipeline->setWindowLevel(windowLevel.getWidth(), windowLevel.getCenter());
}

void VolumeDisplayUnit::getWindowLevel(double windowLevel[2]) const
{
    m_imagePipeline->getCurrentWindowLevel(windowLevel);
}

void VolumeDisplayUnit::setWindowLevel(double window, double level)
{
    m_imagePipeline->setWindowLevel(window, level);
}

void VolumeDisplayUnit::setTransferFunction(const TransferFunction &transferFunction)
{
    if (transferFunction.isEmpty())
    {
        // If an empty transfer function is received, it's interpreted as clearTransferFunction()
        clearTransferFunction();
        m_transferFunction.setName(transferFunction.name());
    }
    else
    {
        double newRange[2];
        m_volume->getScalarRange(newRange);

        // Scale transfer function before applying (only if the volume has at least 2 distinct values and the transfer function has at least 2 points)
        if (newRange[0] < newRange[1] && transferFunction.keys().size() > 1)
        {
            double originalRange[2] = { transferFunction.keys().first(), transferFunction.keys().last() };
            m_transferFunction = transferFunction.toNewRange(originalRange[0], originalRange[1], newRange[0], newRange[1]);
            m_transferFunction.setName(transferFunction.name());
        }
        else
        {
            m_transferFunction = transferFunction;
        }

        m_imagePipeline->setTransferFunction(m_transferFunction);
    }
}

const TransferFunction& VolumeDisplayUnit::getTransferFunction() const
{
    return m_transferFunction;
}

void VolumeDisplayUnit::clearTransferFunction()
{
    m_transferFunction.clear();
    m_imagePipeline->clearTransferFunction();
}

void VolumeDisplayUnit::setSlabProjectionMode(AccumulatorFactory::AccumulatorType accumulatorType)
{
    m_imagePipeline->setSlabProjectionMode(accumulatorType);
}

void VolumeDisplayUnit::setShutterData(vtkImageData *shutterData)
{
    m_imagePipeline->setShutterData(shutterData);
}

}
