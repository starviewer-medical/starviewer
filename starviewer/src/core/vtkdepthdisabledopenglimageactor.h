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
