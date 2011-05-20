#ifndef QOPACITYTRANSFERFUNCTIONGRAPHICALVIEWLINE_H
#define QOPACITYTRANSFERFUNCTIONGRAPHICALVIEWLINE_H

#include <QGraphicsLineItem>

namespace udg {

class QOpacityTransferFunctionGraphicalViewNode;

/**
    Línia de la vista de funcions de transferència d'opacitat.
  */
class QOpacityTransferFunctionGraphicalViewLine : public QGraphicsLineItem {

public:
    /// Constructor.
    QOpacityTransferFunctionGraphicalViewLine();

    /// Retorna el node de l'esquerra.
    QOpacityTransferFunctionGraphicalViewNode* leftNode() const;
    /// Assigna el node de l'esquerra.
    void setLeftNode(QOpacityTransferFunctionGraphicalViewNode *leftNode);
    /// Retorna el node de la dreta.
    QOpacityTransferFunctionGraphicalViewNode* rightNode() const;
    /// Assigna el node de la dreta.
    void setRightNode(QOpacityTransferFunctionGraphicalViewNode *rightNode);

    /// Actualitza la línia perquè vagi del node de l'esquerra al de la dreta.
    void update();

private:
    /// Node de l'esquerra.
    QOpacityTransferFunctionGraphicalViewNode *m_leftNode;
    /// Node de la dreta
    QOpacityTransferFunctionGraphicalViewNode *m_rightNode;

};

} // End namespace udg

#endif // QOPACITYTRANSFERFUNCTIONGRAPHICALVIEWLINE_H
