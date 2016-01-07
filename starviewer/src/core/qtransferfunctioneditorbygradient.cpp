/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.

  This file incorporates work covered by the following copyright and
  permission notice:

    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
    Contact: Qt Software Information (qt-info@nokia.com)

    This file is part of the demonstration applications of the Qt Toolkit.

    $QT_BEGIN_LICENSE:LGPL$
    Commercial Usage
    Licensees holding valid Qt Commercial licenses may use this file in
    accordance with the Qt Commercial License Agreement provided with the
    Software or, alternatively, in accordance with the terms contained in
    a written agreement between you and Nokia.

    GNU Lesser General Public License Usage
    Alternatively, this file may be used under the terms of the GNU Lesser
    General Public License version 2.1 as published by the Free Software
    Foundation and appearing in the file LICENSE.LGPL included in the
    packaging of this file. Please review the following information to
    ensure the GNU Lesser General Public License version 2.1 requirements
    will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.

    In addition, as a special exception, Nokia gives you certain
    additional rights. These rights are described in the Nokia Qt LGPL
    Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
    package.

    GNU General Public License Usage
    Alternatively, this file may be used under the terms of the GNU
    General Public License version 3.0 as published by the Free Software
    Foundation and appearing in the file LICENSE.GPL included in the
    packaging of this file. Please review the following information to
    ensure the GNU General Public License version 3.0 requirements will be
    met: http://www.gnu.org/copyleft/gpl.html.

    If you are unsure which license is appropriate for your use, please
    contact the sales department at qt-sales@nokia.com.
    $QT_END_LICENSE$
 *************************************************************************************/

#include "qtransferfunctioneditorbygradient.h"

#include "hoverpoints.h"
#include "logging.h"
#include "shadewidget.h"
#include "transferfunction.h"

#include <QLabel>
#include <QVBoxLayout>

namespace udg {

QTransferFunctionEditorByGradient::QTransferFunctionEditorByGradient(QWidget *parent)
 : QTransferFunctionEditor(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setSpacing(1);
    vbox->setMargin(1);

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->setSpacing(6);
    hbox->setMargin(0);

    QLabel *nameLabel = new QLabel(tr("Name"), this);
    m_nameLineEdit = new QLineEdit(this);

    hbox->addWidget(nameLabel);
    hbox->addWidget(m_nameLineEdit);

    m_red_shade = new ShadeWidget(ShadeWidget::RedShade, this);
    m_green_shade = new ShadeWidget(ShadeWidget::GreenShade, this);
    m_blue_shade = new ShadeWidget(ShadeWidget::BlueShade, this);
    m_alpha_shade = new ShadeWidget(ShadeWidget::ARGBShade, this);

    vbox->addLayout(hbox);
    vbox->addWidget(m_red_shade);
    vbox->addWidget(m_green_shade);
    vbox->addWidget(m_blue_shade);
    vbox->addWidget(m_alpha_shade);

    connect(m_nameLineEdit, SIGNAL(textChanged(const QString&)), SLOT(setTransferFunctionName(const QString&)));
    connect(m_red_shade, SIGNAL(colorsChanged()), SLOT(pointsUpdated()));
    connect(m_green_shade, SIGNAL(colorsChanged()), SLOT(pointsUpdated()));
    connect(m_blue_shade, SIGNAL(colorsChanged()), SLOT(pointsUpdated()));
    connect(m_alpha_shade, SIGNAL(colorsChanged()), SLOT(pointsUpdated()));

    connect(this, SIGNAL(gradientStopsChanged(const QGradientStops&)), SLOT(setTransferFunction(const QGradientStops&)));
}

QTransferFunctionEditorByGradient::~ QTransferFunctionEditorByGradient()
{
}

void QTransferFunctionEditorByGradient::setTransferFunction(const TransferFunction &transferFunction)
{
    if (m_transferFunction == transferFunction)
    {
        return;
    }

    m_nameLineEdit->setText(transferFunction.name());

    QGradientStops gradientStops;

    QList<double> points = transferFunction.keys();

    foreach (double x, points)
    {
        gradientStops << QGradientStop(x / m_maximum, transferFunction.get(x));
    }

    setGradientStops(gradientStops);
    pointsUpdated();
}

const TransferFunction &QTransferFunctionEditorByGradient::getTransferFunction() const
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

    for (int i = 0; i < points.size(); ++i)
    {
        double x = int(points.at(i).x());
        // Si no és l'últim punt i és a la mateixa x que el següent
        if (i < points.size() - 1 && x == points.at(i + 1).x())
        {
            // No es fa res per aquest punt
            continue;
        }
        // Calcula el color del punt
        QColor color((0x00ff0000 & m_red_shade->colorAt(int(x))) >> 16,
                     (0x0000ff00 & m_green_shade->colorAt(int(x))) >> 8,
                     (0x000000ff & m_blue_shade->colorAt(int(x))),
                     (0xff000000 & m_alpha_shade->colorAt(int(x))) >> 24);

        if (x / w > 1)
        {
            return;
        }

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

    for (int i = 0; i < stops.size(); ++i)
    {
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

void QTransferFunctionEditorByGradient::setTransferFunction(const QGradientStops &stops)
{
    m_transferFunction.clear();

    for (int i = 0; i < stops.size(); i++)
    {
        m_transferFunction.set(stops.at(i).first * m_maximum, stops.at(i).second, stops.at(i).second.alphaF());
    }
}

void QTransferFunctionEditorByGradient::setTransferFunctionName(const QString &name)
{
    m_transferFunction.setName(name);
}

}
