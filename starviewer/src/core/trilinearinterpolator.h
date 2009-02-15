#ifndef UDGTRILINEARINTERPOLATOR_H
#define UDGTRILINEARINTERPOLATOR_H


#include <cmath>

#include "vector3.h"


namespace udg {


/**
 * Classe que implementa una interpolació trilineal genèrica.
 */
class TrilinearInterpolator {

public:

    TrilinearInterpolator();
    ~TrilinearInterpolator();

    /// Assigna els increments que es faran servir per calcular els offsets.
    void setIncrements( int xInc, int yInc, int zInc );
    /// Calcula les posicions per una posició donada, i les retorna com a paràmetres de sortida.
    void getPositions( const Vector3 &position, Vector3 positions[] ) const;
    /**
     * Calcula els offsets i els pesos per una posició donada, i els retorna com a paràmetres de sortida.
     * \param position La posició per a la qual es vol calcular els offsets i els pesos.
     * \param offsets Ha de ser un array de mida 8 on es guardaran els offsets.
     * \param weights Ha de ser un array de mida 8 on es guardaran els pesos.
     */
    void getOffsetsAndWeights( const Vector3 &position, int offsets[], double weights[] ) const;
    /// Retorna un valor interpolat a partir d'un array de valors, uns offsets i uns pesos.
    template <class TOutput, class TInput> static TOutput interpolate( const TInput *values, const int offsets[], const double weights[] );

private:

    int m_increments[8];

};


inline void TrilinearInterpolator::setIncrements( int xInc, int yInc, int zInc )
{
    //m_increments[0] = 0;
    m_increments[1] = xInc;
    m_increments[2] = yInc;
    m_increments[3] = xInc + yInc;
    m_increments[4] = zInc;
    m_increments[5] = xInc + zInc;
    m_increments[6] = yInc + zInc;
    m_increments[7] = xInc + yInc + zInc;
}


inline void TrilinearInterpolator::getPositions( const Vector3 &position, Vector3 positions[] ) const
{
    // |-------|-----|
    //     a      b
    double x = floor( position.x );
    double y = floor( position.y );
    double z = floor( position.z );

    positions[0] = Vector3( x, y, z );
    positions[1] = Vector3( x + 1.0, y, z );
    positions[2] = Vector3( x, y + 1.0, z );
    positions[3] = Vector3( x + 1.0, y + 1.0, z );
    positions[4] = Vector3( x, y, z + 1.0 );
    positions[5] = Vector3( x + 1.0, y, z + 1.0 );
    positions[6] = Vector3( x, y + 1.0, z + 1.0 );
    positions[7] = Vector3( x + 1.0, y + 1.0, z + 1.0 );
}


inline void TrilinearInterpolator::getOffsetsAndWeights( const Vector3 &position, int offsets[], double weights[] ) const
{
    // |-------|-----|
    //     a      b
    int x = static_cast<int>( std::floor( position.x ) );
    int y = static_cast<int>( std::floor( position.y ) );
    int z = static_cast<int>( std::floor( position.z ) );
    double ax = position.x - x, ay = position.y - y, az = position.z - z;
    double bx = 1.0 - ax, by = 1.0 - ay, bz = 1.0 - az;
    int baseOffset = x * m_increments[1] + y * m_increments[2] + z * m_increments[4];

    for ( int i = 0; i < 8; i++ ) offsets[i] = baseOffset + m_increments[i];

    weights[0] = bx * by * bz;
    weights[1] = ax * by * bz;
    weights[2] = bx * ay * bz;
    weights[3] = ax * ay * bz;
    weights[4] = bx * by * az;
    weights[5] = ax * by * az;
    weights[6] = bx * ay * az;
    weights[7] = ax * ay * az;
}


template <class TOutput, class TInput> inline TOutput TrilinearInterpolator::interpolate( const TInput *values, const int offsets[], const double weights[] )
{
    TOutput interpolatedValue = weights[0] * values[offsets[0]];
    for ( int i = 1; i < 8; i++ ) interpolatedValue += weights[i] * values[offsets[i]];

    return interpolatedValue;
}


}


#endif
