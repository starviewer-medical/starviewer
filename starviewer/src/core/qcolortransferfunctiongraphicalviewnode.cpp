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

} // namespace udg