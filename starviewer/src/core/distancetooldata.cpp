/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "distancetooldata.h"

namespace udg {

DistanceToolData::DistanceToolData( double p1[3], double p2[3] )
{
    setFirstPoint( p1 );
    setSecondPoint( p2 );
}
    
void DistanceToolData::setFirstPoint( double p1[3] )
{
    for ( int i = 0; i < 3; i++ )
        m_firstPoint[i] = p1[i];
}
    
void DistanceToolData::setSecondPoint( double p2[3] )
{
    for ( int i = 0; i < 3; i++ )
        m_secondPoint[i] = p2[i];
}
    
};  // end namespace udg

