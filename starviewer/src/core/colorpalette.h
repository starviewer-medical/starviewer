/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCOLORPALETTE_H
#define UDGCOLORPALETTE_H

#include <QColor>

namespace udg {

/**
    Aquesta classe ens implementa una paleta de colors per tal de saber els colors amb que ha de treballar el Drawer.
    Permetrà definir diferents tipus de colors sogons estem treballant amb pantalla monocrom o color.

    @author Grup de Gràfics de Girona  ( GGG )
*/
class ColorPalette{
public:

    /// Aquests enums indiquem quin criteri escollim per ordenar les imatges
    enum ColorMode{ GRAYSCALE, RGB };

    ///constructor per defecte
    ColorPalette();
    
    ///constructor amb paràmetre
    ColorPalette( ColorMode mode );
    
    ~ColorPalette(){}
    
    ///ens permet determinar el mode de color que utilitzem
    void setColorMode( ColorMode mode )
    { m_colorMode = mode; }

    ///ens retorna el tipus de color que estem usant
    ColorMode getColorMode()
    { return( m_colorMode ); }
    
    ///ens retorna el color que usem per a fer highlight1
    QColor getHighlightColor() const;
    
    ///ens retorna el color que usem per a fer la selecció
    QColor getSelectionColor() const;
    
    ///ens retorna el color que usem en un estat de normalitat
    QColor getNormalColor() const;
private:
    ///Determina el sistema de color que fem servir
    ColorMode m_colorMode;

    ///constants definides segons el mode de color. Es definiran en el fitxer d'implementació
    const QColor grayScaleHighlightColor;
    const QColor rgbHighlightColor;
    
    const QColor grayScaleSelectionColor;
    const QColor rgbSelectionColor;
    
    const QColor grayScaleNormalColor;
    const QColor rgbNormalColor;
};

};  // end namespace udg

#endif
