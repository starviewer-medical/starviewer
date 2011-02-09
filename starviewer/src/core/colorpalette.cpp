/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "colorpalette.h"

namespace udg {

ColorPalette::ColorPalette()
 : GrayScaleHighlightColor(Qt::darkGreen), RGBHighlightColor(Qt::yellow), GrayScaleSelectionColor(Qt::darkGreen), RGBSelectionColor(236, 160, 28), GrayScaleNormalColor(Qt::green), RGBNormalColor(Qt::green)
{
    m_colorMode = RGB;
}

ColorPalette::ColorPalette(ColorMode mode)
 : GrayScaleHighlightColor(Qt::darkGreen), RGBHighlightColor(Qt::yellow), GrayScaleSelectionColor(Qt::darkGreen), RGBSelectionColor(236, 160, 28), GrayScaleNormalColor(Qt::green), RGBNormalColor(Qt::green)
{
    m_colorMode = mode;
}

ColorPalette::~ColorPalette()
{
}

void ColorPalette::setColorMode(ColorMode mode)
{
    m_colorMode = mode;
}

ColorPalette::ColorMode ColorPalette::getColorMode()
{
    return(m_colorMode); 
}

QColor ColorPalette::getHighlightColor() const
{
    QColor highlight;

    if (m_colorMode == RGB)
    {
        highlight = RGBHighlightColor;
    }
    else
    {
        highlight = GrayScaleHighlightColor;
    }

    return(highlight);
}

QColor ColorPalette::getSelectionColor() const
{
    QColor selection;

    if (m_colorMode == RGB)
    {
        selection = RGBSelectionColor;
    }
    else
    {
        selection = GrayScaleSelectionColor;
    }

    return(selection);
}

QColor ColorPalette::getNormalColor() const
{
    QColor normal;

    if (m_colorMode == RGB)
    {
        normal = RGBNormalColor;
    }
    else
    {
        normal = GrayScaleNormalColor;
    }

    return(normal);
}

};
