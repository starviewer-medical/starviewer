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
    vtkTypeRevisionMacro( vtkVolumeRayCastSingleVoxelShaderCompositeFunction, vtkVolumeRayCastFunction );
    void PrintSelf( ostream &os, vtkIndent indent );

    void SetCompositeMethod( CompositeMethod compositeMethod ) { m_compositeMethod = qBound( ClassifyInterpolate, compositeMethod, InterpolateClassify); }
    CompositeMethod GetCompositeMethod() const { return m_compositeMethod; }
    void SetCompositeMethodToInterpolateFirst() { m_compositeMethod = InterpolateClassify; }
    void SetCompositeMethodToClassifyFirst() { m_compositeMethod = ClassifyInterpolate; }
    const char* GetCompositeMethodAsString() const;

    //BTX
    void CastRay( vtkVolumeRayCastDynamicInfo *dynamicInfo, vtkVolumeRayCastStaticInfo *staticInfo );

    float GetZeroOpacityThreshold( vtkVolume *volume );
    //ETX

    void SetVoxelShader( VS *voxelShader );

protected:

    vtkVolumeRayCastSingleVoxelShaderCompositeFunction();
    ~vtkVolumeRayCastSingleVoxelShaderCompositeFunction();

    //BTX
    void SpecificFunctionInitialize( vtkRenderer *renderer, vtkVolume *volume, vtkVolumeRayCastStaticInfo *staticInfo, vtkVolumeRayCastMapper *mapper );
    //ETX

    CompositeMethod m_compositeMethod;
    VS *m_voxelShader;
    TrilinearInterpolator *m_interpolator;

private:

    /// Opacitat mínima que ha de restar per continuar el ray casting.
    static const float MINIMUM_REMAINING_OPACITY;

    vtkVolumeRayCastSingleVoxelShaderCompositeFunction( const vtkVolumeRayCastSingleVoxelShaderCompositeFunction& );    // Not implemented.
    void operator=( const vtkVolumeRayCastSingleVoxelShaderCompositeFunction& );                                        // Not implemented.

};


}


#include "vtkVolumeRayCastSingleVoxelShaderCompositeFunction.cxx"


#endif
