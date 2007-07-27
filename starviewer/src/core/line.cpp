/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "line.h"

namespace udg {

Line::Line() : DrawingPrimitive()
{
    setWidth( 1. );
    discontinuousOff();
}


Line::Line( double point1[3], double point2[3] ) : DrawingPrimitive()
{
    setFirstPoint( point1 );
    setSecondPoint( point2 );
    setWidth( 1. );
    discontinuousOff();
}
   
Line::~Line()
{}
    
void Line::setFirstPoint( double point[3] )
{
    for ( int i = 0; i < 3; i++ )
        m_firstPoint[i] = point[i];
}

void Line::setSecondPoint( double point[3] )
{
    for ( int i = 0; i < 3; i++ )
        m_secondPoint[i] = point[i];
}
    
};  // end namespace udg

