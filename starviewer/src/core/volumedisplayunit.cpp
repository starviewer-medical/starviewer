#include "volumedisplayunit.h"

#include "imagepipeline.h"
#include "slicehandler.h"
#include "volume.h"

#include <vtkImageActor.h>

namespace udg {

VolumeDisplayUnit::VolumeDisplayUnit() :
    m_volume(0)
{
    m_imagePipeline = new ImagePipeline();
    m_imageActor = vtkImageActor::New();
    m_sliceHandler = new SliceHandler();
}

VolumeDisplayUnit::~VolumeDisplayUnit()
{
    delete m_imagePipeline;
    m_imageActor->Delete();
    delete m_sliceHandler;
}

Volume* VolumeDisplayUnit::getVolume() const
{
    return m_volume;
}

void VolumeDisplayUnit::setVolume(Volume *volume)
{
    m_volume = volume;
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

}
