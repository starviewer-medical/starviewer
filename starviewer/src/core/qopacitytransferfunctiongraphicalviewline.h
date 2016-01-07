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
