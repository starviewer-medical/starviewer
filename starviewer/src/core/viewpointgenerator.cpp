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

    m_viewpoints << Vector3( UNIT, UNIT, UNIT )
                 << Vector3( -UNIT, -UNIT, UNIT )
                 << Vector3( -UNIT, UNIT, -UNIT )
                 << Vector3( UNIT, -UNIT, -UNIT );
}


void ViewpointGenerator::setToUniform6( float radius )
{
    m_viewpoints.clear();

    m_viewpoints << Vector3( radius, 0.0, 0.0 )
                 << Vector3( -radius, 0.0, 0.0 )
                 << Vector3( 0.0, radius, 0.0 )
                 << Vector3( 0.0, -radius, 0.0 )
                 << Vector3( 0.0, 0.0, radius )
                 << Vector3( 0.0, 0.0, -radius );
}


void ViewpointGenerator::setToUniform8( float radius )
{
    m_viewpoints.clear();

    const double UNIT = radius / sqrt( 3.0 );

    m_viewpoints << Vector3( UNIT, UNIT, UNIT )
                 << Vector3( UNIT, UNIT, -UNIT )
                 << Vector3( UNIT, -UNIT, UNIT )
                 << Vector3( UNIT, -UNIT, -UNIT )
                 << Vector3( -UNIT, UNIT, UNIT )
                 << Vector3( -UNIT, UNIT, -UNIT )
                 << Vector3( -UNIT, -UNIT, UNIT )
                 << Vector3( -UNIT, -UNIT, -UNIT );
}


void ViewpointGenerator::setToUniform12( float radius )
{
    m_viewpoints.clear();

    const double PHI = ( 1.0 + sqrt( 5.0 ) ) / 2.0;

    m_viewpoints << Vector3( 0.0, 1.0, PHI ).normalize() * radius
                 << Vector3( 0.0, 1.0, -PHI ).normalize() * radius
                 << Vector3( 0.0, -1.0, PHI ).normalize() * radius
                 << Vector3( 0.0, -1.0, -PHI ).normalize() * radius
                 << Vector3( 1.0, PHI, 0.0 ).normalize() * radius
                 << Vector3( 1.0, -PHI, 0.0 ).normalize() * radius
                 << Vector3( -1.0, PHI, 0.0 ).normalize() * radius
                 << Vector3( -1.0, -PHI, 0.0 ).normalize() * radius
                 << Vector3( PHI, 0.0, 1.0 ).normalize() * radius
                 << Vector3( PHI, 0.0, -1.0 ).normalize() * radius
                 << Vector3( -PHI, 0.0, 1.0 ).normalize() * radius
                 << Vector3( -PHI, 0.0, -1.0 ).normalize() * radius;
}


void ViewpointGenerator::setToUniform20( float radius )
{
    m_viewpoints.clear();

    const double UNIT = radius / sqrt( 3.0 );
    const double PHI = ( 1.0 + sqrt( 5.0 ) ) / 2.0;
    const double INV_PHI = 1.0 / PHI;

    m_viewpoints << Vector3( UNIT, UNIT, UNIT )
                 << Vector3( UNIT, UNIT, -UNIT )
                 << Vector3( UNIT, -UNIT, UNIT )
                 << Vector3( UNIT, -UNIT, -UNIT )
                 << Vector3( -UNIT, UNIT, UNIT )
                 << Vector3( -UNIT, UNIT, -UNIT )
                 << Vector3( -UNIT, -UNIT, UNIT )
                 << Vector3( -UNIT, -UNIT, -UNIT )
                 << Vector3( 0.0, INV_PHI, PHI ).normalize() * radius
                 << Vector3( 0.0, INV_PHI, -PHI ).normalize() * radius
                 << Vector3( 0.0, -INV_PHI, PHI ).normalize() * radius
                 << Vector3( 0.0, -INV_PHI, -PHI ).normalize() * radius
                 << Vector3( INV_PHI, PHI, 0.0 ).normalize() * radius
                 << Vector3( INV_PHI, -PHI, 0.0 ).normalize() * radius
                 << Vector3( -INV_PHI, PHI, 0.0 ).normalize() * radius
                 << Vector3( -INV_PHI, -PHI, 0.0 ).normalize() * radius
                 << Vector3( PHI, 0.0, INV_PHI ).normalize() * radius
                 << Vector3( PHI, 0.0, -INV_PHI ).normalize() * radius
                 << Vector3( -PHI, 0.0, INV_PHI ).normalize() * radius
                 << Vector3( -PHI, 0.0, -INV_PHI ).normalize() * radius;
}


void ViewpointGenerator::setToQuasiUniform( unsigned char depth, float radius )
{
    SphereUniformPointCloudGenerator cloud( radius, depth );
    cloud.createPOVCloud();
    m_viewpoints = cloud.getVertices();
}


QVector<Vector3> ViewpointGenerator::viewpoints() const
{
    return m_viewpoints;
}


Vector3 ViewpointGenerator::viewpoint( int i ) const
{
    return m_viewpoints.at( i );
}


}
