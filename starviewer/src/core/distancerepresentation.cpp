/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "distancerepresentation.h"
#include "text.h"
#include "line.h"
#include "distancetooldata.h"

namespace udg {

DistanceRepresentation::DistanceRepresentation( DistanceToolData *dtd ) : Representation()
{ 
    m_line = new Line( dtd->getFirstPoint(), dtd->getSecondPoint() );
    m_text = new Text( dtd->getTextPosition(), dtd->getDistanceText() );
    
    m_distanceToolData = dtd;
    
    //creem les connexions entre aquest objecte i els seu atribut DistanceToolData
    connect( m_distanceToolData, SIGNAL( firstPointChanged() ), this , SLOT( updateFirstPointLine() ) );
    connect( m_distanceToolData, SIGNAL( secondPointChanged() ), this , SLOT( updateSecondPointLine() ) );
}
    
DistanceRepresentation::~DistanceRepresentation()
{
    delete m_line;
    delete m_text;
    delete m_distanceToolData;
}    

void DistanceRepresentation::setText( Text *text )
{
    m_text = text;
}
    
void DistanceRepresentation::setLine( Line *line )
{
    m_line = line;
}

void DistanceRepresentation::setDistanceToolData( DistanceToolData *dtd )
{
    m_distanceToolData = dtd;
}
    
Text* DistanceRepresentation::getText()
{
    return( m_text );   
}
    
Line* DistanceRepresentation::getLine()
{
    return( m_line );
} 

DistanceToolData* DistanceRepresentation::getDistanceToolData()
{
    return( m_distanceToolData );
} 

void DistanceRepresentation::updateFirstPointLine()
{
    m_line->setFirstPoint( m_distanceToolData->getFirstPoint() );
}

void DistanceRepresentation::updateSecondPointLine()
{
    m_line->setSecondPoint( m_distanceToolData->getSecondPoint() );
}

void DistanceRepresentation::calculateTextAndPositionOfDistance()
{
    m_distanceToolData->calculateDistance();
    m_text->setText( m_distanceToolData->getDistanceText() );
    m_text->setAttatchmentPoint( m_distanceToolData->getTextPosition() );    
}

};  // end namespace udg

