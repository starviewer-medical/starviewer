/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDRAWERTEXT_H
#define UDGDRAWERTEXT_H

#include "drawerprimitive.h"

class vtkCaptionActor2D;

namespace udg {

/**
Primitiva de text per al Drawer

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DrawerText : public DrawerPrimitive
{
Q_OBJECT
public:
    DrawerText(QObject *parent = 0);

    ~DrawerText();

    vtkProp *getAsVtkProp();

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
    { return( m_attatchPoint ); }
    
    ///activa el dibuixat del requadre de la casella de text
    void borderOn()
    { borderEnabled( true ); }
    
    ///desactiva el dibuixat del requadre de la casella de text
    void borderOff()
    { borderEnabled( false ); }
    
    ///activa / desactiva el dibuixat del requadre de la casella de text segons el paràmetre que li passem
    void borderEnabled( bool enabled )
    { m_border = enabled; }
    
    ///ens diu si s'ha de dibuixar el requadre o no.
    bool isBorderEnabled()
    { return( m_border ); }
    
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
    { m_scaled = scaled; }
    
    ///ens diu si el text és escalat o no
    bool isTextScaled()
    { return( m_scaled ); }

protected slots:
    void updateVtkProp();

private:
    /// Mètode intern per transformar les propietats de la primitiva a propietats de vtk
    void updateVtkActorProperties();

private:
    /// Text a mostrar
    QString m_text;
    
    /// Punt on es mostrara el text
    double m_attatchPoint[3];

    /// Justificació horitzontal
    QString m_horizontalJustification;

    /// Justificació vertical
    QString m_verticalJustification;

    /// Contorn del text
    bool m_border;

    /// Familia de la font
    QString m_fontFamily;

    /// Mida de la font
    int m_fontSize;

    /// Ombra
    bool m_shadow;

    /// Lletres en cursiva
    bool m_italic;

    /// Lletres en negreta
    bool m_bold;

    /// Alçada
    double m_height;

    /// Amplada
    double m_width;

    /// Text escalat
    bool m_scaled;

    /// Espaiat
    int m_padding;

    /// Color de la font
    QColor m_color;

    /// Estructures de vtk, per construir el text
    vtkCaptionActor2D *m_vtkActor;
};

}

#endif
