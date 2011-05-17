#include "qopacitytransferfunctiongraphicalviewline.h"

#include "qopacitytransferfunctiongraphicalviewnode.h"

namespace udg {

QOpacityTransferFunctionGraphicalViewLine::QOpacityTransferFunctionGraphicalViewLine()
    : m_leftNode(0), m_rightNode(0)
{
    setZValue(-1.0);
}

QOpacityTransferFunctionGraphicalViewNode* QOpacityTransferFunctionGraphicalViewLine::leftNode() const
{
    return m_leftNode;
}

void QOpacityTransferFunctionGraphicalViewLine::setLeftNode(QOpacityTransferFunctionGraphicalViewNode *leftNode)
{
    m_leftNode = leftNode;
    update();
}

QOpacityTransferFunctionGraphicalViewNode* QOpacityTransferFunctionGraphicalViewLine::rightNode() const
{
    return m_rightNode;
}

void QOpacityTransferFunctionGraphicalViewLine::setRightNode(QOpacityTransferFunctionGraphicalViewNode *rightNode)
{
    m_rightNode = rightNode;
    update();
}

void QOpacityTransferFunctionGraphicalViewLine::update()
{
    if (m_leftNode && m_rightNode)
    {
        setLine(m_leftNode->x(), m_leftNode->y(), m_rightNode->x(), m_rightNode->y());
    }
    else
    {
        setLine(QLineF());
    }
}

} // End namespace udg
