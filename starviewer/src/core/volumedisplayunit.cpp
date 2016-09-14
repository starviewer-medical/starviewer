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
#include "voiluthelper.h"

#include <vtkCamera.h>
#include <vtkImageResliceMapper.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
#include <vtkImageStack.h>
#include <vtkPropPicker.h>
#include <vtkImageProperty.h>

namespace udg {

VolumeDisplayUnit::VolumeDisplayUnit()
 : m_volume(nullptr), m_shutterImageSlice(nullptr)
{
    m_imagePipeline = new ImagePipeline();
    m_imageSlice = vtkImageSlice::New();
    vtkImageResliceMapper *mapper = vtkImageResliceMapper::New();
    mapper->SliceAtFocalPointOn();
    mapper->SliceFacesCameraOn();
    mapper->JumpToNearestSliceOn();
    mapper->StreamingOn();
    m_imageSlice->SetMapper(mapper);
    mapper->Delete();
    m_imageSlice->GetProperty()->SetInterpolationTypeToCubic();
    m_imageStack = vtkImageStack::New();
    m_imageStack->AddImage(m_imageSlice);
    m_sliceHandler = new SliceHandler();
    m_imagePointPicker =  0;
    m_voiLutData = 0;
    m_currentThickSlabPixelData = 0;
}

VolumeDisplayUnit::~VolumeDisplayUnit()
{
    delete m_imagePipeline;
    m_imageSlice->Delete();
    m_imageStack->Delete();

    if (m_shutterImageSlice)
    {
        m_shutterImageSlice->Delete();
    }

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

    m_imageSlice->GetMapper()->SetInputConnection(m_imagePipeline->getOutput().getVtkAlgorithmOutput());
}

void VolumeDisplayUnit::setVoiLutData(VoiLutPresetsToolData *voiLutData)
{
    m_voiLutData = voiLutData;
    VoiLutHelper().initializeVoiLutData(m_voiLutData, m_volume);
}

VoiLutPresetsToolData *VolumeDisplayUnit::getVoiLutData() const
{
    return m_voiLutData;
}

ImagePipeline* VolumeDisplayUnit::getImagePipeline() const
{
    return m_imagePipeline;
}

vtkImageSlice* VolumeDisplayUnit::getImageSlice() const
{
    return m_imageSlice;
}

vtkImageStack* VolumeDisplayUnit::getImageStack() const
{
    return m_imageStack;
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

void VolumeDisplayUnit::updateImageSlice(vtkCamera *camera)
{
    if (!m_volume || !m_volume->isPixelDataLoaded())
    {
        return;
    }

    int imageIndex = m_volume->getImageIndex(m_sliceHandler->getCurrentSlice(), m_sliceHandler->getCurrentPhase());
    int zIndex = this->getViewPlane().getZIndex();
    double origin[3];
    m_volume->getOrigin(origin);
    double spacing[3];
    m_volume->getSpacing(spacing);
    double focalPoint[3];
    camera->GetFocalPoint(focalPoint);
    focalPoint[zIndex] = origin[zIndex] + imageIndex * spacing[zIndex];
    camera->SetFocalPoint(focalPoint);
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
    m_imagePointPicker->AddPickList(getImageSlice());
    m_imagePointPicker->PickFromListOn();
}

void VolumeDisplayUnit::updateCurrentImageDefaultPresets()
{
    if (getViewPlane() == OrthogonalPlane::XYPlane)
    {
        Image *image = getVolume()->getImage(m_sliceHandler->getCurrentSlice(), m_sliceHandler->getCurrentPhase());
        if (image)
        {
            for (int i = 0; i < image->getNumberOfVoiLuts(); ++i)
            {
                VoiLut voiLut = VoiLutHelper().getDefaultVoiLutForPresentation(image, i);
                m_voiLutData->updatePreset(voiLut);
            }
        }
    }
    
    VoiLut voiLut = m_voiLutData->getCurrentPreset();
    setVoiLut(voiLut);
}

void VolumeDisplayUnit::setVoiLut(const VoiLut &voiLut)
{
    if (m_volume->getImage(0) && m_volume->getImage(0)->getPhotometricInterpretation() == PhotometricInterpretation::Monochrome1)
    {
        m_imagePipeline->setVoiLut(voiLut.inverse());
    }
    else
    {
        m_imagePipeline->setVoiLut(voiLut);
    }
}

void VolumeDisplayUnit::setCurrentVoiLutPreset(const VoiLut &voiLut)
{
    m_voiLutData->setCurrentPreset(voiLut);
    setVoiLut(voiLut);
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
    if (shutterData)
    {
        if (!m_shutterImageSlice)
        {
            m_shutterImageSlice = vtkImageSlice::New();
            m_shutterImageSlice->GetProperty()->SetLayerNumber(1);
            auto *mapper = vtkImageSliceMapper::New();
            m_shutterImageSlice->SetMapper(mapper);
            mapper->Delete();
        }

        m_shutterImageSlice->GetMapper()->SetInputData(shutterData);
        m_imageStack->AddImage(m_shutterImageSlice);
    }
    else
    {
        m_imageStack->RemoveImage(m_shutterImageSlice);
    }
}

bool VolumeDisplayUnit::isVisible() const
{
    return m_imageStack->GetVisibility() || m_imageSlice->GetVisibility() || (m_shutterImageSlice && m_shutterImageSlice->GetVisibility());
}

void VolumeDisplayUnit::setVisible(bool visible)
{
    m_imageStack->SetVisibility(visible);
    m_imageSlice->SetVisibility(visible);
    if (m_shutterImageSlice)
    {
        m_shutterImageSlice->SetVisibility(visible);
    }
}

void VolumeDisplayUnit::setOpacity(double opacity)
{
    m_imageSlice->GetProperty()->SetOpacity(opacity);
    if (m_shutterImageSlice)
    {
        m_shutterImageSlice->GetProperty()->SetOpacity(opacity);
    }
}

}
