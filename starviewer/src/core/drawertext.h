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
    void setText( QString text );
    
    ///ens retorna el text que conté
    QString getText();
    
    ///permet canviar el punt d'ancoratge
    void setAttatchmentPoint( double point[3] );
    
    ///retorna el punt d'ancoratge
    double* getAttatchmentPoint();
    
    ///activa el dibuixat del requadre de la casella de text
    void borderOn();
    
    ///desactiva el dibuixat del requadre de la casella de text
    void borderOff();
    
    ///activa / desactiva el dibuixat del requadre de la casella de text segons el paràmetre que li passem
    void borderEnabled( bool enabled );
    
    ///ens diu si s'ha de dibuixar el requadre o no.
    bool isBorderEnabled();
    
    ///assigna l'espaiat entre el text i la caixa contenidora
    void setPadding( int padding );
    
    ///ens retona l'espaiat entre el text i la caixa contenidora
    int getPadding();
    
    ///ens retorna el tipus de font
    QString getFontFamily();
    
    ///ens permet assignar el tipus de font
    void setFontFamily( QString family );
    
    ///assigna el tamany de la font 
    void setFontSize( int size );
    
    ///ens retona el tamany de la font
    int getFontSize();
    
    ///activa la negreta del text
    void boldOn();
    
    ///desactiva la negreta del text
    void boldOff();
    
    ///activa / desactiva la negreta del text
    void bold( bool bold );
    
    ///ens diu si està activada la negreta del text
    bool isBold();
    
    ///activa la cursiva del text
    void italicOn();
    
    ///desactiva la cursiva del text
    void italicOff();
    
    ///activa / desactiva la cursiva del text
    void italic( bool italic );
    
    ///ens diu si està activada la cursiva del text
    bool isItalic();
    
    ///activa l'ombra del text
    void shadowOn();
    
    ///desactiva l'ombra del text
    void shadowOff();
    
    ///activa / desactiva l'ombra del text
    void shadow( bool shadow );
    
    ///ens diu si està activada l'ombra del text
    bool hasShadow();
    
    ///ens permet assignar el tipus de justificació horitzontal
    void setHorizontalJustification( QString horizontalJustification );
    
    ///ens retorna el tipus de justificació horitzontal
    QString getHorizontalJustification();
    
    ///ens permet assignar el tipus de justificació vertical
    void setVerticalJustification( QString verticalJustification );
    
    ///ens retorna el tipus de justificació vertical
    QString getVerticalJustification();
    
    ///fa que el text s'escali
    void scaledTextOn();
    
    ///fa que el text no s'escali
    void scaledTextOff();
    
    ///ens permet determinar si el text s'escala o no
    void scaledText( bool scaled );
    
    ///ens diu si el text és escalat o no
    bool isTextScaled();
    
    ///calcula la distància que té respecte al punt passat per paràmetre
    double getDistanceToPoint( double *point3D );
    
    ///mira si està dins dels límits que marquen els punts
    bool isInsideOfBounds( double p1[3], double p2[3], int view );

public slots:
    void update( int representation );

protected slots:
    void updateVtkProp();

private:
    /// Mètode intern per transformar les propietats de la primitiva a propietats de vtk
    void updateVtkActorProperties();

private:
    /// Text a mostrar
    QString m_text;

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

    /// Estructures de vtk, per construir el text
    vtkCaptionActor2D *m_vtkActor;

    /// Punt on es mostrara el text
    double m_attatchPoint[3];
};

}

#endif
