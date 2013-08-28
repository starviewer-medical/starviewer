#include "volumedisplayunit.h"

#include "image.h"
#include "imagepipeline.h"
#include "slicehandler.h"
#include "volume.h"

#include <vtkImageActor.h>

namespace udg {

VolumeDisplayUnit::VolumeDisplayUnit() :
    m_volume(0), m_viewPlane(OrthogonalPlane::XYPlane)
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

    m_sliceHandler->setSlice(0);
    m_sliceHandler->setPhase(0);

    m_viewPlane = OrthogonalPlane::XYPlane;

    m_sliceHandler->setNumberOfPhases(m_volume->getNumberOfPhases());
    m_sliceHandler->setSliceRange(m_volume->getMinimumSlice(m_viewPlane), m_volume->getMaximumSlice(m_viewPlane));

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

const OrthogonalPlane& VolumeDisplayUnit::getViewPlane() const
{
    return m_viewPlane;
}

void VolumeDisplayUnit::setViewPlane(const OrthogonalPlane &viewPlane)
{
    m_viewPlane = viewPlane;
}

double VolumeDisplayUnit::getSliceThickness() const
{
    double thickness = 0.0;

    switch (m_viewPlane)
    {
        case OrthogonalPlane::XYPlane:
            {
                Image *image = m_volume->getImage(m_sliceHandler->getCurrentSlice(), m_sliceHandler->getCurrentPhase());

                if (image)
                {
                    thickness = image->getSliceThickness();

                    if (m_sliceHandler->getSlabThickness() > 1)
                    {
                        double gap = m_volume->getSpacing()[2] - thickness;

                        if (gap < 0)
                        {
                            // If gap between spacing and thickness is negative, this means slices overlap, so
                            // we have to substract this gap between to get the real thickness
                            thickness = (thickness + gap) * m_sliceHandler->getSlabThickness();
                        }
                        else
                        {
                            thickness = thickness * m_sliceHandler->getSlabThickness();
                        }
                    }
                }
            }
            break;

        case OrthogonalPlane::YZPlane:
            thickness = m_volume->getSpacing()[0] * m_sliceHandler->getSlabThickness();
            break;

        case OrthogonalPlane::XZPlane:
            thickness = m_volume->getSpacing()[1] * m_sliceHandler->getSlabThickness();
            break;
    }

    return thickness;
}

}
