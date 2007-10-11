/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "polyline.h"
#include <math.h>

namespace udg {

Polyline::Polyline()
 : Shape()
{
    m_typeOfInterpolation = LINES;
    m_polylinePoints = std::vector<Point>(0);
}


Polyline::~Polyline()
{
}

void Polyline::addPoint( Point point )
{

    m_polylinePoints.push_back( point );
}

int Polyline::getNumberOfPoints()
{
    return m_polylinePoints.size();
}

void Polyline::removeLastPoint()
{
    m_polylinePoints.pop_back();
}

void Polyline::removeFirstPoint()
{
    m_polylinePoints.erase( m_polylinePoints.begin() );
}

void Polyline::removePointOfPosition( unsigned int index )
{
    if ( index < m_polylinePoints.size()-1 )
    {
        it = m_polylinePoints.begin();

        for (unsigned int i = 0; i < index; i++)
            it++;

        m_polylinePoints.erase( it );
    }
}

Point Polyline::getFirstPoint()
{
    it = m_polylinePoints.begin();
    return ( *it );
}

Point Polyline::getLastPoint()
{
    it = m_polylinePoints.end();
    return ( *it );
}

Point Polyline::getPointOfPosition( unsigned int index )
{
    if (index < m_polylinePoints.size())
    {
        it = m_polylinePoints.begin();

        for (unsigned int i = 0; i < index; i++)
            it++;

        return (*it);
    }
    else
        return 0;
}

void Polyline::goToBegin()
{
    it = m_polylinePoints.begin();
}

void Polyline::goToEnd()
{
    it = m_polylinePoints.end();
}

void Polyline::next()
{
    if ( it < m_polylinePoints.end() )
        it++;
}

void Polyline::previous()
{
    if ( it > m_polylinePoints.begin() )
        it--;
}

Point Polyline::getCurrentPoint()
{
    return (*it);
}

bool Polyline::hasNext()
{
    return (it == m_polylinePoints.end());
}

bool Polyline::hasPrevious()
{
    return (it == m_polylinePoints.begin());
}

double Polyline::getDistance2D()
{
    double valor = 0;
    double xx;
    double yy;

    if ( m_polylinePoints.size() == 2 )
    {
        xx = ( getPointOfPosition( 1 ).getX() - getPointOfPosition( 0 ).getX() );
        yy = ( getPointOfPosition( 1 ).getY() - getPointOfPosition( 0 ).getY() );
        valor = pow( xx, 2 ) + pow( yy, 2 );
    }
    else if ( m_polylinePoints.size() > 2 )
    {
        //per cada parell de punts calculem la distància euclidiana i incrementem la variable valor
        for ( unsigned int i = 1; i < m_polylinePoints.size()-1; i++ )
        {
            xx = ( getPointOfPosition( i ).getX() - getPointOfPosition( i-1 ).getX() );
            yy = ( getPointOfPosition( i ).getY() - getPointOfPosition( i-1 ).getY() );
            valor += pow( xx, 2 ) + pow( yy, 2 );
        }
    }
    return ( sqrt( valor ) );
}

double Polyline::getDistance3D()
{
    double valor = 0;
    double xx;
    double yy;
    double zz;

    if ( m_polylinePoints.size() == 2 )
    {
        xx = ( getPointOfPosition( 1 ).getX() - getPointOfPosition( 0 ).getX() );
        yy = ( getPointOfPosition( 1 ).getY() - getPointOfPosition( 0 ).getY() );
//         zz = ( getPointOfPosition( 1 ).getZ() - getPointOfPosition( 0 ).getZ() );
        valor = pow( xx, 2 ) + pow( yy, 2 );// + pow( zz, 2 );
    }
    else if ( m_polylinePoints.size() > 2 )
    {
        //per cada parell de punts calculem la distància euclidiana i incrementem la variable valor
        for ( unsigned int i = 1; i < m_polylinePoints.size()-1; i++ )
        {
            xx = ( getPointOfPosition( i ).getX() - getPointOfPosition( i-1 ).getX() );
            yy = ( getPointOfPosition( i ).getY() - getPointOfPosition( i-1 ).getY() );
//             zz = ( getPointOfPosition( i ).getZ() - getPointOfPosition( i-1 ).getZ() );
            valor += pow( xx, 2 ) + pow( yy, 2 );// + pow( zz, 2 );
        }
    }
    return ( sqrt( valor ) );
}

}
