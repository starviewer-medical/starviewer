/****************************************************************************
**
** Copyright (C) 2005-2006 Trolltech AS. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/


/***************************************************************************
 *   Copyright (C) 2006-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include <QtGui>

#include "qtransferfunctioneditorbygradient.h"
#include "shadewidget.h"
#include "hoverpoints.h"

#include "transferfunction.h"


namespace udg {


QTransferFunctionEditorByGradient::QTransferFunctionEditorByGradient( QWidget * parent )
    : QTransferFunctionEditor( parent )
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setSpacing(1);
    vbox->setMargin(1);

    m_red_shade = new ShadeWidget(ShadeWidget::RedShade, this);
    m_green_shade = new ShadeWidget(ShadeWidget::GreenShade, this);
    m_blue_shade = new ShadeWidget(ShadeWidget::BlueShade, this);
    m_alpha_shade = new ShadeWidget(ShadeWidget::ARGBShade, this);

    vbox->addWidget(m_red_shade);
    vbox->addWidget(m_green_shade);
    vbox->addWidget(m_blue_shade);
    vbox->addWidget(m_alpha_shade);

    connect(m_red_shade, SIGNAL(colorsChanged()), this, SLOT(pointsUpdated()));
    connect(m_green_shade, SIGNAL(colorsChanged()), this, SLOT(pointsUpdated()));
    connect(m_blue_shade, SIGNAL(colorsChanged()), this, SLOT(pointsUpdated()));
    connect(m_alpha_shade, SIGNAL(colorsChanged()), this, SLOT(pointsUpdated()));

    connect( this, SIGNAL( gradientStopsChanged(const QGradientStops&) ), SLOT( setTransferFunction(const QGradientStops&) ) );
}


QTransferFunctionEditorByGradient::~ QTransferFunctionEditorByGradient()
{
}


void QTransferFunctionEditorByGradient::setTransferFunction( const TransferFunction & transferFunction )
{
    if ( m_transferFunction == transferFunction ) return;

    QGradientStops gradientStops;

    QList< double > points = transferFunction.getPoints();

    foreach ( double x, points )
    {
        gradientStops << QGradientStop( x / 255.0, transferFunction.get( x ) );
    }

    setGradientStops( gradientStops );
}


const TransferFunction & QTransferFunctionEditorByGradient::getTransferFunction() const
{
    return m_transferFunction;
}


inline static bool x_less_than(const QPointF &p1, const QPointF &p2)
{
    return p1.x() < p2.x();
}


void QTransferFunctionEditorByGradient::pointsUpdated()
{
    double w = m_alpha_shade->width();

    QGradientStops stops;

    QPolygonF points;

    points += m_red_shade->points();
    points += m_green_shade->points();
    points += m_blue_shade->points();
    points += m_alpha_shade->points();

    qSort(points.begin(), points.end(), x_less_than);

    for (int i=0; i<points.size(); ++i) {
        double x = int(points.at(i).x());
        if (i < points.size() - 1 && x == points.at(i+1).x())    // si no és l'últim punt i és a la mateixa x que el següent
            continue;    // no es fa res per aquest punt
        QColor color((0x00ff0000 & m_red_shade->colorAt(int(x))) >> 16,
                     (0x0000ff00 & m_green_shade->colorAt(int(x))) >> 8,
                     (0x000000ff & m_blue_shade->colorAt(int(x))),
                     (0xff000000 & m_alpha_shade->colorAt(int(x))) >> 24);    // calcula el color del punt

        if (x / w > 1)
            return;

        stops << QGradientStop(x / w, color);
    }

    m_alpha_shade->setGradientStops(stops);

    emit gradientStopsChanged(stops);
}


static void set_shade_points(const QPolygonF &points, ShadeWidget *shade)
{
    shade->hoverPoints()->setPoints(points);
    shade->hoverPoints()->setPointLock(0, HoverPoints::LockToLeft);
    shade->hoverPoints()->setPointLock(points.size() - 1, HoverPoints::LockToRight);
    shade->update();
}

void QTransferFunctionEditorByGradient::setGradientStops(const QGradientStops &stops)
{
    QPolygonF pts_red, pts_green, pts_blue, pts_alpha;

    double h_red = m_red_shade->height();
    double h_green = m_green_shade->height();
    double h_blue = m_blue_shade->height();
    double h_alpha = m_alpha_shade->height();

    for (int i=0; i<stops.size(); ++i) {
        double pos = stops.at(i).first;
        QRgb color = stops.at(i).second.rgba();
        pts_red << QPointF(pos * m_red_shade->width(), h_red - qRed(color) * h_red / 255);
        pts_green << QPointF(pos * m_green_shade->width(), h_green - qGreen(color) * h_green / 255);
        pts_blue << QPointF(pos * m_blue_shade->width(), h_blue - qBlue(color) * h_blue / 255);
        pts_alpha << QPointF(pos * m_alpha_shade->width(), h_alpha - qAlpha(color) * h_alpha / 255);
    }

    set_shade_points(pts_red, m_red_shade);
    set_shade_points(pts_green, m_green_shade);
    set_shade_points(pts_blue, m_blue_shade);
    set_shade_points(pts_alpha, m_alpha_shade);

}


void QTransferFunctionEditorByGradient::setTransferFunction( const QGradientStops & stops )
{
    m_transferFunction.clear();

    for ( unsigned char i = 0; i < stops.size(); i++ )
    {
        m_transferFunction.addPoint( stops.at( i ).first * 255.0, stops.at( i ).second );
    }
}


}
