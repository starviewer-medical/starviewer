#include "qopacitytransferfunctiongraphicalview.h"

#include "opacitytransferfunction.h"
#include "qopacitytransferfunctiongraphicalviewline.h"
#include "qopacitytransferfunctiongraphicalviewnode.h"

#include <cmath>
#include <typeinfo>

#include <QMouseEvent>
#include <QWheelEvent>
#include <QtConcurrentFilter>

namespace {

/// Retorna cert si item és un QOpacityTransferFunctionGraphicalViewNode.
bool isNode(const QGraphicsItem *item)
{
    return typeid(*item) == typeid(udg::QOpacityTransferFunctionGraphicalViewNode);
}

/// Retorna cert si la x d'item1 és més petita que la x d'item2.
bool lessThan(const QGraphicsItem *item1, const QGraphicsItem *item2)
{
    return item1->x() < item2->x();
}

} // namespace

namespace udg {

QOpacityTransferFunctionGraphicalView::QOpacityTransferFunctionGraphicalView(QWidget *parent)
    : QGraphicsView(parent), m_state(Ready)
{
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setRenderHint(QPainter::Antialiasing);

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

void QOpacityTransferFunctionGraphicalView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);

    if (m_state != Ready)
    {
        DEBUG_LOG(QString("Mouse press inesperat (estat %1)").arg(m_state));
        return;
    }

    if (event->button() == Qt::LeftButton)
    {
        if (scene()->selectedItems().isEmpty()) // si no hi ha items seleccionats
        {
            m_state = Adding;
            QOpacityTransferFunctionGraphicalViewNode *node = addNode(event->pos());
            // Després d'afegir el node a la vista volem poder moure'l abans d'afegir-lo definitivament a la funció.
            // Per fer això necessitem que QGraphicsView comenci la interacció amb el node com si haguéssim clicat damunt d'ell des del principi.
            // Per aconseguir-ho creem un nou esdeveniment com el que estem processant però amb la posició actualitzada.
            // Això cal perquè el node no s'afegeix sempre just a sota el cursor perquè l'opacitat està limitada entre 0 i 1.
            // Llavors fem que QGraphicsView processi aquest nou esdeveniment i l'enganyem perquè faci el que volem (trollface.jpg).
            // De pas també posem el cursor a sobre el node.
            QPoint position = mapFromScene(node->pos());
            QCursor::setPos(mapToGlobal(position));
            QMouseEvent mouseEvent(event->type(), position, event->button(), event->buttons(), event->modifiers());
            QGraphicsView::mousePressEvent(&mouseEvent);
        }
        else
        {
            //beginMoveNodes();
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        m_state = Removing;
    }
}

void QOpacityTransferFunctionGraphicalView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
}

void QOpacityTransferFunctionGraphicalView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);

    if (m_state == Adding)
    {
        QGraphicsItem *node = scene()->selectedItems().first();
        emit nodeAdded(node->x(), node->y());
        m_state = Ready;
    }
    else if (m_state == Removing)
    {
        removeNode(event->pos());
        m_state = Ready;
    }
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
    QRectF sceneRect(rect.x() - 0.05 * rect.width(), rect.y() - 0.05 * rect.height(), 1.1 * rect.width(), 1.1 * rect.height());
    scene()->setSceneRect(sceneRect);

    // cridem el mètode del pare per ajustar el zoom
    QGraphicsView::fitInView(rect);
}

