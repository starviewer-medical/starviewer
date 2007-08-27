/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "text.h"

namespace udg {

Text::Text() 
    : DrawingPrimitive(), m_borderEnabled( false ), m_padding( 0 ), m_fontFamily( "Arial" ), m_fontSize( 12 ), m_bold( false ), m_italic( false ), m_shadow( false ), m_scaledText( false ), m_horizontalJustification( "Centered" ), m_verticalJustification( "Centered" )
{
    setHeight( 0.05 );
    setWidth( 0.09 );
}


Text::Text( double point[3], QString text ) 
    : DrawingPrimitive(), m_borderEnabled( false ), m_padding( 0 ), m_fontFamily( "Arial" ), m_fontSize( 12 ), m_bold( false ), m_italic( false ), m_shadow( false ), m_scaledText( false ), m_horizontalJustification( "Centered" ), m_verticalJustification( "Centered" )
{ 
    setAttatchmentPoint( point );
    setHeight( 0.05 );
    setWidth( 0.09 );
    setText( text );
}
    
void Text::setAttatchmentPoint( double point[3] )
{
    for ( int i = 0; i < 3; i++ )
        m_attatchmentPoint[i] = point[i];
}

void Text::refreshText()
{
    emit  textChanged( this );
}

};  // end namespace udg

