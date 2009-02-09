#include "viewpointgenerator.h"

#include "sphereuniformpointcloudgenerator.h"
#include "vector3.h"


namespace udg {


ViewpointGenerator::ViewpointGenerator()
{
}


ViewpointGenerator::~ViewpointGenerator()
{
}


void ViewpointGenerator::setToUniform4( float radius )
{
    m_viewpoints.clear();

    const double UNIT = radius / sqrt( 3.0 );

    m_viewpoints << Vector3( UNIT, UNIT, UNIT )     // 0
                 << Vector3( -UNIT, -UNIT, UNIT )   // 1
                 << Vector3( -UNIT, UNIT, -UNIT )   // 2
                 << Vector3( UNIT, -UNIT, -UNIT );  // 3
    m_neighbours << ( QVector<int>() << 1 << 2 << 3 )   // 0
                 << ( QVector<int>() << 0 << 2 << 3 )   // 1
                 << ( QVector<int>() << 0 << 1 << 3 )   // 2
                 << ( QVector<int>() << 0 << 1 << 2 );  // 3
}


void ViewpointGenerator::setToUniform6( float radius )
{
    m_viewpoints.clear();

    m_viewpoints << Vector3( radius, 0.0, 0.0 )     // 0
                 << Vector3( -radius, 0.0, 0.0 )    // 1
                 << Vector3( 0.0, radius, 0.0 )     // 2
                 << Vector3( 0.0, -radius, 0.0 )    // 3
                 << Vector3( 0.0, 0.0, radius )     // 4
                 << Vector3( 0.0, 0.0, -radius );   // 5
    m_neighbours << ( QVector<int>() << 2 << 3 << 4 << 5 )  // 0
                 << ( QVector<int>() << 2 << 3 << 4 << 5 )  // 1
                 << ( QVector<int>() << 0 << 1 << 4 << 5 )  // 2
                 << ( QVector<int>() << 0 << 1 << 4 << 5 )  // 3
                 << ( QVector<int>() << 0 << 1 << 2 << 3 )  // 4
                 << ( QVector<int>() << 0 << 1 << 2 << 3 ); // 5
}


void ViewpointGenerator::setToUniform8( float radius )
{
    m_viewpoints.clear();

    const double UNIT = radius / sqrt( 3.0 );

    m_viewpoints << Vector3( UNIT, UNIT, UNIT )     // 0
                 << Vector3( UNIT, UNIT, -UNIT )    // 1
                 << Vector3( UNIT, -UNIT, UNIT )    // 2
                 << Vector3( UNIT, -UNIT, -UNIT )   // 3
                 << Vector3( -UNIT, UNIT, UNIT )    // 4
                 << Vector3( -UNIT, UNIT, -UNIT )   // 5
                 << Vector3( -UNIT, -UNIT, UNIT )   // 6
                 << Vector3( -UNIT, -UNIT, -UNIT ); // 7
    m_neighbours << ( QVector<int>() << 1 << 2 << 4 )   // 0
                 << ( QVector<int>() << 0 << 3 << 5 )   // 1
                 << ( QVector<int>() << 0 << 3 << 6 )   // 2
                 << ( QVector<int>() << 1 << 2 << 7 )   // 3
                 << ( QVector<int>() << 0 << 5 << 6 )   // 4
                 << ( QVector<int>() << 1 << 4 << 7 )   // 5
                 << ( QVector<int>() << 2 << 4 << 7 )   // 6
                 << ( QVector<int>() << 3 << 5 << 6 );  // 7
}


void ViewpointGenerator::setToUniform12( float radius )
{
    m_viewpoints.clear();

    const double PHI = ( 1.0 + sqrt( 5.0 ) ) / 2.0;

    m_viewpoints << Vector3( 0.0, 1.0, PHI ).normalize() * radius       //  0
                 << Vector3( 0.0, 1.0, -PHI ).normalize() * radius      //  1
                 << Vector3( 0.0, -1.0, PHI ).normalize() * radius      //  2
                 << Vector3( 0.0, -1.0, -PHI ).normalize() * radius     //  3
                 << Vector3( 1.0, PHI, 0.0 ).normalize() * radius       //  4
                 << Vector3( 1.0, -PHI, 0.0 ).normalize() * radius      //  5
                 << Vector3( -1.0, PHI, 0.0 ).normalize() * radius      //  6
                 << Vector3( -1.0, -PHI, 0.0 ).normalize() * radius     //  7
                 << Vector3( PHI, 0.0, 1.0 ).normalize() * radius       //  8
                 << Vector3( PHI, 0.0, -1.0 ).normalize() * radius      //  9
                 << Vector3( -PHI, 0.0, 1.0 ).normalize() * radius      // 10
                 << Vector3( -PHI, 0.0, -1.0 ).normalize() * radius;    // 11
    m_neighbours << ( QVector<int>() <<  2 <<  4 <<  6 <<  8 << 10 )    //  0
                 << ( QVector<int>() <<  3 <<  4 <<  6 <<  9 << 11 )    //  1
                 << ( QVector<int>() <<  0 <<  5 <<  7 <<  8 << 10 )    //  2
                 << ( QVector<int>() <<  1 <<  5 <<  7 <<  9 << 11 )    //  3
                 << ( QVector<int>() <<  0 <<  1 <<  6 <<  8 <<  9 )    //  4
                 << ( QVector<int>() <<  2 <<  3 <<  7 <<  8 <<  9 )    //  5
                 << ( QVector<int>() <<  0 <<  1 <<  4 << 10 << 11 )    //  6
                 << ( QVector<int>() <<  2 <<  3 <<  5 << 10 << 11 )    //  7
                 << ( QVector<int>() <<  0 <<  2 <<  4 <<  5 <<  9 )    //  8
                 << ( QVector<int>() <<  1 <<  3 <<  4 <<  5 <<  8 )    //  9
                 << ( QVector<int>() <<  0 <<  2 <<  6 <<  7 << 11 )    // 10
                 << ( QVector<int>() <<  1 <<  3 <<  6 <<  7 << 10 );   // 11
}


void ViewpointGenerator::setToUniform20( float radius )
{
    m_viewpoints.clear();

    const double UNIT = radius / sqrt( 3.0 );
    const double PHI = ( 1.0 + sqrt( 5.0 ) ) / 2.0;
    const double INV_PHI = 1.0 / PHI;

    m_viewpoints << Vector3( UNIT, UNIT, UNIT )                             //  0
                 << Vector3( UNIT, UNIT, -UNIT )                            //  1
                 << Vector3( UNIT, -UNIT, UNIT )                            //  2
                 << Vector3( UNIT, -UNIT, -UNIT )                           //  3
                 << Vector3( -UNIT, UNIT, UNIT )                            //  4
                 << Vector3( -UNIT, UNIT, -UNIT )                           //  5
                 << Vector3( -UNIT, -UNIT, UNIT )                           //  6
                 << Vector3( -UNIT, -UNIT, -UNIT )                          //  7
             << Vector3( 0.0, INV_PHI, PHI ).normalize() * radius           //  8
                 << Vector3( 0.0, INV_PHI, -PHI ).normalize() * radius      //  9
                 << Vector3( 0.0, -INV_PHI, PHI ).normalize() * radius      // 10
                 << Vector3( 0.0, -INV_PHI, -PHI ).normalize() * radius     // 11
                 << Vector3( INV_PHI, PHI, 0.0 ).normalize() * radius       // 12
                 << Vector3( INV_PHI, -PHI, 0.0 ).normalize() * radius      // 13
                 << Vector3( -INV_PHI, PHI, 0.0 ).normalize() * radius      // 14
                 << Vector3( -INV_PHI, -PHI, 0.0 ).normalize() * radius     // 15
                 << Vector3( PHI, 0.0, INV_PHI ).normalize() * radius       // 16
                 << Vector3( PHI, 0.0, -INV_PHI ).normalize() * radius      // 17
                 << Vector3( -PHI, 0.0, INV_PHI ).normalize() * radius      // 18
                 << Vector3( -PHI, 0.0, -INV_PHI ).normalize() * radius;    // 19
    m_neighbours << ( QVector<int>() <<  8 << 12 << 16 )    //  0
                 << ( QVector<int>() <<  9 << 12 << 17 )    //  1
                 << ( QVector<int>() << 10 << 13 << 16 )    //  2
                 << ( QVector<int>() << 11 << 13 << 17 )    //  3
                 << ( QVector<int>() <<  8 << 14 << 18 )    //  4
                 << ( QVector<int>() <<  9 << 14 << 19 )    //  5
                 << ( QVector<int>() << 10 << 15 << 18 )    //  6
                 << ( QVector<int>() << 11 << 15 << 19 )    //  7
                 << ( QVector<int>() <<  0 <<  4 << 10 )    //  8
                 << ( QVector<int>() <<  1 <<  5 << 11 )    //  9
                 << ( QVector<int>() <<  2 <<  6 <<  8 )    // 10
                 << ( QVector<int>() <<  3 <<  7 <<  9 )    // 11
                 << ( QVector<int>() <<  0 <<  1 << 14 )    // 12
                 << ( QVector<int>() <<  2 <<  3 << 15 )    // 13
                 << ( QVector<int>() <<  4 <<  5 << 12 )    // 14
                 << ( QVector<int>() <<  7 <<  6 << 13 )    // 15
                 << ( QVector<int>() <<  0 <<  2 << 17 )    // 16
                 << ( QVector<int>() <<  1 <<  3 << 16 )    // 17
                 << ( QVector<int>() <<  4 <<  6 << 19 )    // 18
                 << ( QVector<int>() <<  5 <<  7 << 18 );   // 19
}


void ViewpointGenerator::setToQuasiUniform( unsigned char depth, float radius )
{
    SphereUniformPointCloudGenerator cloud( radius, depth );
    cloud.createPOVCloud();
    m_viewpoints = cloud.getVertices();
    m_neighbours = cloud.getNeighbours();
}


QVector<Vector3> ViewpointGenerator::viewpoints() const
{
    return m_viewpoints;
}


Vector3 ViewpointGenerator::viewpoint( int i ) const
{
    return m_viewpoints.at( i );
}


QVector<int> ViewpointGenerator::neighbours( int i ) const
{
    return m_neighbours.at( i );
}


}
