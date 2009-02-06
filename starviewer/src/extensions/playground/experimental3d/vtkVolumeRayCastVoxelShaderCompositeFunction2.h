#ifndef UDGVTKVOLUMERAYCASTVOXELSHADERCOMPOSITEFUNCTION2_H
#define UDGVTKVOLUMERAYCASTVOXELSHADERCOMPOSITEFUNCTION2_H


#include <vtkVolumeRayCastFunction.h>


#include <QList>


namespace udg {


class TrilinearInterpolator;
class VoxelShader2;


/**
 * Classe que fa un ray casting permetent aplicar un voxel shader per decidir el color de cada vòxel.
 */
class vtkVolumeRayCastVoxelShaderCompositeFunction2 : public vtkVolumeRayCastFunction {

public:

    enum CompositeMethod { ClassifyInterpolate, InterpolateClassify };

    static vtkVolumeRayCastVoxelShaderCompositeFunction2* New();
    vtkTypeRevisionMacro( vtkVolumeRayCastVoxelShaderCompositeFunction2, vtkVolumeRayCastFunction );
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

    void AddVoxelShader( VoxelShader2 * voxelShader );
    void InsertVoxelShader( int i, VoxelShader2 * voxelShader );
    int IndexOfVoxelShader( VoxelShader2 * voxelShader );
    void RemoveVoxelShader( int i );
    void RemoveVoxelShader( VoxelShader2 * voxelShader );
    void RemoveAllVoxelShaders();

protected:

    vtkVolumeRayCastVoxelShaderCompositeFunction2();
    ~vtkVolumeRayCastVoxelShaderCompositeFunction2();

    //BTX
    void SpecificFunctionInitialize( vtkRenderer *renderer, vtkVolume *volume,
                                     vtkVolumeRayCastStaticInfo *staticInfo, vtkVolumeRayCastMapper *mapper );
    //ETX

    template <class T>
    void CastRay( const T *data, vtkVolumeRayCastDynamicInfo *dynamicInfo, const vtkVolumeRayCastStaticInfo *staticInfo ) const;

    CompositeMethod m_compositeMethod;
    QList<VoxelShader2*> m_voxelShaderList;
    TrilinearInterpolator *m_interpolator;

private:

    static const float REMAINING_OPACITY;

    vtkVolumeRayCastVoxelShaderCompositeFunction2( const vtkVolumeRayCastVoxelShaderCompositeFunction2& );  // Not implemented.
    void operator=( const vtkVolumeRayCastVoxelShaderCompositeFunction2& );                       // Not implemented.

};

}


#endif
