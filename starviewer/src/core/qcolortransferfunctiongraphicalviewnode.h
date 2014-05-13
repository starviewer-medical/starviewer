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
