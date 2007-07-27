/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "polygon.h"

namespace udg {

Polygon::Polygon( QList< double* > points ) : DrawingPrimitive(),  DefaultBackgroundColor( Qt::darkGreen ), m_primitiveBackgroundColor( Qt::darkGreen ) 
{
    disableBackground();
    setPoints( points );
}
   
Polygon::~Polygon()
{}

int Polygon::getNumberOfPoints()
{
    return( m_pointsList.count() );
}
    

};  // end namespace udg

