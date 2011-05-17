#ifndef QOPACITYTRANSFERFUNCTIONGRAPHICALVIEW_H
#define QOPACITYTRANSFERFUNCTIONGRAPHICALVIEW_H

#include <QGraphicsView>

namespace udg {

class OpacityTransferFunction;
class QOpacityTransferFunctionGraphicalViewNode;

/**
    Vista gràfica d'una funció de transferència d'opacitat.
 */
class QOpacityTransferFunctionGraphicalView : public QGraphicsView {

    Q_OBJECT

public:

    /// Constructor.
    explicit QOpacityTransferFunctionGraphicalView(QWidget *parent = 0);

    /// Assigna la funció de transferència d'opacitat.
    void setOpacityTransferFunction(const OpacityTransferFunction &opacityTransferFunction);

signals:

    /// S'emet quan s'afegeix un node.
    void nodeAdded(double x, double opacity);
    /// S'emet quan s'esborra un node.
    void nodeRemoved(double x);
    /// S'emet quan es mou un node.
    //void nodeMoved(double origin, double destination);
    /// S'emet quan es mou ms d'un node.
    //void nodesMoved(const QList<double> &origins, double offset);
    /// S'emet quan es canvia l'opacitat d'un node.
    //void nodeChangedOpacity(double x, double opacity);

protected:

    ///@{
    /// Gestió d'esdeveniments. \todo Explicar una mica què fem a cadascun.
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    //virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    ///@}

private:

    /// Estat que ens indica que estem fent entre el mouse press i el mouse release.
    enum State { Ready, Adding, Removing };

    Q_DISABLE_COPY(QOpacityTransferFunctionGraphicalView)

    void fitInView(const QRectF &rect);

    /// Afegeix un nou node i el retorna.
    QOpacityTransferFunctionGraphicalViewNode* addNode(const QPoint &position);
    /// Esborra un node a position si existeix.
    void removeNode(const QPoint &position);
    /// Fa els preparatius per comenar a moure nodes.
    //void beginMoveNodes();
    /// Finalitza el moviment de nodes.
    //void endMoveNodes();
    /// Canvia l'opacitat del node a x si existeix.
    //void changeNodeOpacity(double x);

private:

    /// Estat actual.
    State m_state;

    /// Nivell de zoom actual.
    //double m_zoom;
    /// Indica si s'ha d'actualitzar el fons desprs de moure el ratol.
    //bool m_backgroundUpdateRequested;

};

} // End namespace udg

#endif // QOPACITYTRANSFERFUNCTIONGRAPHICALVIEW_H
