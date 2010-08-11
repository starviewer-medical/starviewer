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
 *
 *  Primitiva de text per al Drawer
 *
 */
class DrawerText : public DrawerPrimitive {
Q_OBJECT
public:
    DrawerText(QObject *parent = 0);
    ~DrawerText();

    vtkProp* getAsVtkProp();

    /// Permet assignar el text
    void setText(const QString &text);
    
    /// Ens retorna el text que conté
    QString getText();
    
    /// Permet canviar el punt d'ancoratge
    void setAttachmentPoint(double point[3]);
    
    /// Retorna el punt d'ancoratge
    double* getAttachmentPoint();
    
    /// Activa el dibuixat del requadre de la casella de text
    void borderOn();
    
    /// Desactiva el dibuixat del requadre de la casella de text
    void borderOff();
    
    /// Activa / desactiva el dibuixat del requadre de la casella de text segons el paràmetre que li passem
    void borderEnabled(bool enabled);
    
    /// Ens diu si s'ha de dibuixar el requadre o no.
    bool isBorderEnabled();
    
    /// Assigna l'espaiat entre el text i la caixa contenidora
    void setPadding(int padding);
    
    /// Ens retona l'espaiat entre el text i la caixa contenidora
    int getPadding();
    
    /// Ens retorna el tipus de font
    QString getFontFamily();
    
    /// Ens permet assignar el tipus de font
    void setFontFamily(const QString &family);
    
    /// Assigna el tamany de la font 
    void setFontSize(int size);
    
    /// Ens retona el tamany de la font
    int getFontSize();
    
    /// Activa la negreta del text
    void boldOn();
    
    /// Desactiva la negreta del text
    void boldOff();
    
    /// Activa / desactiva la negreta del text
    void bold(bool bold);
    
    /// Ens diu si està activada la negreta del text
    bool isBold();
    
    /// Activa la cursiva del text
    void italicOn();
    
    /// Desactiva la cursiva del text
    void italicOff();
    
    /// Activa / desactiva la cursiva del text
    void italic(bool italic);
    
    /// Ens diu si està activada la cursiva del text
    bool isItalic();
    
    /// Activa l'ombra del text
    void shadowOn();
    
    /// Desactiva l'ombra del text
    void shadowOff();
    
    /// Activa / desactiva l'ombra del text
    void setShadow(bool shadow);
    
    /// Ens diu si està activada l'ombra del text
    bool hasShadow();
    
    /// Ens permet assignar el tipus de justificació horitzontal
    void setHorizontalJustification(const QString &horizontalJustification);
    
    /// Ens retorna el tipus de justificació horitzontal
    QString getHorizontalJustification();
    
    /// Ens permet assignar el tipus de justificació vertical
    void setVerticalJustification(const QString &verticalJustification);
    
    /// Ens retorna el tipus de justificació vertical
    QString getVerticalJustification();
    
    /// Fa que el text s'escali
    void scaledTextOn();
    
    /// Fa que el text no s'escali
    void scaledTextOff();
    
    /// Ens permet determinar si el text s'escala o no
    void scaledText(bool scaled);
    
    /// Ens diu si el text és escalat o no
    bool isTextScaled();
    
    /// Calcula la distància que té respecte al punt passat per paràmetre
    double getDistanceToPoint(double *point3D);

    void getBounds(double bounds[6]);

public slots:
    void update();

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

    /// Família de la font
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

    /// Punt on es mostrarà el text
    double m_attachPoint[3];
};

}

#endif
