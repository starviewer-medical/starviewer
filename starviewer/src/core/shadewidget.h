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

#ifndef UDGSHADEWIDGET_H
#define UDGSHADEWIDGET_H

#include <QWidget>

namespace udg {

class HoverPoints;

class ShadeWidget : public QWidget {
Q_OBJECT
public:
    enum ShadeType {
        RedShade,
        GreenShade,
        BlueShade,
        BlackShade,
        ARGBShade
    };

    ShadeWidget(QWidget *parent);
    ShadeWidget(ShadeType type, QWidget *parent);

    void setGradientStops(const QGradientStops &stops);

    void paintEvent(QPaintEvent *e);

    QSize sizeHint() const
    {
        return QSize(150, 40);
    }
    QPolygonF points() const;
    void setPoints(QPolygonF p);

    HoverPoints* hoverPoints() const
    {
        return m_hoverPoints;
    }

    uint colorAt(int x);

signals:
    void colorsChanged();

private:
    void generateShade();

    ShadeType m_shade_type;
    QImage m_shade;
    HoverPoints *m_hoverPoints;
    QLinearGradient m_alpha_gradient;
};

}

#endif
