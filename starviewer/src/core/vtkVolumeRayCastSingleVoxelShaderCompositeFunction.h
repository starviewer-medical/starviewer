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

#ifndef UDGVTKVOLUMERAYCASTSINGLEVOXELSHADERCOMPOSITEFUNCTION_H
#define UDGVTKVOLUMERAYCASTSINGLEVOXELSHADERCOMPOSITEFUNCTION_H

#include <vtkVolumeRayCastFunction.h>

namespace udg {

class TrilinearInterpolator;

/**
 * Classe que fa un ray casting permetent aplicar un voxel shader per decidir el color de cada vòxel. El tipus del voxel shader és un paràmetre de template per
 * evitar cridar mètodes virtuals.
 */
template <class VS>
class vtkVolumeRayCastSingleVoxelShaderCompositeFunction : public vtkVolumeRayCastFunction {

public:
    enum CompositeMethod { ClassifyInterpolate, InterpolateClassify };

    static vtkVolumeRayCastSingleVoxelShaderCompositeFunction* New();
    vtkTypeMacro(vtkVolumeRayCastSingleVoxelShaderCompositeFunction, vtkVolumeRayCastFunction)
    void PrintSelf(ostream &os, vtkIndent indent);

    void SetCompositeMethod(CompositeMethod compositeMethod) { m_compositeMethod = qBound(ClassifyInterpolate, compositeMethod, InterpolateClassify); }
    CompositeMethod GetCompositeMethod() const { return m_compositeMethod; }
    void SetCompositeMethodToInterpolateFirst() { m_compositeMethod = InterpolateClassify; }
    void SetCompositeMethodToClassifyFirst() { m_compositeMethod = ClassifyInterpolate; }
    const char* GetCompositeMethodAsString() const;

    //BTX
    void CastRay(vtkVolumeRayCastDynamicInfo *dynamicInfo, vtkVolumeRayCastStaticInfo *staticInfo);

    float GetZeroOpacityThreshold(vtkVolume *volume);
    //ETX

    void SetVoxelShader(VS *voxelShader);

protected:
    vtkVolumeRayCastSingleVoxelShaderCompositeFunction();
    ~vtkVolumeRayCastSingleVoxelShaderCompositeFunction();

    //BTX
    void SpecificFunctionInitialize(vtkRenderer *renderer, vtkVolume *volume, vtkVolumeRayCastStaticInfo *staticInfo, vtkVolumeRayCastMapper *mapper);
    //ETX

    CompositeMethod m_compositeMethod;
    VS *m_voxelShader;
    TrilinearInterpolator *m_interpolator;

private:
    /// Opacitat mínima que ha de restar per continuar el ray casting.
    static const float MINIMUM_REMAINING_OPACITY;

    vtkVolumeRayCastSingleVoxelShaderCompositeFunction(const vtkVolumeRayCastSingleVoxelShaderCompositeFunction&);    // Not implemented.
    void operator=(const vtkVolumeRayCastSingleVoxelShaderCompositeFunction&);                                        // Not implemented.

};

}

#include "vtkVolumeRayCastSingleVoxelShaderCompositeFunction.cxx"

#endif
