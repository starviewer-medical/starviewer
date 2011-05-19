#include "qopacitytransferfunctiongraphicalviewnode.h"

#include "qopacitytransferfunctiongraphicalviewline.h"

#include <QBrush>
#include <QPen>

namespace udg {

const double QOpacityTransferFunctionGraphicalViewNode::Size = 10.0;

QOpacityTransferFunctionGraphicalViewNode::QOpacityTransferFunctionGraphicalViewNode()
    : QGraphicsEllipseItem(-Size / 2.0, -Size / 2.0, Size, Size), m_leftLine(0), m_rightLine(0)
{
    setFlags(ItemIsMovable | ItemIsSelectable | ItemIgnoresTransformations | ItemSendsGeometryChanges);
    setBrush(QColor(127, 127, 127, 200));
    setPen(QColor(0, 0, 0, 255));
}

void QOpacityTransferFunctionGraphicalViewNode::storeOldX()
{
    m_oldX = x();
}

double QOpacityTransferFunctionGraphicalViewNode::oldX() const
{
    return m_oldX;
}

QOpacityTransferFunctionGraphicalViewLine* QOpacityTransferFunctionGraphicalViewNode::leftLine() const
{
    return m_leftLine;
}

void QOpacityTransferFunctionGraphicalViewNode::setLeftLine(QOpacityTransferFunctionGraphicalViewLine *leftLine)
{
    m_leftLine = leftLine;
}

QOpacityTransferFunctionGraphicalViewLine* QOpacityTransferFunctionGraphicalViewNode::rightLine() const
{
    return m_rightLine;
}

void QOpacityTransferFunctionGraphicalViewNode::setRightLine(QOpacityTransferFunctionGraphicalViewLine *rightLine)
{
    m_rightLine = rightLine;
}

QVariant QOpacityTransferFunctionGraphicalViewNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange)
    {
        // value is the new position
        QPointF newPosition = value.toPointF();
        newPosition.setY(qBound(0.0, newPosition.y(), 1.0));
        setToolTip(QString("(%1, %2)").arg(newPosition.x()).arg(newPosition.y()));
        return newPosition;
    }
    else if (change == ItemPositionHasChanged)
    {
        if (m_leftLine && this->x() < m_leftLine->leftNode()->x())
        {
            fixLines(this, m_leftLine->leftNode());
        }
        if (m_rightLine && this->x() > m_rightLine->rightNode()->x())
        {
            fixLines(m_rightLine->rightNode(), this);
        }
        if (m_leftLine)
        {
            m_leftLine->update();
        }
        if (m_rightLine)
        {
            m_rightLine->update();
        }
        return value;
    }
    else
    {
        return QGraphicsItem::itemChange(change, value);
    }
}

void QOpacityTransferFunctionGraphicalViewNode::fixLines(QOpacityTransferFunctionGraphicalViewNode *leftNode,
                                                         QOpacityTransferFunctionGraphicalViewNode *rightNode)
{
    Q_ASSERT(leftNode->x() < rightNode->x());
    Q_ASSERT(leftNode->leftLine() == rightNode->rightLine());

    QOpacityTransferFunctionGraphicalViewLine *leftLine = rightNode->leftLine();
    QOpacityTransferFunctionGraphicalViewLine *centerLine = leftNode->leftLine();
    QOpacityTransferFunctionGraphicalViewLine *rightLine = leftNode->rightLine();

    Q_ASSERT(!leftLine || leftLine->rightNode() == rightNode);
    Q_ASSERT(centerLine->leftNode() == rightNode);
    Q_ASSERT(centerLine->rightNode() == leftNode);
    Q_ASSERT(!rightLine || rightLine->leftNode() == leftNode);

    if (leftLine)
    {
        leftLine->setRightNode(leftNode);
    }
    leftNode->setLeftLine(leftLine);
    leftNode->setRightLine(centerLine);
    centerLine->setLeftNode(leftNode);
    centerLine->setRightNode(rightNode);
    centerLine->update();
    rightNode->setLeftLine(centerLine);
    rightNode->setRightLine(rightLine);
    if (rightLine)
    {
        rightLine->setLeftNode(rightNode);
    }
}

} // End namespace udg
