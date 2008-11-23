#ifndef UDGCOMBININGVOXELSHADER_H
#define UDGCOMBININGVOXELSHADER_H


#include "voxelshader.h"

#include "trilinearinterpolator.h"


namespace udg {


/**
 * És un voxel shader que en combina dos.
 *
 * Els voxel shaders combinats han d'implementar els mètodes nvShade. La recomanació és que implementin la funcionalitat als nvShade (per a
 * poder ser combinats) com a inline i no virtuals, i des dels shade (virtuals, i poden ser inline o no, i són necessaris per complir amb
 * l'especificació de VoxelShader) cridin els nvShade, tal com fa aquesta mateixa classe.
 */
template <class VS1, class VS2>
class CombiningVoxelShader : public VoxelShader {

public:

    CombiningVoxelShader();
    virtual ~CombiningVoxelShader();

    /// Assigna els dos voxel shaders que es combinaran.
    void setVoxelShaders( VS1 *voxelShader1, VS2 *voxelShader2 );

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual HdrColor shade( int offset, const Vector3 &direction, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    virtual HdrColor shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna el color corresponent al vòxel a la posició offset.
    HdrColor nvShade( int offset, const Vector3 &direction, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    HdrColor nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

private:

    VS1 *m_voxelShader1;
    VS2 *m_voxelShader2;

};


template <class VS1, class VS2>
inline HdrColor CombiningVoxelShader<VS1, VS2>::shade( int offset, const Vector3 &direction, const HdrColor &baseColor ) const
{
    return nvShade( offset, direction, baseColor );
}


template <class VS1, class VS2>
inline HdrColor CombiningVoxelShader<VS1, VS2>::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor ) const
{
    return nvShade( position, direction, interpolator, baseColor );
}


template <class VS1, class VS2>
inline HdrColor CombiningVoxelShader<VS1, VS2>::nvShade( int offset, const Vector3 &direction, const HdrColor &baseColor ) const
{
    Q_ASSERT( m_voxelShader1 );
    Q_ASSERT( m_voxelShader2 );

    HdrColor color1 = m_voxelShader1->nvShade( offset, direction, baseColor );
    return m_voxelShader2->nvShade( offset, direction, color1 );
}


template <class VS1, class VS2>
inline HdrColor CombiningVoxelShader<VS1, VS2>::nvShade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor ) const
{
    Q_ASSERT( m_voxelShader1 );
    Q_ASSERT( m_voxelShader2 );

    HdrColor color1 = m_voxelShader1->nvShade( position, direction, interpolator, baseColor );
    return m_voxelShader2->nvShade( position, direction, interpolator, color1 );
}


}


#include "combiningvoxelshader.cpp"


#endif
