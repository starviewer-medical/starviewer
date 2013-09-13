#include "volumedisplayunit.h"

#include "imagepipeline.h"
#include "slicehandler.h"
#include "volume.h"
#include "vtkdepthdisabledopenglimageactor.h"

#include <vtkPropPicker.h>

namespace udg {

VolumeDisplayUnit::VolumeDisplayUnit()
 : m_volume(0)
{
    m_imagePipeline = new ImagePipeline();
    m_imageActor = VtkDepthDisabledOpenGLImageActor::New();
    m_sliceHandler = new SliceHandler();
    m_imagePointPicker =  0;
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

ImagePipeline* VolumeDisplayUnit::getImagePipeline() const
{
    return m_imagePipeline;
}

vtkImageActor* VolumeDisplayUnit::getImageActor() const
{
    return m_imageActor;
}

SliceHandler* VolumeDisplayUnit::getSliceHandler() const
{
    return m_sliceHandler;
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

}
