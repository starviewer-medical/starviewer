/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "point.h"
#include "math.h"

namespace udg {

Point::Point()
{
    m_x = m_y = m_z =  0;
    
}

Point::Point( double points[3] )
{
    setValues( points );
}

Point::Point( double x , double y , double z )
{
    setValues( x , y , z );
}

Point::~Point()
{
}

void Point::setValues( double x, double y , double z )
{
    setX( x );
    setY( y );
    setZ( z );
}

void Point::setValues( double points[3] )
{
    setX( points[0] );
    setY( points[1] );
    setZ( points[2] );
}

double Point::distance( Point p )
{
    return sqrt( pow( (m_x-p.m_x) ,2) + pow( (m_y-p.m_y) ,2) + pow( (m_z-p.m_z) ,2) );
}

};  // end namespace udg 
