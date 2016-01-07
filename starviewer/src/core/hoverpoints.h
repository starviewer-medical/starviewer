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

#ifndef UDGHOVERPOINTS_H
#define UDGHOVERPOINTS_H

#include <QObject>

#include <QPen>
#include <QPolygonF>
#include <QWidget>

namespace udg {

class HoverPoints : public QObject {
Q_OBJECT
public:
    enum PointShape {
        CircleShape,
        RectangleShape,
        NoShape
    };

    enum LockType {
        LockToLeft   = 0x01,
        LockToRight  = 0x02,
        LockToTop    = 0x04,
        LockToBottom = 0x08
    };

    enum SortType {
        NoSort,
        XSort,
        YSort
    };

    enum ConnectionType {
        NoConnection,
        LineConnection,
        CurveConnection
    };

    HoverPoints(QWidget *widget, PointShape shape);

    bool eventFilter(QObject *object, QEvent *event);

    void paintPoints();

    inline QRectF boundingRect() const;
    void setBoundingRect(const QRectF &boundingRect)
    {
        m_bounds = boundingRect;
    }

    QPolygonF points() const
    {
        return m_points;
    }
    void setPoints(const QPolygonF &points);

    QSizeF pointSize() const
    {
        return m_pointSize;
    }
    void setPointSize(const QSizeF &size)
    {
        m_pointSize = size;
    }

    SortType sortType() const
    {
        return m_sortType;
    }
    void setSortType(SortType sortType)
    {
        m_sortType = sortType;
    }

    ConnectionType connectionType() const
    {
        return m_connectionType;
    }
    void setConnectionType(ConnectionType connectionType)
    {
        m_connectionType = connectionType;
    }

    void setConnectionPen(const QPen &pen)
    {
        m_connectionPen = pen;
    }
    void setShapePen(const QPen &pen)
    {
        m_pointPen = pen;
    }
    void setShapeBrush(const QBrush &brush)
    {
        m_pointBrush = brush;
    }

    void setPointLock(int pos, LockType lock)
    {
        m_locks[pos] = lock;
    }

    void setEditable(bool editable)
    {
        m_editable = editable;
    }
    bool editable() const
    {
        return m_editable;
    }

public slots:
    void setEnabled(bool enabled);
    void setDisabled(bool disabled)
    {
        setEnabled(!disabled);
    }

signals:
    void pointsChanged(const QPolygonF &points);

public:
    void firePointChange();

private:
    inline QRectF pointBoundingRect(int i) const;
    inline void movePoint(int i, const QPointF &newPos, bool emitChange = true);

    QWidget *m_widget;

    QPolygonF m_points;
    QRectF m_bounds;
    PointShape m_shape;
    SortType m_sortType;
    ConnectionType m_connectionType;

    QVector<uint> m_locks;

    QSizeF m_pointSize;
    int m_currentIndex;
    bool m_editable;
    bool m_enabled;

    QPen m_pointPen;
    QBrush m_pointBrush;
    QPen m_connectionPen;

    int m_width, m_height;
};

inline QRectF HoverPoints::pointBoundingRect(int i) const
{
    QPointF p = m_points.at(i);
    double w = m_pointSize.width();
    double h = m_pointSize.height();
    double x = p.x() - w / 2;
    double y = p.y() - h / 2;
    return QRectF(x, y, w, h);
}

inline QRectF HoverPoints::boundingRect() const
{
    if (m_bounds.isEmpty())
    {
        return m_widget->rect();
    }
    else
    {
        return m_bounds;
    }
}

}

#endif
