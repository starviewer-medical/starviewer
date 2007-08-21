/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "distancerepresentation.h"
#include "text.h"
#include "line.h"

namespace udg {

DistanceRepresentation::DistanceRepresentation() : Representation()
{
    m_line = new Line();
    m_text = new Text();
}


DistanceRepresentation::DistanceRepresentation( Line *line, Text *text ) : Representation()
{ 
    m_line = line;
    m_text = text;
}
    
DistanceRepresentation::~DistanceRepresentation()
{
    delete m_line;
    delete m_text;
}    

void DistanceRepresentation::setText( Text *text )
{
    m_text = text;
}
    
void DistanceRepresentation::setLine( Line *line )
{
    m_line = line;
}
    
Text* DistanceRepresentation::getText()
{
    return( m_text );   
}
    
Line* DistanceRepresentation::getLine()
{
    return( m_line );
} 

};  // end namespace udg

