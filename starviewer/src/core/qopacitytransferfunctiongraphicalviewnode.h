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

#ifndef QOPACITYTRANSFERFUNCTIONGRAPHICALVIEWNODE_H
#define QOPACITYTRANSFERFUNCTIONGRAPHICALVIEWNODE_H

#include <QGraphicsEllipseItem>

namespace udg {

class QOpacityTransferFunctionGraphicalViewLine;

/**
    Node de la vista de funcions de transferència d'opacitat. Guarda l'opacitat i la x inicial d'un moviment.
  */
class QOpacityTransferFunctionGraphicalViewNode : public QGraphicsEllipseItem {

public:
    /// Mida del node.
    static const double Size;

    /// Constructor.
    QOpacityTransferFunctionGraphicalViewNode();

    /// Guarda la x actual com a vella.
    void storeOldX();
    /// Retorna la x vella.
    double oldX() const;

    /// Retorna la línia de l'esquerra.
    QOpacityTransferFunctionGraphicalViewLine* leftLine() const;
    /// Assigna la línia de l'esquerra.
    void setLeftLine(QOpacityTransferFunctionGraphicalViewLine *leftLine);
    /// Retorna la línia de la dreta.
    QOpacityTransferFunctionGraphicalViewLine* rightLine() const;
    /// Assigna la línia de la dreta.
    void setRightLine(QOpacityTransferFunctionGraphicalViewLine *rightLine);

protected:
    /// Gestió de les interaccions de l'usuari amb el node.
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    /// Arregla les línies connectades amb els nodes donats quan aquests s'han intercanviat els costats.
    static void fixLines(QOpacityTransferFunctionGraphicalViewNode *leftNode, QOpacityTransferFunctionGraphicalViewNode *rightNode);

private:
    /// x vella.
    double m_oldX;
    /// Línia de l'esquerra.
    QOpacityTransferFunctionGraphicalViewLine *m_leftLine;
    /// Línia de la dreta.
    QOpacityTransferFunctionGraphicalViewLine *m_rightLine;

};

} // End namespace udg

#endif // QOPACITYTRANSFERFUNCTIONGRAPHICALVIEWNODE_H
