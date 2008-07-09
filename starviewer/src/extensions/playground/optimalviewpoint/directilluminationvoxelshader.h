#ifndef UDGDIRECTILLUMINATIONVOXELSHADER_H
#define UDGDIRECTILLUMINATIONVOXELSHADER_H


#include <ambientvoxelshader.h>


namespace udg {


/**
 * És un voxel shader que pinta el volum amb il·luminació directa (ambient, difusa, especular).
 */
class DirectIlluminationVoxelShader : public AmbientVoxelShader {

public:

    DirectIlluminationVoxelShader();
    virtual ~DirectIlluminationVoxelShader();

    /// Assigna la taula de normals codificades.
    void setEncodedNormals( const unsigned short *encodedNormals );
    /// Assigna les taules d'il·luminació difusa (indexades per les normals codificades).
    void setDiffuseShadingTables( const float *red, const float *green, const float *blue );
    /// Assigna les taules d'il·luminació especular (indexades per les normals codificades).
    void setSpecularShadingTables( const float *red, const float *green, const float *blue );

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual QColor shade( int offset, const QColor &baseColor = Qt::white ) const;
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

protected:

    const unsigned short *m_encodedNormals;
    const float *m_redDiffuseShadingTable, *m_greenDiffuseShadingTable, *m_blueDiffuseShadingTable;
    const float *m_redSpecularShadingTable, *m_greenSpecularShadingTable, *m_blueSpecularShadingTable;

};


}


#endif
