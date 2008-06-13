/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "colorpalette.h"

namespace udg {

ColorPalette::ColorPalette() : grayScaleHighlightColor( Qt::darkGreen ), rgbHighlightColor( Qt::yellow ), grayScaleSelectionColor( Qt::darkGreen ), rgbSelectionColor( 236, 160, 28 ), grayScaleNormalColor( Qt::green ), rgbNormalColor( Qt::green )
{
    m_colorMode = RGB;
}

ColorPalette::ColorPalette( ColorMode mode ) : grayScaleHighlightColor( Qt::darkGreen ), rgbHighlightColor( Qt::yellow ), grayScaleSelectionColor( Qt::darkGreen ), rgbSelectionColor( 236, 160, 28 ), grayScaleNormalColor( Qt::green ), rgbNormalColor( Qt::green )
{
    m_colorMode = mode;
}


QColor ColorPalette::getHighlightColor() const
{
    QColor highlight;

    if ( m_colorMode == RGB )
        highlight = rgbHighlightColor;
    else
        highlight = grayScaleHighlightColor;

    return( highlight );
}

QColor ColorPalette::getSelectionColor() const
{
    QColor selection;

    if ( m_colorMode == RGB )
        selection = rgbSelectionColor;
    else
        selection = grayScaleSelectionColor;

    return( selection );
}

QColor ColorPalette::getNormalColor() const
{
    QColor normal;

    if ( m_colorMode == RGB )
        normal = rgbNormalColor;
    else
        normal = grayScaleNormalColor;

    return( normal );
}

};
