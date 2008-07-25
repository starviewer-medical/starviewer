#ifndef UDGVTKVOLUMERAYCASTCOMPOSITEFXFUNCTION_H
#define UDGVTKVOLUMERAYCASTCOMPOSITEFXFUNCTION_H


#include <vtkVolumeRayCastFunction.h>


#include <QList>


namespace udg {


class TrilinearInterpolator;
class VoxelShader;


/**
 * Classe que fa un ray casting permetent aplicar un voxel shader per decidir el color de cada vòxel.
 */
class vtkVolumeRayCastCompositeFxFunction : public vtkVolumeRayCastFunction {

public:

    enum CompositeMethod { ClassifyInterpolate, InterpolateClassify };

    static vtkVolumeRayCastCompositeFxFunction* New();
    vtkTypeRevisionMacro( vtkVolumeRayCastCompositeFxFunction, vtkVolumeRayCastFunction );
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

    void AddVoxelShader( VoxelShader * voxelShader );
    void InsertVoxelShader( int i, VoxelShader * voxelShader );
    int IndexOfVoxelShader( VoxelShader * voxelShader );
    void RemoveVoxelShader( int i );
    void RemoveVoxelShader( VoxelShader * voxelShader );
    void RemoveAllVoxelShaders();

protected:

    vtkVolumeRayCastCompositeFxFunction();
    ~vtkVolumeRayCastCompositeFxFunction();

    //BTX
    void SpecificFunctionInitialize( vtkRenderer *renderer, vtkVolume *volume,
                                     vtkVolumeRayCastStaticInfo *staticInfo, vtkVolumeRayCastMapper *mapper );
    //ETX

    template <class T>
    void CastRay( const T *data, vtkVolumeRayCastDynamicInfo *dynamicInfo, const vtkVolumeRayCastStaticInfo *staticInfo ) const;

    CompositeMethod m_compositeMethod;
    QList<VoxelShader*> m_voxelShaderList;
    TrilinearInterpolator *m_interpolator;

private:

    static const float REMAINING_OPACITY;

    vtkVolumeRayCastCompositeFxFunction( const vtkVolumeRayCastCompositeFxFunction& );  // Not implemented.
    void operator=( const vtkVolumeRayCastCompositeFxFunction& );                       // Not implemented.

};

}


#endif
