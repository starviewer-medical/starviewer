#include "volumedisplayunit.h"

#include "imagepipeline.h"
#include "slicehandler.h"
#include "volume.h"
#include "vtkdepthdisabledopenglimageactor.h"
#include "windowlevelpresetstooldata.h"
#include "volumepixeldata.h"
#include "image.h"
#include "windowlevelhelper.h"

#include <vtkPropPicker.h>

namespace udg {

VolumeDisplayUnit::VolumeDisplayUnit()
 : m_volume(0)
{
    m_imagePipeline = new ImagePipeline();
    m_imageActor = VtkDepthDisabledOpenGLImageActor::New();
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

    m_imageActor->SetInput(m_imagePipeline->getOutput().getVtkImageData());
}

void VolumeDisplayUnit::setWindowLevelData(WindowLevelPresetsToolData *windowLevelData)
{
    m_windowLevelData = windowLevelData;
    WindowLevelHelper().initializeWindowLevelData(m_windowLevelData, m_volume);
}

WindowLevelPresetsToolData *VolumeDisplayUnit::getWindowLevelData()
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

VolumePixelData* VolumeDisplayUnit::getCurrentPixelData()
{
    if (!m_volume)
    {
        return 0;
    }
    
    // TODO We should have a method kind of "isThickSlabActive()"
    if (getSlabThickness() > 1)
    {
        if (!m_currentThickSlabPixelData)
        {
            m_currentThickSlabPixelData = new VolumePixelData;
        }
        m_currentThickSlabPixelData->setData(getImagePipeline()->getSlabProjectionOutput());

        return m_currentThickSlabPixelData;
    }
    else
    {
        return m_volume->getPixelData();
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
    m_volume->getWholeExtent(displayExtent);
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
    m_sliceHandler->setSlabThickness(thickness);
    m_imagePipeline->setSlice(m_volume->getImageIndex(getSlice(), getPhase()));
    m_imagePipeline->setSlabThickness(thickness);
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
    Image *image = getVolume()->getImage(m_sliceHandler->getCurrentSlice(), m_sliceHandler->getCurrentPhase());
    for (int i = 0; i < image->getNumberOfWindowLevels(); ++i)
    {
        WindowLevel windowLevel = WindowLevelHelper().getDefaultWindowLevelForPresentation(image, i);
        m_windowLevelData->updatePreset(windowLevel);
    }
    WindowLevel wl = m_windowLevelData->getCurrentPreset();
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
    m_imagePipeline->setTransferFunction(transferFunction);
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
