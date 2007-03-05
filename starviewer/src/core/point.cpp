/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "point.h"
#include "math.h"

namespace udg {

Point::Point() : Shape()
{
    for (int i = 0; i < 2; i++)
        m_point[i]=  0;
}

Point::Point( double points[2] )  : Shape()
{
    m_point[0] = points[0];
    m_point[1] = points[1];
}

Point::Point( double x , double y )  : Shape()
{
    m_point[0] = x;
    m_point[1] = y;
}

Point::~Point()
{
}

void Point::setValues( double x, double y )
{
    setX( x );
    setY( y );
}

void Point::setValues( double points[2] )
{
    setX( points[0] );
    setY( points[1] );
}

};  // end namespace udg 
