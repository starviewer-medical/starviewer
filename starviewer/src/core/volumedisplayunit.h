#ifndef VOLUMEDISPLAYUNIT_H
#define VOLUMEDISPLAYUNIT_H

class vtkImageActor;

namespace udg {

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

private:
    Volume *m_volume;
    ImagePipeline *m_imagePipeline;
    vtkImageActor *m_imageActor;
    SliceHandler *m_sliceHandler;

};

}

#endif
