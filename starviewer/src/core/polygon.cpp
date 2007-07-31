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
}

Polygon::Polygon( QList< double* > points ) : DrawingPrimitive()
{
    disableBackground();
    setPoints( points );
    discontinuousOff();
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

};  // end namespace udg

