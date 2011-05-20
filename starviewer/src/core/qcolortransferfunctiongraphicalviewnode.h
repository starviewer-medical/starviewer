#ifndef QCOLORTRANSFERFUNCTIONGRAPHICALVIEWNODE_H
#define QCOLORTRANSFERFUNCTIONGRAPHICALVIEWNODE_H

#include <QGraphicsRectItem>

namespace udg {

class QColorTransferFunctionGraphicalView;

/**
    Node de la vista de funcions de transferència de color. Guarda el color i la x inicial d'un moviment.
  */
class QColorTransferFunctionGraphicalViewNode : public QGraphicsRectItem {

public:
    /// Crea el node i li passa la vista que el conté.
    QColorTransferFunctionGraphicalViewNode(QColorTransferFunctionGraphicalView *view);

    /// Retorna el color.
    const QColor& color() const;
    /// Assigna el color.
    void setColor(const QColor &color);
    /// Guarda la x actual com a vella.
    void storeOldX();
    /// Retorna la x vella.
    double oldX() const;

protected:
    /// Gestió de les interaccions de l'usuari amb el node.
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    /// Vista que conté el node.
    QColorTransferFunctionGraphicalView *m_view;
    /// x vella.
    double m_oldX;
    /// Color.
    QColor m_color;

};

} // End namespace udg

#endif // QCOLORTRANSFERFUNCTIONGRAPHICALVIEWNODE_H
