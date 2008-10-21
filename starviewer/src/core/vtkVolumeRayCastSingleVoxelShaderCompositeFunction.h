#ifndef UDGVTKVOLUMERAYCASTSINGLEVOXELSHADERCOMPOSITEFUNCTION_H
#define UDGVTKVOLUMERAYCASTSINGLEVOXELSHADERCOMPOSITEFUNCTION_H


#include <vtkVolumeRayCastFunction.h>


namespace udg {


class TrilinearInterpolator;
// class VoxelShader;


/**
 * Classe que fa un ray casting permetent aplicar un voxel shader per decidir el color de cada vòxel.
 */
template <class VS>
class vtkVolumeRayCastSingleVoxelShaderCompositeFunction : public vtkVolumeRayCastFunction {

public:

    enum CompositeMethod { ClassifyInterpolate, InterpolateClassify };

    static vtkVolumeRayCastSingleVoxelShaderCompositeFunction* New();
    vtkTypeRevisionMacro( vtkVolumeRayCastSingleVoxelShaderCompositeFunction, vtkVolumeRayCastFunction );
    void PrintSelf( ostream &os, vtkIndent indent );

    void SetCompositeMethod( CompositeMethod compositeMethod )
        { m_compositeMethod = qBound( ClassifyInterpolate, compositeMethod, InterpolateClassify); }
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
    void SpecificFunctionInitialize( vtkRenderer *renderer, vtkVolume *volume,
                                     vtkVolumeRayCastStaticInfo *staticInfo, vtkVolumeRayCastMapper *mapper );
    //ETX

    template <class T>
    void CastRay( const T *data, vtkVolumeRayCastDynamicInfo *dynamicInfo, const vtkVolumeRayCastStaticInfo *staticInfo ) const;

    CompositeMethod m_compositeMethod;
    VS *m_voxelShader;
    TrilinearInterpolator *m_interpolator;

private:

    static const float REMAINING_OPACITY;

    vtkVolumeRayCastSingleVoxelShaderCompositeFunction( const vtkVolumeRayCastSingleVoxelShaderCompositeFunction& );  // Not implemented.
    void operator=( const vtkVolumeRayCastSingleVoxelShaderCompositeFunction& );                       // Not implemented.

};


}


#include "vtkVolumeRayCastSingleVoxelShaderCompositeFunction.cxx"


#endif
