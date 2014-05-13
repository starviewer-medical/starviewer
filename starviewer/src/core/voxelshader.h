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

#ifndef UDGVOXELSHADER_H
#define UDGVOXELSHADER_H

#include "hdrcolor.h"
#include "vector3.h"

namespace udg {

class TrilinearInterpolator;

/**
    Aquesta classe implementa els mètodes per retornar el color d'un vòxel. El mètode shade ha de ser implementat per les classes filles.
  */
class VoxelShader {

public:
    VoxelShader();
    virtual ~VoxelShader();

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual HdrColor shade(const Vector3 &position, int offset, const Vector3 &direction, float remainingOpacity, const HdrColor &baseColor = HdrColor()) = 0;
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    virtual HdrColor shade(const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, float remainingOpacity,
                            const HdrColor &baseColor = HdrColor()) = 0;
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

};

}

#endif
