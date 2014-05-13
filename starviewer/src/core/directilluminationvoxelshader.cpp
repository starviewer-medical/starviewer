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

#include "directilluminationvoxelshader.h"

namespace udg {

DirectIlluminationVoxelShader::DirectIlluminationVoxelShader()
 : AmbientVoxelShader()
{
    m_encodedNormals = 0;
    m_redDiffuseShadingTable = m_greenDiffuseShadingTable = m_blueDiffuseShadingTable = 0;
    m_redSpecularShadingTable = m_greenSpecularShadingTable = m_blueSpecularShadingTable = 0;
}

DirectIlluminationVoxelShader::~DirectIlluminationVoxelShader()
{
}

void DirectIlluminationVoxelShader::setEncodedNormals(const unsigned short *encodedNormals)
{
    m_encodedNormals = encodedNormals;
}

void DirectIlluminationVoxelShader::setDiffuseShadingTables(const float *red, const float *green, const float *blue)
{
    m_redDiffuseShadingTable = red;
    m_greenDiffuseShadingTable = green;
    m_blueDiffuseShadingTable = blue;
}

void DirectIlluminationVoxelShader::setSpecularShadingTables(const float *red, const float *green, const float *blue)
{
    m_redSpecularShadingTable = red;
    m_greenSpecularShadingTable = green;
    m_blueSpecularShadingTable = blue;
}

QString DirectIlluminationVoxelShader::toString() const
{
    return "DirectIlluminationVoxelShader";
}

}