QOpacityTransferFunctionGraphicalViewNode* QOpacityTransferFunctionGraphicalView::addNode(const QPoint &position)
{
    QPointF scenePosition = mapToScene(position);
    QOpacityTransferFunctionGraphicalViewNode *node = new QOpacityTransferFunctionGraphicalViewNode();  // (n*)
    node->setPos(scenePosition);
    node->setToolTip(QString("(%1, %2").arg(scenePosition.x()).arg(scenePosition.y()));
    scene()->addItem(node);

    // Busquem els nodes anterior i posterior
    QList<QGraphicsItem*> items = this->items();    // tots els items (nodes i línies)
    QtConcurrent::blockingFilter(items, isNode);    // filtrem per quedar-nos només amb els nodes
    qSort(items.begin(), items.end(), lessThan);    // ordenem per x
    QList<QGraphicsItem*>::const_iterator it = qBinaryFind(items.begin(), items.end(), node, lessThan); // busquem el nou node (ja l'hem inserit abans)
    Q_ASSERT(it != items.end());
    Q_ASSERT(*it == node);
    QOpacityTransferFunctionGraphicalViewNode *leftNode = 0;    // (ln)
    QOpacityTransferFunctionGraphicalViewNode *rightNode = 0;   // (rn)
    if (it != items.begin())    // existeix un node a l'esquerra
    {
        leftNode = dynamic_cast<QOpacityTransferFunctionGraphicalViewNode*>(*(it - 1));
        Q_ASSERT(leftNode);
    }
    if (it + 1 != items.end())  // existeix un node a la dreta
    {
        rightNode = dynamic_cast<QOpacityTransferFunctionGraphicalViewNode*>(*(it + 1));
        Q_ASSERT(rightNode);
    }

    // Busquem les línies anterior i posterior
    QOpacityTransferFunctionGraphicalViewLine *leftLine = 0;    // (ll)
    QOpacityTransferFunctionGraphicalViewLine *rightLine = 0;   // (rl)
    if (leftNode && rightNode)              // ?-(ln)--ll--(rn)-? ==> ?-(ln)--ll--(n*)--rl*--(rn)-?
    {
        Q_ASSERT(leftNode->rightLine());
        Q_ASSERT(leftNode->rightLine()->rightNode() == rightNode);
        Q_ASSERT(rightNode->leftLine());
        Q_ASSERT(rightNode->leftLine()->leftNode() == leftNode);
        leftLine = leftNode->rightLine();
        rightLine = new QOpacityTransferFunctionGraphicalViewLine();
        scene()->addItem(rightLine);
    }
    else if (leftNode)  // && !rightNode    // ?-(ln) ==> ?-(ln)--ll*--(n*)
    {
        Q_ASSERT(!leftNode->rightLine());
        leftLine = new QOpacityTransferFunctionGraphicalViewLine();
        scene()->addItem(leftLine);
    }
    else if (rightNode) // && !leftNode     // (rn)-? ==> (n*)--rl*--(rn)-?
    {
        Q_ASSERT(!rightNode->leftLine());
        rightLine = new QOpacityTransferFunctionGraphicalViewLine();
        scene()->addItem(rightLine);
    }

    // Enllacem bé els nodes i les línies
    if (leftLine)
    {
        leftNode->setRightLine(leftLine);
        leftLine->setLeftNode(leftNode);
        leftLine->setRightNode(node);
        node->setLeftLine(leftLine);
    }
    if (rightLine)
    {
        node->setRightLine(rightLine);
        rightLine->setLeftNode(node);
        rightLine->setRightNode(rightNode);
        rightNode->setLeftLine(rightLine);
    }

    return node;
}

void QOpacityTransferFunctionGraphicalView::removeNode(const QPoint &position)
{
    // Busquem el node més proper a position i que sigui prou a prop
    QList<QGraphicsItem*> items = this->items(position);    // tots els items (nodes i línies) a position
    QtConcurrent::blockingFilter(items, isNode);    // filtrem per quedar-nos només amb els nodes
    if (items.isEmpty())
    {
        return;
    }
    QOpacityTransferFunctionGraphicalViewNode *node;
    if (items.size() == 1)
    {
        node = dynamic_cast<QOpacityTransferFunctionGraphicalViewNode*>(items.first());
    }
    else
    {
        qSort(items.begin(), items.end(), lessThan);    // ordenem per x
        QOpacityTransferFunctionGraphicalViewNode *auxiliarNode = new QOpacityTransferFunctionGraphicalViewNode();  // node auxiliar per fer la cerca
        auxiliarNode->setPos(mapToScene(position));
        QList<QGraphicsItem*>::const_iterator it = qLowerBound(items.begin(), items.end(), auxiliarNode, lessThan); // busquem el node més proper a x
        delete auxiliarNode;
        if (it == items.begin())
        {
            node = dynamic_cast<QOpacityTransferFunctionGraphicalViewNode*>(*it);
        }
        else if (it == items.end())
        {
            node = dynamic_cast<QOpacityTransferFunctionGraphicalViewNode*>(*(it - 1));
        }
        else
        {
            double x = mapToScene(position).x();
            if (qAbs(x - (*it)->x()) < qAbs(x - (*(it - 1))->x()))
            {
                node = dynamic_cast<QOpacityTransferFunctionGraphicalViewNode*>(*it);
            }
            else
            {
                node = dynamic_cast<QOpacityTransferFunctionGraphicalViewNode*>(*(it - 1));
            }
        }
    }

    // Refem els enllaços entre els nodes i les línies restants
    if (node->leftLine() && node->rightLine())              // ?-(ln)--ll--(n/)--rl/--(rn)-? ==> ?-(ln)--ll--(rn)-?
    {
        node->leftLine()->setRightNode(node->rightLine()->rightNode());
        node->rightLine()->rightNode()->setLeftLine(node->leftLine());
        scene()->removeItem(node->rightLine());
        delete node->rightLine();
    }
    else if (node->leftLine())  // && !node->rightLine()    // ?-(ln)--ll/--(n/) ==> ?-(ln)
    {
        node->leftLine()->leftNode()->setRightLine(0);
        scene()->removeItem(node->leftLine());
        delete node->leftLine();
    }
    else if (node->rightLine()) // && !node->leftLine()     // (n/)--rl/--(rn)-? ==> (rn)-?
    {
        node->rightLine()->rightNode()->setLeftLine(0);
        scene()->removeItem(node->rightLine());
        delete node->rightLine();
    }

    // Esborrem el node
    scene()->removeItem(node);
    emit nodeRemoved(node->x());
    delete node;
}

} // namespace udg
