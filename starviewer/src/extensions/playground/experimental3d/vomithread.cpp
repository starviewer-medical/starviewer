#include "vomithread.h"

#include "informationtheory.h"


namespace udg {


VomiThread::VomiThread( const QVector<float> &viewProbabilities, const QVector<float> &objectProbabilities, const QVector<Vector3Float> &viewpointColors, QVector<Vector3Float> &colorVomi )
 : m_viewProbabilities( viewProbabilities ), m_objectProbabilities( objectProbabilities ), m_viewpointColors( viewpointColors ), m_colorVomi( colorVomi ),
   m_pOV( 0 ), m_dimX( 0 ), m_dimY( 0 ), m_dimZ( 0 ), m_yStart( 0 ), m_yStep( 0 ), m_computeColorVomi( false ), m_z( 0 )
{
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


void VomiThread::setMeasuresToCompute( bool computeColorVomi )
{
    m_computeColorVomi = computeColorVomi;
}


void VomiThread::setZ( int z )
{
    m_z = z;
}


void VomiThread::run()
{
    float **pOV = m_pOV;
    int dimX = m_dimX, dimY = m_dimY, dimXY = dimX * dimY;
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


float VomiThread::maximumColorVomi() const
{
    return m_maximumColorVomi;
}


} // namespace udg
