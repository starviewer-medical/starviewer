#ifndef VOLUMEDISPLAYUNIT_H
#define VOLUMEDISPLAYUNIT_H

class vtkImageActor;

#include "orthogonalplane.h"

namespace udg {

class Image;
class ImagePipeline;
class SliceHandler;
class Volume;

/**
    This class groups together a Volume and the associated objects that a Q2DViewer needs to display a volume.
 */
class VolumeDisplayUnit {

public:
    VolumeDisplayUnit();
    ~VolumeDisplayUnit();

    Volume* getVolume() const;
    void setVolume(Volume *volume);
    ImagePipeline* getImagePipeline() const;
    vtkImageActor* getImageActor() const;
    SliceHandler* getSliceHandler() const;
    OrthogonalPlane::OrthogonalPlaneType getViewPlane() const;
    void setViewPlane(OrthogonalPlane::OrthogonalPlaneType viewPlane);

    /// Returns slice thickness of the currently displayed image.
    /// On the acquisition plane, this depends on DICOM's slice thickness and slab thickness (if DICOM's slice thickness is not defined, the method returns 0).
    /// On the other planes, this depends on the spacing and the slab thickness.
    double getSliceThickness() const;

private:
    Volume *m_volume;
    ImagePipeline *m_imagePipeline;
    vtkImageActor *m_imageActor;
    SliceHandler *m_sliceHandler;
    OrthogonalPlane::OrthogonalPlaneType m_viewPlane;

};

}

#endif
