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

#ifndef UDG_STIPPLINGVTKOPENGLPOLYDATAMAPPER2D_H
#define UDG_STIPPLINGVTKOPENGLPOLYDATAMAPPER2D_H

#include <vtkOpenGLPolyDataMapper2D.h>

namespace udg {

/**
 * @brief The StipplingVtkOpenGLPolyDataMapper2D class is a vtkOpenGLPolyDataMapper2D subclass that implements stippled lines via GLSL shaders.
 */
class StipplingVtkOpenGLPolyDataMapper2D : public vtkOpenGLPolyDataMapper2D
{
public:
    vtkTypeMacro(StipplingVtkOpenGLPolyDataMapper2D, vtkOpenGLPolyDataMapper2D)
    static StipplingVtkOpenGLPolyDataMapper2D *New();

    void PrintSelf(std::ostream &os, vtkIndent indent) override;

protected:
    StipplingVtkOpenGLPolyDataMapper2D() = default;
    ~StipplingVtkOpenGLPolyDataMapper2D() override = default;

    /// Sets the necessary code for each shader in the first three parameters, that are output parameters.
    void BuildShaders(std::string &vertexCode, std::string &fragmentCode, std::string &geometryCode, vtkViewport *renderer, vtkActor2D *actor) override;
    /// Passes the required uniforms to the shaders.
    void SetMapperShaderParameters(vtkOpenGLHelper &cellBO, vtkViewport *renderer, vtkActor2D *actor) override;

private:
    StipplingVtkOpenGLPolyDataMapper2D(const StipplingVtkOpenGLPolyDataMapper2D&) = delete;
    void operator=(const StipplingVtkOpenGLPolyDataMapper2D&) = delete;

};

} // namespace udg

#endif // UDG_STIPPLINGVTKOPENGLPOLYDATAMAPPER2D_H
