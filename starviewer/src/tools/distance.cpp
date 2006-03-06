/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "distance.h"


namespace udg {

Distance::Distance()
{
}

Distance::Distance( Point first , Point second )
{
    setPoints( first , second );
}

Distance::Distance( Point points[2] )
{
    setPoints( points[0] , points[1] );
}

Distance::~Distance()
{
}

void Distance::setPoint( Point point , int n )
{
    if( n == 0 )
        setFirstPoint( point );
    else
        setSecondPoint( point );
}
    
void Distance::setPoints( Point first , Point second )
{
    setFirstPoint( first );
    setSecondPoint( second );
}

double Distance::getDistance()
{
    return m_first.distance( m_second );
}

};  // end namespace udg 
