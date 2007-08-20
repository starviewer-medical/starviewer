/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTEXT_H
#define UDGTEXT_H

#include "drawingprimitive.h"

namespace udg {

/**
Classe que implementa la primitiva gràfica de tipus caixa de text. Hereta de la classe DrawPrimitive.

@author Grup de Gràfics de Girona  ( GGG )
*/

class Text : public DrawingPrimitive{
Q_OBJECT
public:
    
    /**propietats especials que aporta el text:
        -voltant de l'àrea de text
        -distància entre el text i la caixa contenidora
        -tipus de font per al text
        -tamany de la font
        -negreta 
        -cursiva
        -ombra
        -justificació horitzontal del text
        -justificació vertical del text
        -text és escalat
     */
    
    ///constructor per defecte
    Text();
    
    ///constructor amb paràmetres: passem el punt d'ancoratge
    Text( double point[3], QString text );
    
    ~Text(){}
    
    ///permet assignar el text
    void setText( QString text )
    { m_text = text; }
    
    ///ens retorna el text que conté
    QString getText()
    { return( m_text ); }
    
    ///permet canviar el punt d'ancoratge
    void setAttatchmentPoint( double point[3] );
    
    ///retorna el punt d'ancoratge
    double* getAttatchmentPoint()
    { return( m_attatchmentPoint ); }
    
    ///activa el dibuixat del requadre de la casella de text
    void borderOn()
    { borderEnabled( true ); }
    
    ///desactiva el dibuixat del requadre de la casella de text
    void borderOff()
    { borderEnabled( false ); }
    
    ///activa / desactiva el dibuixat del requadre de la casella de text segons el paràmetre que li passem
    void borderEnabled( bool enabled )
    { m_borderEnabled = enabled; }
    
    ///ens diu si s'ha de dibuixar el requadre o no.
    bool isBorderEnabled()
    { return( m_borderEnabled ); }
    
    ///assigna l'espaiat entre el text i la caixa contenidora
    void setPadding( int padding )
    { m_padding = padding; }
    
    ///ens retona l'espaiat entre el text i la caixa contenidora
    int getPadding()
    { return( m_padding ); }
    
    ///ens retorna el tipus de font
    QString getFontFamily() 
    { return m_fontFamily; }
    
    ///ens permet assignar el tipus de font
    void setFontFamily( QString family ) 
    { m_fontFamily = family; }
    
    ///assigna el tamany de la font 
    void setFontSize( int size )
    { m_fontSize = size; }
    
    ///ens retona el tamany de la font
    int getFontSize()
    { return( m_fontSize ); }
    
    ///activa la negreta del text
    void boldOn()
    { bold( true ); }
    
    ///desactiva la negreta del text
    void boldOff()
    { bold( false ); }
    
    ///activa / desactiva la negreta del text
    void bold( bool bold )
    { m_bold = bold; }
    
    ///ens diu si està activada la negreta del text
    bool isBold()
    { return( m_bold ); }
    
    ///activa la cursiva del text
    void italicOn()
    { italic( true ); }
    
    ///desactiva la cursiva del text
    void italicOff()
    { italic( false ); }
    
    ///activa / desactiva la cursiva del text
    void italic( bool italic )
    { m_italic = italic; }
    
    ///ens diu si està activada la cursiva del text
    bool isItalic()
    { return( m_italic ); }
    
    ///activa l'ombra del text
    void shadowOn()
    { shadow( true ); }
    
    ///desactiva l'ombra del text
    void shadowOff()
    { shadow( false ); }
    
    ///activa / desactiva l'ombra del text
    void shadow( bool shadow )
    { m_shadow = shadow; }
    
    ///ens diu si està activada l'ombra del text
    bool hasShadow()
    { return( m_shadow ); }
    
    ///ens permet assignar el tipus de justificació horitzontal
    void setHorizontalJustification( QString horizontalJustification )
    { m_horizontalJustification = horizontalJustification; }
    
    ///ens retorna el tipus de justificació horitzontal
    QString getHorizontalJustification()
    { return( m_horizontalJustification ); }
    
    ///ens permet assignar el tipus de justificació vertical
    void setVerticalJustification( QString verticalJustification )
    { m_verticalJustification = verticalJustification; }
    
    ///ens retorna el tipus de justificació vertical
    QString getVerticalJustification()
    { return( m_verticalJustification ); }
    
    ///fa que el text s'escali
    void scaledTextOn()
    { scaledText( true ); }
    
    ///fa que el text no s'escali
    void scaledTextOff()
    { scaledText( false ); }
    
    ///ens permet determinar si el text s'escala o no
    void scaledText( bool scaled )
    { m_scaledText = scaled; }
    
    ///ens diu si el text és escalat o no
    bool isTextScaled()
    { return( m_scaledText ); }
    
    QString getPrimitiveType()
    { return( "Text" ); }
    
private:
    
    ///Text que contindrà
    QString m_text;
    
    ///punt d'ancoratge de la línia
    double m_attatchmentPoint[3];
    
    ///ens determina si el el requadre del text s'ha de mostrar
    bool m_borderEnabled;
    
    ///determina l'espaiat entre el text i la caixa contenidora
    int m_padding;
    
    ///Tipus de font per al text
    QString m_fontFamily;
    
    ///Tamany de la font
    int m_fontSize;
    
    ///determina si s'ha de mostrar en negreta o no
    bool m_bold;
    
    ///determina si s'ha de mostrar en cursiva o no
    bool m_italic;
    
    ///determina si el text té ombra o no
    bool m_shadow;
    
    ///justificació horitzontal del text
    QString m_horizontalJustification;
    
    ///justificació vertical del text
    QString m_verticalJustification;
    
    ///ens determina si el text és escalat o no
    bool m_scaledText;
};

};  //  end  namespace udg

#endif
