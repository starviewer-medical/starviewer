/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "polygon.h"

namespace udg {

Polygon::Polygon() : DrawingPrimitive()
{
    disableBackground();
    setWidth( 1. );
}

Polygon::Polygon( QList< double* > points ) : DrawingPrimitive()
{
    disableBackground();
    setPoints( points );
    discontinuousOff();
    setWidth( 1. );
}
   
Polygon::~Polygon()
{}

int Polygon::getNumberOfPoints()
{
    return( m_pointsList.count() );
}

void Polygon::addPoint( double newPoint[3] )
{
    m_pointsList << newPoint;
}

void Polygon::refreshPolygon()
{
    emit  polygonChanged( this );
}

};  // end namespace udg

