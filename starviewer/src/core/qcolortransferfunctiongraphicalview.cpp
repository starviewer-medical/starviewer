#include "qcolortransferfunctiongraphicalview.h"

#include "colortransferfunction.h"
#include "qcolortransferfunctiongraphicalviewnode.h"

#include <cmath>

#include <QColorDialog>
#include <QMouseEvent>
#include <QWheelEvent>

namespace udg {

QColorTransferFunctionGraphicalView::QColorTransferFunctionGraphicalView(QWidget *parent)
    : QGraphicsView(parent), m_zoom(1.0), m_backgroundUpdateRequested(false)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setMaximumHeight(48);
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setScene(new QGraphicsScene(this));
}

void QColorTransferFunctionGraphicalView::setColorTransferFunction(const ColorTransferFunction &colorTransferFunction)
{
    scene()->clear();

    QList<double> keys = colorTransferFunction.keys();

    foreach (double x, keys)
    {
        // Punt
        QColorTransferFunctionGraphicalViewNode *node = new QColorTransferFunctionGraphicalViewNode(this);
        node->setX(x);
        node->setToolTip(QString("%1").arg(x));
        node->setColor(colorTransferFunction.get(x));
        scene()->addItem(node);
//        QBrush brush;
//        brush.setStyle(Qt::SolidPattern);
//        brush.setColor(colorTransferFunction(x));
//        QGraphicsRectItem *item = m_scene->addRect(-2.0, -20.0, 4.0, 20.0, QPen(colorTransferFunction.get(x)), brush);
    }

    updateBackground();
}

void QColorTransferFunctionGraphicalView::requestBackgroundUpdate()
{
    m_backgroundUpdateRequested = true;
}

void QColorTransferFunctionGraphicalView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);

    if (event->button() == Qt::LeftButton)
    {
        if (scene()->selectedItems().isEmpty())
        {
            addNode(mapToScene(event->pos()).x());
        }
        else
        {
            beginMoveNodes();
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        removeNode(mapToScene(event->pos()).x());
    }
}

void QColorTransferFunctionGraphicalView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);

    if (m_backgroundUpdateRequested)
    {
        updateBackground();
    }
}

void QColorTransferFunctionGraphicalView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);

    if (event->button() == Qt::LeftButton)
    {
        endMoveNodes();
    }
}

void QColorTransferFunctionGraphicalView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QGraphicsView::mouseDoubleClickEvent(event);

    if (event->button() == Qt::LeftButton)
    {
        changeNodeColor(mapToScene(event->pos()).x());
    }
}

void QColorTransferFunctionGraphicalView::wheelEvent(QWheelEvent *event)
{
    double scale = pow(2.0, event->delta() / 240.0);
    this->scale(scale, 1.0);
    m_zoom *= scale;
    double antiScale = 1.0 / m_zoom;

    QList<QGraphicsItem*> nodes = scene()->items();

    foreach (QGraphicsItem *node, nodes)
    {
        // Si només fem servir escalats es pot fer així, sinó es pot afegir un segon paràmetre a setTransform
        node->setTransform(QTransform::fromScale(antiScale, 1.0));
    }
}

// Tenir en compte z value, selected items...
void QColorTransferFunctionGraphicalView::updateBackground()
{
    QLinearGradient background(scene()->sceneRect().left(), 0.0, scene()->sceneRect().right(), 0.0);
    double shift = -scene()->sceneRect().left();
    double scale = 1.0 / scene()->sceneRect().width();
    QList<QGraphicsItem*> nodes = items();

    foreach (QGraphicsItem *item, nodes)
    {
        QColorTransferFunctionGraphicalViewNode *node = dynamic_cast<QColorTransferFunctionGraphicalViewNode*>(item);
        background.setColorAt((node->x() + shift) * scale, node->color());
    }

    scene()->setBackgroundBrush(background);
    m_backgroundUpdateRequested = false;
}

void QColorTransferFunctionGraphicalView::addNode(double x)
{
    QColor color = QColorDialog::getColor(Qt::black, this);

    if (color.isValid())
    {
        QColorTransferFunctionGraphicalViewNode *node = new QColorTransferFunctionGraphicalViewNode(this);
        node->setX(x);
        node->setToolTip(QString("%1").arg(x));
        node->setColor(color);
        // Si només fem servir escalats es pot fer així, sinó es pot afegir un segon paràmetre a setTransform
        node->setTransform(QTransform::fromScale(1.0 / m_zoom, 1.0));
        scene()->addItem(node);
        updateBackground();
        emit nodeAdded(x, color);
    }
}

void QColorTransferFunctionGraphicalView::removeNode(double x)
{
    // TODO: es podria refinar agafant tots els que siguin aquí i triant el més proper a x
    QGraphicsItem *item = scene()->itemAt(x, 0.0);

    if (item)
    {
        x = item->x();
        scene()->removeItem(item);
        updateBackground();
        emit nodeRemoved(x);
    }
}

void QColorTransferFunctionGraphicalView::beginMoveNodes()
{
    QList<QGraphicsItem*> selectedNodes = scene()->selectedItems();

    foreach (QGraphicsItem *item, selectedNodes)
    {
        QColorTransferFunctionGraphicalViewNode *node = dynamic_cast<QColorTransferFunctionGraphicalViewNode*>(item);
        node->storeOldX();
        // Amb això fem que a updateBackground() els tinguem en l'ordre que ens interessa (els que s'estan movent seran els últims)
        node->setZValue(-1.0);
    }
}

void QColorTransferFunctionGraphicalView::endMoveNodes()
{
    QList<QGraphicsItem*> selectedNodes = scene()->selectedItems();
    QList<double> origins;
    double offset = 0.0;

    foreach (QGraphicsItem *item, selectedNodes)
    {
        QColorTransferFunctionGraphicalViewNode *node = dynamic_cast<QColorTransferFunctionGraphicalViewNode*>(item);
        // Restaurem el Z-value predeterminat
        node->setZValue(0.0);

        if (node->x() != node->oldX())
        {
            origins.append(node->oldX());
            double difference = node->x() - node->oldX();
            // L'offset hauria de ser el mateix per tots els moviments
            Q_ASSERT(offset == 0.0 || qFuzzyCompare(offset, difference));
            offset = difference;
        }
    }

    if (origins.size() == 1)
    {
        emit nodeMoved(origins.first(), origins.first() + offset);
    }
    else if (origins.size() >= 2)
    {
        emit nodesMoved(origins, offset);
    }

    // Potser s'hauria de fer un m_scene->setSceneRect(...) aquí, per ajustar el tros visible
    // o potser seria millor posar una manera de fer-ho manualment, per començar
}

void QColorTransferFunctionGraphicalView::changeNodeColor(double x)
{
    // TODO: es podria refinar agafant tots els que siguin aquí i triant el més proper a x
    QGraphicsItem *item = scene()->itemAt(x, 0.0);

    if (item)
    {
        x = item->x();
        QColorTransferFunctionGraphicalViewNode *node = dynamic_cast<QColorTransferFunctionGraphicalViewNode*>(item);
        QColor color = QColorDialog::getColor(node->color(), this);

        if (color.isValid())
        {
            node->setColor(color);
            updateBackground();
            emit nodeChangedColor(x, color);
        }
    }
}

} // End namespace udg
