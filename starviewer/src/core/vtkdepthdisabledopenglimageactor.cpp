#include "vtkdepthdisabledopenglimageactor.h"

#include <QtOpenGL>

#include <vtkObjectFactory.h>

namespace udg {

vtkStandardNewMacro(VtkDepthDisabledOpenGLImageActor);

void VtkDepthDisabledOpenGLImageActor::Render(vtkRenderer *renderer)
{
    glPushAttrib(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    vtkOpenGLImageActor::Render(renderer);
    glPopAttrib();
}

VtkDepthDisabledOpenGLImageActor::VtkDepthDisabledOpenGLImageActor()
{
}

}
