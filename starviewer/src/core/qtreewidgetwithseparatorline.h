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

#ifndef UDGQTREEWITHSEPARATORLINE_H
#define UDGQTREEWITHSEPARATORLINE_H

#include <QTreeWidget>
#include <QColor>

namespace udg {

/** Aquesta classe implementa un QTreeWidget que dibuixa una línia com separador dels elements del QTreeWidget
  */
class QTreeWidgetWithSeparatorLine : public QTreeWidget {
Q_OBJECT

public:
    /// Constructor
    QTreeWidgetWithSeparatorLine(QWidget *parent = 0);

    /// Dibuixa una fila del QTreeWidget afegint una línia com a separador amb la fila precendent. 
    /// Per la primera fila del QTreeWidget no es dibuixa separador
    void drawRow(QPainter* painter, const QStyleOptionViewItem &options, const QModelIndex &rowIndex) const;

    /// Assigna/Obté el color de la línia que fa de separador. Per defecte és d'un to gris
    QColor getSepatorLineColor() const;
    void setSeparatorLineColor(const QColor &separatorLineColor);

private:
    QColor m_sepatorLineColor;
};

}

#endif
