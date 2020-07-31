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

  This file incorporates work covered by the following copyright and
  permission notice:

    Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
    All rights reserved.
    See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

       This software is distributed WITHOUT ANY WARRANTY; without even
       the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
       PURPOSE.  See the above copyright notice for more information.
 *************************************************************************************/

#include "stipplingvtkopenglpolydatamapper2d.h"

#include <vtkActor2D.h>
#include <vtkObjectFactory.h>
#include <vtkOpenGLVertexBufferObjectGroup.h>
#include <vtkProperty2D.h>
#include <vtkShaderProgram.h>
#include <vtkUnsignedCharArray.h>
#include <vtkViewport.h>

namespace {

constexpr char const *FragmentTCoordDec = R"(
//VTK::TCoord::Dec
noperspective in float stippleCoord;
uniform int StipplePattern;
)";

constexpr char const *FragmentTCoordImpl = R"(
//VTK::TCoord::Impl
int stip = 0x1 & (StipplePattern >> int(fract(stippleCoord)*16.0));
if (stip == 0)
    discard;
)";

constexpr char const *GeometryPositionVCDec = R"(
//VTK::PositionVC::Dec
uniform vec2 ViewportSize;
)";

constexpr char const *GeometryOutputDec = R"(
//VTK::Output::Dec
noperspective out float stippleCoord;
)";

constexpr char const *GeometryPositionVCImpl = R"(
//VTK::PositionVC::Impl
if (i == 0) {
    stippleCoord = 0.0;
}
else {
    vec4 pos0 = gl_in[0].gl_Position;
    vec2 p0 = pos0.xy / pos0.w * ViewportSize;
    vec4 pos1 = gl_in[1].gl_Position;
    vec2 p1 = pos1.xy / pos1.w * ViewportSize;
    float len = length(p0.xy - p1.xy);
    stippleCoord = len / 32;
}
)";

constexpr char const *BasicGeometryShader = R"(
//VTK::System::Dec

layout(lines) in;
layout(line_strip, max_vertices=2) out;

//VTK::PositionVC::Dec
//VTK::PrimID::Dec
//VTK::Color::Dec
//VTK::Normal::Dec
//VTK::Light::Dec
//VTK::TCoord::Dec
//VTK::Picking::Dec
//VTK::DepthPeeling::Dec
//VTK::Clip::Dec
//VTK::Output::Dec

void main()
{
    //VTK::Normal::Start
    for (int i = 0; i < 2; i++) {
        //VTK::PrimID::Impl
        //VTK::Clip::Impl
        //VTK::Color::Impl
        //VTK::Normal::Impl
        //VTK::Light::Impl
        //VTK::TCoord::Impl
        //VTK::DepthPeeling::Impl
        //VTK::Picking::Impl
        //VTK::PositionVC::Impl
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
}
)";

}

namespace udg {

vtkStandardNewMacro(StipplingVtkOpenGLPolyDataMapper2D)

void StipplingVtkOpenGLPolyDataMapper2D::PrintSelf(std::ostream &os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
}

void StipplingVtkOpenGLPolyDataMapper2D::BuildShaders(std::string &vertexCode, std::string &fragmentCode, std::string &geometryCode, vtkViewport *renderer,
                                                      vtkActor2D *actor)
{
    this->Superclass::BuildShaders(vertexCode, fragmentCode, geometryCode, renderer, actor);

    if (actor->GetProperty()->GetLineStipplePattern() != 0xFFFF)
    {
        vtkShaderProgram::Substitute(fragmentCode, "//VTK::TCoord::Dec", FragmentTCoordDec, false);
        vtkShaderProgram::Substitute(fragmentCode, "//VTK::TCoord::Impl", FragmentTCoordImpl, false);

        if (geometryCode.empty())
        {
            geometryCode = BasicGeometryShader;

            // Repeat substitutions that the superclass does if it uses a geometry shader
            if (!this->HaveCellScalars)
            {
                if (this->Colors && this->Colors->GetNumberOfComponents())
                    {
                        vtkShaderProgram::Substitute(geometryCode, "//VTK::Color::Dec",
                                                     "in vec4 fcolorVSOutput[];\n"
                                                     "out vec4 fcolorGSOutput;");
                        vtkShaderProgram::Substitute(geometryCode, "//VTK::Color::Impl",
                                                     "fcolorGSOutput = fcolorVSOutput[i];");
                    }
            }

            int numTCoordComps = this->VBOs->GetNumberOfComponents("tcoordMC");
            if (numTCoordComps == 1 || numTCoordComps == 2)
            {
                if (numTCoordComps == 1)
                {
                    vtkShaderProgram::Substitute(geometryCode, "//VTK::TCoord::Dec",
                                                 "in float tcoordVCVSOutput[];\n"
                                                 "out float tcoordVCGSOutput;");
                    vtkShaderProgram::Substitute(geometryCode, "//VTK::TCoord::Impl",
                                                 "tcoordVCGSOutput = tcoordVCVSOutput[i];");
                }
                else
                {
                    vtkShaderProgram::Substitute(geometryCode, "//VTK::TCoord::Dec",
                                                 "in vec2 tcoordVCVSOutput[];\n"
                                                 "out vec2 tcoordVCGSOutput;");
                    vtkShaderProgram::Substitute(geometryCode, "//VTK::TCoord::Impl",
                                                 "tcoordVCGSOutput = tcoordVCVSOutput[i];");
                }
            }

            // are we handling the apple bug?
            if (!this->AppleBugPrimIDs.empty())
            {
                vtkShaderProgram::Substitute(geometryCode, "//VTK::PrimID::Dec",
                                             "in  vec4 applePrimIDVSOutput[];\n"
                                             "out vec4 applePrimIDGSOutput;");
                vtkShaderProgram::Substitute(geometryCode, "//VTK::PrimID::Impl",
                                             "applePrimIDGSOutput = applePrimIDVSOutput[i];");
            }
            else
            {
                if (this->HaveCellScalars)
                {
                    vtkShaderProgram::Substitute(geometryCode, "//VTK::PrimID::Impl",
                                                 "gl_PrimitiveID = gl_PrimitiveIDIn;");
                }
            }
        }

        vtkShaderProgram::Substitute(geometryCode, "//VTK::PositionVC::Dec", GeometryPositionVCDec, false);
        vtkShaderProgram::Substitute(geometryCode, "//VTK::Output::Dec", GeometryOutputDec, false);
        vtkShaderProgram::Substitute(geometryCode, "//VTK::PositionVC::Impl", GeometryPositionVCImpl, false);
    }
}

void StipplingVtkOpenGLPolyDataMapper2D::SetMapperShaderParameters(vtkOpenGLHelper &cellBO, vtkViewport *renderer, vtkActor2D *actor)
{
    this->Superclass::SetMapperShaderParameters(cellBO, renderer, actor);

    int stipplePattern = actor->GetProperty()->GetLineStipplePattern();

    if (stipplePattern != 0xFFFF)
    {
        vtkShaderProgram *program = cellBO.Program;

        // The proper place to do this would be SetPropertyShaderParameters, but it is not virtual
        program->SetUniformi("StipplePattern", stipplePattern);

        int *size = renderer->GetSize();
        float viewportSize[2] = { static_cast<float>(size[0]), static_cast<float>(size[1]) };
        program->SetUniform2f("ViewportSize", viewportSize);
    }
}

} // namespace udg
