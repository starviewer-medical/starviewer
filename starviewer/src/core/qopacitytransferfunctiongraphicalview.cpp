#include "qopacitytransferfunctiongraphicalview.h"

#include "opacitytransferfunction.h"
#include "qopacitytransferfunctiongraphicalviewline.h"
#include "qopacitytransferfunctiongraphicalviewnode.h"

#include <cmath>

#include <QWheelEvent>

namespace udg {

QOpacityTransferFunctionGraphicalView::QOpacityTransferFunctionGraphicalView(QWidget *parent)
    : QGraphicsView(parent)
{
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    setScene(new QGraphicsScene(this));
    scale(1.0, -1.0);
    QGraphicsView::fitInView(0.0, 0.0, 1.0, 1.0);
}

void QOpacityTransferFunctionGraphicalView::setOpacityTransferFunction(const OpacityTransferFunction &opacityTransferFunction)
{
    scene()->clear();

    QList<double> keys = opacityTransferFunction.keys();
    QRectF rect;
    bool first = true;
    QOpacityTransferFunctionGraphicalViewNode *previousNode = 0;

    foreach (double x, keys)
    {
        double opacity = opacityTransferFunction.get(x);
        QOpacityTransferFunctionGraphicalViewNode *node = new QOpacityTransferFunctionGraphicalViewNode();
        node->setX(x);
        node->setY(opacity);
        node->setToolTip(QString("(%1, %2)").arg(x).arg(opacity));
        scene()->addItem(node);

        if (previousNode)
        {
            QOpacityTransferFunctionGraphicalViewLine *line = new QOpacityTransferFunctionGraphicalViewLine();
            line->setLeftNode(previousNode);
            line->setRightNode(node);
            previousNode->setRightLine(line);
            node->setLeftLine(line);
            line->update();
            scene()->addItem(line);
        }

        if (first)
        {
            first = false;
            rect.setLeft(x);
            rect.setRight(x);
            rect.setTop(opacity);
            rect.setBottom(opacity);
        }
        else
        {
            if (x < rect.left())
            {
                rect.setLeft(x);
            }
            if (x > rect.right())
            {
                rect.setRight(x);
            }
            if (opacity < rect.top())
            {
                rect.setTop(opacity);
            }
            if (opacity > rect.bottom())
            {
                rect.setBottom(opacity);
            }
        }

        previousNode = node;
    }

    fitInView(rect);
}

void QOpacityTransferFunctionGraphicalView::wheelEvent(QWheelEvent *event)
{
    double scale = pow(2.0, event->delta() / 240.0);

    if (event->modifiers().testFlag(Qt::ShiftModifier)) // zoom vertical
    {
        this->scale(1.0, scale);
    }
    else    // zoom horitzontal
    {
        this->scale(scale, 1.0);
    }
}

void QOpacityTransferFunctionGraphicalView::fitInView(const QRectF &rect)
{
    QRectF sceneRect(rect.x() - 0.1 * rect.width(), rect.y() - 0.1 * rect.height(), 1.2 * rect.width(), 1.2 * rect.height());
    scene()->setSceneRect(sceneRect);

    // cridem el mètode del pare per ajustar el zoom
    QGraphicsView::fitInView(rect);
}

} // namespace udg
