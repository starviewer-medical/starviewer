#ifndef UDGCOLORPALETTE_H
#define UDGCOLORPALETTE_H

#include <QColor>

namespace udg {

/**
    Aquesta classe ens implementa una paleta de colors per tal de saber els colors amb que ha de treballar el Drawer.
    Permetrà definir diferents tipus de colors sogons estem treballant amb pantalla monocrom o color.
*/
class ColorPalette {
public:
    /// Aquests enums indiquem quin criteri escollim per ordenar les imatges
    enum ColorMode { Grayscale, RGB };

    ColorPalette();
    ColorPalette(ColorMode mode);
    ~ColorPalette();
    
    /// Ens permet determinar el mode de color que utilitzem
    void setColorMode(ColorMode mode);

    /// Ens retorna el tipus de color que estem usant
    ColorMode getColorMode();
    
    /// Ens retorna el color que usem per a fer highlight1
    QColor getHighlightColor() const;
    
    /// Ens retorna el color que usem per a fer la selecció
    QColor getSelectionColor() const;
    
    /// Ens retorna el color que usem en un estat de normalitat
    QColor getNormalColor() const;

private:
    /// Determina el sistema de color que fem servir
    ColorMode m_colorMode;

    /// Constants definides segons el mode de color. Es definiran en el fitxer d'implementació
    const QColor GrayScaleHighlightColor;
    const QColor RGBHighlightColor;
    
    const QColor GrayScaleSelectionColor;
    const QColor RGBSelectionColor;
    
    const QColor GrayScaleNormalColor;
    const QColor RGBNormalColor;
};

};  // end namespace udg

#endif
