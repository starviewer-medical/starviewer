#include "vomithread.h"

#include "informationtheory.h"


namespace udg {


VomiThread::VomiThread( const QVector<float> &viewProbabilities, const QVector<float> &objectProbabilities, const QVector<Vector3Float> &viewpointColors,
                        QVector<float> &vomi, QVector<float> &voxelSaliencies, QVector<Vector3Float> &colorVomi )
 : m_viewProbabilities( viewProbabilities ), m_objectProbabilities( objectProbabilities ), m_viewpointColors( viewpointColors ), m_vomi( vomi ), m_voxelSaliencies( voxelSaliencies ), m_colorVomi( colorVomi ),
   m_pOV( 0 ), m_dimX( 0 ), m_dimY( 0 ), m_dimZ( 0 ), m_yStart( 0 ), m_yStep( 0 ), m_computeVoxelSaliencies( false ), m_computeViewpointVomi( false ), m_computeColorVomi( false ), m_z( 0 ),
   m_maximumVomi( 0.0f ), m_maximumSaliency( 0.0f )
{
    m_viewpointVomi.resize( viewProbabilities.size() );
}


void VomiThread::setPOV( float **pOV )
{
    m_pOV = pOV;
}


void VomiThread::setDimensions( int dimX, int dimY, int dimZ )
{
    m_dimX = dimX; m_dimY = dimY; m_dimZ = dimZ;
}


void VomiThread::setYStartAndStep( int yStart, int yStep )
{
    m_yStart = yStart; m_yStep = yStep;
}


void VomiThread::setMeasuresToCompute( bool computeVoxelSaliencies, bool computeViewpointVomi, bool computeColorVomi )
{
    m_computeVoxelSaliencies = computeVoxelSaliencies;
    m_computeViewpointVomi = computeViewpointVomi;
    m_computeColorVomi = computeColorVomi;
}


void VomiThread::setZ( int z )
{
    m_z = z;
}


void VomiThread::run()
{
    float **pOV = m_pOV;
    int dimX = m_dimX, dimY = m_dimY, dimZ = m_dimZ, dimXY = dimX * dimY;
    int z = m_z;

    int nViewpoints = m_viewProbabilities.size();
    QVector<float> pVoi( nViewpoints ); // p(V|oi)
    QVector<float> pVoj( nViewpoints ); // p(V|oj)

    int pOvShift = ( z - 1 ) * dimXY;

    for ( int y = m_yStart; y < dimY; y += m_yStep )
    {
        int i = z * dimXY + y * dimX;

        for ( int x = 0; x < dimX; x++, i++ )
        {
            Q_ASSERT( i == x + y * dimX + z * dimXY );

            float poi = m_objectProbabilities.at( i );    // p(oi)
            Q_ASSERT( poi == poi );

            // p(V|oi)
            if ( poi == 0.0 ) pVoi.fill( 0.0f );    // si p(oi) == 0 vol dir que el vòxel no es veu des d'enlloc --> p(V|oi) ha de ser tot zeros
            else for ( int k = 0; k < nViewpoints; k++ ) pVoi[k] = m_viewProbabilities.at( k ) * pOV[k][i - pOvShift] / poi;

            if ( m_computeVoxelSaliencies )
            {
//                        int neighbours[6] = { x - 1 + y * dimX + z * dimXY, x + 1 + y * dimX + z * dimXY,
//                                              x + ( y - 1 ) * dimX + z * dimXY, x + ( y + 1 ) * dimX + z * dimXY,
//                                              x + y * dimX + ( z - 1 ) * dimXY, x + y * dimX + ( z + 1 ) * dimXY };
//                        bool validNeighbours[6] = { x > 0, x + 1 < dimX,
//                                                    y > 0, y + 1 < dimY,
//                                                    z > 0, z + 1 < dimZ };
                int neighbours[26] = { x-1 + (y-1) * dimX + (z-1) * dimXY, x-1 + (y-1) * dimX +  z    * dimXY, x-1 + (y-1) * dimX + (z+1) * dimXY,
                                       x-1 +  y    * dimX + (z-1) * dimXY, x-1 +  y    * dimX +  z    * dimXY, x-1 +  y    * dimX + (z+1) * dimXY,
                                       x-1 + (y+1) * dimX + (z-1) * dimXY, x-1 + (y+1) * dimX +  z    * dimXY, x-1 + (y+1) * dimX + (z+1) * dimXY,
                                       x   + (y-1) * dimX + (z-1) * dimXY, x   + (y-1) * dimX +  z    * dimXY, x   + (y-1) * dimX + (z+1) * dimXY,
                                       x   +  y    * dimX + (z-1) * dimXY,                                     x   +  y    * dimX + (z+1) * dimXY,
                                       x   + (y+1) * dimX + (z-1) * dimXY, x   + (y+1) * dimX +  z    * dimXY, x   + (y+1) * dimX + (z+1) * dimXY,
                                       x+1 + (y-1) * dimX + (z-1) * dimXY, x+1 + (y-1) * dimX +  z    * dimXY, x+1 + (y-1) * dimX + (z+1) * dimXY,
                                       x+1 +  y    * dimX + (z-1) * dimXY, x+1 +  y    * dimX +  z    * dimXY, x+1 +  y    * dimX + (z+1) * dimXY,
                                       x+1 + (y+1) * dimX + (z-1) * dimXY, x+1 + (y+1) * dimX +  z    * dimXY, x+1 + (y+1) * dimX + (z+1) * dimXY };
                bool validNeighbours[26] = {   x > 0    &&   y > 0    &&   z > 0   ,   x > 0    &&   y > 0                 ,   x > 0    &&   y > 0    && z+1 < dimZ,
                                               x > 0                  &&   z > 0   ,   x > 0                               ,   x > 0                  && z+1 < dimZ,
                                               x > 0    && y+1 < dimY &&   z > 0   ,   x > 0    && y+1 < dimY              ,   x > 0    && y+1 < dimY && z+1 < dimZ,
                                                             y > 0    &&   z > 0   ,                 y > 0                 ,                 y > 0    && z+1 < dimZ,
                                                                           z > 0   ,                                                                     z+1 < dimZ,
                                                           y+1 < dimY &&   z > 0   ,               y+1 < dimY              ,               y+1 < dimY && z+1 < dimZ,
                                             x+1 < dimX &&   y > 0    &&   z > 0   , x+1 < dimX &&   y > 0                 , x+1 < dimX &&   y > 0    && z+1 < dimZ,
                                             x+1 < dimX               &&   z > 0   , x+1 < dimX                            , x+1 < dimX               && z+1 < dimZ,
                                             x+1 < dimX && y+1 < dimY &&   z > 0   , x+1 < dimX && y+1 < dimY              , x+1 < dimX && y+1 < dimY && z+1 < dimZ };
                const float SQRT_1_2 = 1.0f / sqrt( 2.0f ), SQRT_1_3 = 1.0f / sqrt( 3.0f );
                float weights[26] = { SQRT_1_3, SQRT_1_2, SQRT_1_3,
                                      SQRT_1_2,   1.0f  , SQRT_1_2,
                                      SQRT_1_3, SQRT_1_2, SQRT_1_3,
                                      SQRT_1_2,   1.0f  , SQRT_1_2,
                                        1.0f  ,             1.0f  ,
                                      SQRT_1_2,   1.0f  , SQRT_1_2,
                                      SQRT_1_3, SQRT_1_2, SQRT_1_3,
                                      SQRT_1_2,   1.0f  , SQRT_1_2,
                                      SQRT_1_3, SQRT_1_2, SQRT_1_3 };

                float saliency = 0.0f;
                float totalWeight = 0.0f;

                // iterem pels veïns
                for ( int j = 0; j < 26; j++ )
                {
                    if ( !validNeighbours[j] ) continue;

                    totalWeight += weights[j];

                    float poj = m_objectProbabilities.at( neighbours[j] );    // p(oj)
                    Q_ASSERT( poj == poj );
                    float poij = poi + poj; // p(ô)

                    if ( poij == 0.0f ) continue;

                    // p(V|oj)
                    if ( poj == 0.0f ) pVoj.fill( 0.0f );   // si p(oj) == 0 vol dir que el vòxel no es veu des d'enlloc --> p(V|oj) ha de ser tot zeros
                    else for ( int k = 0; k < nViewpoints; k++ ) pVoj[k] = m_viewProbabilities.at( k ) * pOV[k][neighbours[j] - pOvShift] / poj;

                    float s = weights[j] * InformationTheory<float>::jensenShannonDivergence( poi / poij, poj / poij, pVoi, pVoj );
                    Q_ASSERT( s == s );
                    saliency += s;
                }

                saliency /= totalWeight;
                m_voxelSaliencies[i] = saliency;
                if ( saliency > m_maximumSaliency ) m_maximumSaliency = saliency;
            }

            if ( m_computeViewpointVomi )
            {
                for ( int k = 0; k < nViewpoints; k++ ) m_viewpointVomi[k] += m_vomi.at( i ) * pVoi.at( k );
            }

            if ( m_computeColorVomi )
            {
                Vector3Float white( 1.0f, 1.0f, 1.0f );
                Vector3Float colorVomi;

                for ( int k = 0; k < nViewpoints; k++ )
                {
                    float pvo = pVoi.at( k );
                    if ( pvo > 0.0f ) colorVomi += pvo * log( pvo / m_viewProbabilities.at( k ) ) * ( white - m_viewpointColors.at( k ) );
                }

                colorVomi /= log( 2.0f );
                Q_ASSERT( colorVomi.x == colorVomi.x && colorVomi.y == colorVomi.y && colorVomi.z == colorVomi.z );
                m_colorVomi[i] = colorVomi;

                if ( colorVomi.x > m_maximumColorVomi ) m_maximumColorVomi = colorVomi.x;
                if ( colorVomi.y > m_maximumColorVomi ) m_maximumColorVomi = colorVomi.y;
                if ( colorVomi.z > m_maximumColorVomi ) m_maximumColorVomi = colorVomi.z;
            }
        }
    }
}


float VomiThread::maximumVomi() const
{
    return m_maximumVomi;
}


float VomiThread::maximumSaliency() const
{
    return m_maximumSaliency;
}


const QVector<float>& VomiThread::viewpointVomi() const
{
    return m_viewpointVomi;
}


float VomiThread::maximumColorVomi() const
{
    return m_maximumColorVomi;
}


} // namespace udg
