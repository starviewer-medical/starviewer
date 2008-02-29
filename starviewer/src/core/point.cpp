/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "point.h"

namespace udg {

Point::Point() : DrawingPrimitive()
{
    setWidth( 2. );
    rounded();
    filledOn();
}


Point::Point( double point[3] ) : DrawingPrimitive()
{
    setPosition( point );
    setWidth( 2. );
    rounded();
    filledOn();
}
   
Point::~Point()
{}
    
void Point::setPosition( double point[3] )
{
    for ( int i = 0; i < 3; i++ )
        m_position[i] = point[i];
}

};  // end namespace udg

