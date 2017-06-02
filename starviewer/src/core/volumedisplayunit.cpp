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
#include "sliceorientedvolumepixeldata.h"
#include "volume.h"
#include "voilutpresetstooldata.h"
#include "volumepixeldata.h"
#include "image.h"
#include "voiluthelper.h"
#include "vtkimagereslicemapper2.h"

#include <vtkCamera.h>
#include <vtkImageProperty.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
#include <vtkImageStack.h>
#include <vtkLookupTable.h>
#include <vtkPropPicker.h>

namespace udg {

VolumeDisplayUnit::VolumeDisplayUnit(QObject *parent)
 : QObject(parent), m_volume(nullptr), m_shutterImageSlice(nullptr), m_auxiliarCurrentVolumePixelData(nullptr)
{
    m_imagePipeline = new ImagePipeline();
    m_imageSlice = vtkImageSlice::New();
    m_mapper = VtkImageResliceMapper2::New();
    m_mapper->SliceAtFocalPointOn();
    m_mapper->SliceFacesCameraOn();
    m_mapper->JumpToNearestSliceOn();
    m_mapper->StreamingOn();
    m_imageSlice->SetMapper(m_mapper);
    m_imageSlice->GetProperty()->SetInterpolationTypeToCubic();
    m_imageStack = vtkImageStack::New();
    m_imageStack->AddImage(m_imageSlice);
    m_sliceHandler = new SliceHandler();
    m_imagePointPicker =  0;
    m_voiLutData = 0;
}

VolumeDisplayUnit::~VolumeDisplayUnit()
{
    delete m_imagePipeline;
    m_imageSlice->Delete();
    m_mapper->Delete();
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
    delete m_auxiliarCurrentVolumePixelData;
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

SliceOrientedVolumePixelData VolumeDisplayUnit::getCurrentPixelData()
{
    if (!m_volume)
    {
        return SliceOrientedVolumePixelData();
    }
    
    if (isThickSlabActive())
    {
        if (!m_auxiliarCurrentVolumePixelData)
        {
            m_auxiliarCurrentVolumePixelData = new VolumePixelData();
        }

        m_mapper->ResampleToScreenPixelsOff();
        m_mapper->Update();
        m_auxiliarCurrentVolumePixelData->setData(m_mapper->getResliceOutput());

        return SliceOrientedVolumePixelData().setVolumePixelData(m_auxiliarCurrentVolumePixelData).setDataToWorldMatrix(m_mapper->getSliceToWorldMatrix());
    }
    else if (m_sliceHandler->getNumberOfPhases() > 1)
    {
        if (!m_auxiliarCurrentVolumePixelData)
        {
            m_auxiliarCurrentVolumePixelData = new VolumePixelData();
        }

        m_auxiliarCurrentVolumePixelData->setData(m_imagePipeline->getPhaseOutput());

        return SliceOrientedVolumePixelData().setVolumePixelData(m_auxiliarCurrentVolumePixelData).setOrthogonalPlane(getViewPlane());
    }
    else
    {
        return SliceOrientedVolumePixelData().setVolumePixelData(m_volume->getPixelData()).setOrthogonalPlane(getViewPlane());
    }
}

void VolumeDisplayUnit::restoreRenderingQuality()
{
    m_mapper->ResampleToScreenPixelsOn();
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

    int imageIndex = getSlice();
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
    m_imagePipeline->setPhase(getPhase());
}

int VolumeDisplayUnit::getNumberOfPhases() const
{
    return m_sliceHandler->getNumberOfPhases();
}

double VolumeDisplayUnit::getSlabThickness() const
{
    return m_mapper->GetSlabThickness();
}

void VolumeDisplayUnit::setSlabThickness(double thickness)
{
    m_mapper->SetSlabThickness(thickness);
    m_sliceHandler->setSlabThickness(thickness);
}

double VolumeDisplayUnit::getMaximumSlabThickness() const
{
    return m_sliceHandler->getMaximumSlabThickness();
}

bool VolumeDisplayUnit::isThickSlabActive() const
{
    return getSlabThickness() > 0.0;
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
        m_imagePipeline->setNumberOfPhases(getNumberOfPhases());
        m_mapper->SetSlabThickness(0.0);
        m_mapper->SetSlabTypeToMax();
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
        m_voiLut = voiLut.inverse();
    }
    else
    {
        m_voiLut = voiLut;
    }

    applyVoiLut();
}

void VolumeDisplayUnit::applyVoiLut()
{
    if (m_volume && m_volume->getNumberOfScalarComponents() == 3)
    {
        m_imagePipeline->enableColorMapping(true);
        m_imagePipeline->setVoiLut(m_voiLut);
    }
    else
    {
        m_imagePipeline->enableColorMapping(false);
        m_imageSlice->GetProperty()->SetColorWindow(qAbs(m_voiLut.getWindowLevel().getWidth()));
        m_imageSlice->GetProperty()->SetColorLevel(m_voiLut.getWindowLevel().getCenter());

        if (!m_transferFunction.isEmpty())
        {
            applyTransferFunction();
        }
        else if (m_voiLut.isWindowLevel())
        {
            vtkLookupTable *lut = m_voiLut.toVtkLookupTable();
            m_imageSlice->GetProperty()->SetLookupTable(lut);
            lut->Delete();
        }
        else
        {
            m_imageSlice->GetProperty()->SetLookupTable(m_voiLut.toVtkLookupTable());
        }
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
        m_transferFunction = transferFunction;
        m_imagePipeline->setTransferFunction(m_transferFunction);
        applyTransferFunction();
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
    applyVoiLut();  // we must reapply the current VOI LUT without transfer function
}

void VolumeDisplayUnit::applyTransferFunction()
{
    if (!(m_volume && m_volume->getNumberOfScalarComponents() == 3))
    {
        TransferFunction scaledTransferFunction;

        // Scale transfer function before applying (only if the volume has at least 2 distinct values and the transfer function has at least 2 points)
        if (m_transferFunction.keys().size() > 1)
        {
            double oldX1 = m_transferFunction.keys().first();
            double oldX2 = m_transferFunction.keys().last();
            double windowLevel = m_voiLut.getWindowLevel().getCenter();
            double windowWidth = m_voiLut.getWindowLevel().getWidth();

            if (qAbs(windowWidth) < 1)
            {
                // The minimum width is restricted to 1 (absolute value) to avoid glitches
                windowWidth = std::copysign(1.0, windowWidth);
            }

            if (m_voiLut.isLut())
            {
                auto lut = m_voiLut.getLut();
                int leftValue = lut.getColor(lut.keys().first()).value();
                int rightValue = lut.getColor(lut.keys().last()).value();

                // If the LUT goes from light to dark, invert the window width to invert the transfer function
                if (leftValue > rightValue)
                {
                    windowWidth = -windowWidth;
                }
            }

            double newX1 = windowLevel - windowWidth / 2.0;
            double newX2 = windowLevel + windowWidth / 2.0;
            scaledTransferFunction = m_transferFunction.toNewRange(oldX1, oldX2, newX1, newX2);
            scaledTransferFunction.setName(m_transferFunction.name());
        }
        else
        {
            scaledTransferFunction = m_transferFunction;
        }

        m_imageSlice->GetProperty()->SetLookupTable(scaledTransferFunction.toVtkLookupTable());
    }
}

void VolumeDisplayUnit::setSlabProjectionMode(SlabProjectionMode mode)
{
    m_mapper->SetSlabType(mode);
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

        shutterData->SetSpacing(m_volume->getSpacing());
        m_shutterImageSlice->GetMapper()->SetInputData(shutterData);

        if (!m_imageStack->HasImage(m_shutterImageSlice))
        {
            m_imageStack->AddImage(m_shutterImageSlice);
            emit imageStackChanged();
        }
    }
    else
    {
        if (m_imageStack->HasImage(m_shutterImageSlice))
        {
            m_imageStack->RemoveImage(m_shutterImageSlice);
            emit imageStackChanged();
        }
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
