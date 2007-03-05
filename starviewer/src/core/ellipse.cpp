/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "ellipse.h"
#include "point.h"
#include "polyline.h"
#include <math.h>

#include <iostream.h>

namespace udg {

Ellipse::Ellipse()
 : ROI()
{
    m_behavior = ELLIPSE;
}


Ellipse::~Ellipse()
{
}

void Ellipse::setMinorRadius( Point p1 )
{
    m_minorRadius.setX( p1.getX() );
    m_minorRadius.setY( p1.getY() );
}

void Ellipse::setMajorRadius( Point p1 )
{
    m_majorRadius.setX( p1.getX() );
    m_majorRadius.setY( p1.getY() );
}

Point Ellipse::getMinorRadius()
{
    return (m_minorRadius);
}

Point Ellipse::getMajorRadius()
{
    return (m_majorRadius);
}

void Ellipse::setCenter( Point p1 )
{
    m_center.setX( p1.getX() );
    m_center.setY( p1.getY() );
}
    
Point Ellipse::getCenter()
{ 
    return (m_center);
}

void Ellipse::convertToCircle()
{
    setMinorRadius( getMajorRadius() );
    m_behavior = CIRCLE;
}

// void Ellipse::setWholeExtend()
// {
// 
// }

void Ellipse::computeArea()
{
    /*
    Àrea d'una el·lipse= PI * radi major * radi menor
    Àrea d'un cercle: pi * radi * radi
    
    Podem utilitzar la mateixa fòrmula tant per calcular l'àrea del cercle com la de l'el·lipse, ja que en el cas de cercle,
    edls dos eixos coincideixen, i per tant es com si fessim radi al quadrat.
    */
    
    Polyline minor;
    Polyline major;
    
    minor.addPoint( m_minorRadius );
    minor.addPoint( m_center );
    
    major.addPoint( m_majorRadius );
    major.addPoint( m_center );

    m_area = M_PI * ( minor.getDistance2D() ) * ( major.getDistance2D() ); 
}
}
