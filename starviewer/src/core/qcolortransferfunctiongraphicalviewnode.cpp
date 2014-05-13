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
 *************************************************************************************/

#include "qcolortransferfunctiongraphicalviewnode.h"

#include "qcolortransferfunctiongraphicalview.h"

#include <QBrush>
#include <QPen>

namespace udg {

QColorTransferFunctionGraphicalViewNode::QColorTransferFunctionGraphicalViewNode(QColorTransferFunctionGraphicalView *view)
 : QGraphicsRectItem(-2.5, -16.0, 5.0, 32.0), m_view(view)
{
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
    setBrush(QColor(0, 0, 0, 127));
    setPen(QColor(255, 255, 255, 255));
}

const QColor& QColorTransferFunctionGraphicalViewNode::color() const
{
    return m_color;
}

void QColorTransferFunctionGraphicalViewNode::setColor(const QColor &color)
{
    m_color = color;
}

void QColorTransferFunctionGraphicalViewNode::storeOldX()
{
    m_oldX = x();
}

double QColorTransferFunctionGraphicalViewNode::oldX() const
{
    return m_oldX;
}

QVariant QColorTransferFunctionGraphicalViewNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange)
    {
        // value is the new position
        QPointF newPosition = value.toPointF();
        newPosition.setY(0.0);
        setToolTip(QString("%1").arg(newPosition.x()));
        return newPosition;
    }
    else if (change == ItemPositionHasChanged)
    {
        m_view->requestBackgroundUpdate();
        return value;
    }
    else
    {
        return QGraphicsItem::itemChange(change, value);
    }
}

} // End namespace udg
