/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "distancetooldata.h"
#include "distance.h"
#include <QString>

namespace udg {

DistanceToolData::DistanceToolData( double p1[3], double p2[3] )
{
    setFirstPoint( p1 );
    setSecondPoint( p2 );
}

DistanceToolData::~DistanceToolData()
{}
    
void DistanceToolData::setFirstPoint( double p1[3] )
{
    for ( int i = 0; i < 3; i++ )
        m_firstPoint[i] = p1[i];
    
    calculateDistance();
    emit firstPointChanged();
}
    
void DistanceToolData::setSecondPoint( double p2[3] )
{
    for ( int i = 0; i < 3; i++ )
        m_secondPoint[i] = p2[i];
    
    calculateDistance();
    emit secondPointChanged();
}

void DistanceToolData::calculateDistance()
{
    Distance d( m_firstPoint, m_secondPoint );

    m_text = QString( "%1 mm" ).arg( d.getDistance2D(), 0, 'f',  2);
    
    m_textPosition[0] = ( m_firstPoint[0] + m_secondPoint[0] ) / 2;
    m_textPosition[1] = ( m_firstPoint[1] + m_secondPoint[1] ) / 2;
    m_textPosition[2] = ( m_firstPoint[2] + m_secondPoint[2] ) / 2;
    
    emit distanceTextChanged();
}
    
};  // end namespace udg

