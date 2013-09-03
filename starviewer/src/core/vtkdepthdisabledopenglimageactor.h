#ifndef VTKDEPTHDISABLEDOPENGLIMAGEACTOR_H
#define VTKDEPTHDISABLEDOPENGLIMAGEACTOR_H

#include <vtkOpenGLImageActor.h>

namespace udg {

/**
    Subclass of vtkOpenGLImageActor that is rendered with the depth test disabled.
    Its purpose is to be able to obtain correct image fusion with 2 or more actors independently of their z camera-space coordinate.
 */
class VtkDepthDisabledOpenGLImageActor : public vtkOpenGLImageActor {

public:
    vtkTypeMacro(VtkDepthDisabledOpenGLImageActor, vtkOpenGLImageActor);

    /// Instantiate the image actor.
    static VtkDepthDisabledOpenGLImageActor* New();

    /// Renders the image actor with the depth test disabled.
    virtual void Render(vtkRenderer *renderer);

protected:
    VtkDepthDisabledOpenGLImageActor();

private:
    VtkDepthDisabledOpenGLImageActor(const VtkDepthDisabledOpenGLImageActor&);  // Not implemented
    void operator=(const VtkDepthDisabledOpenGLImageActor&);                    // Not implemented

};

}

#endif
