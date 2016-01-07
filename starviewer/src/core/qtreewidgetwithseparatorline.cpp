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

#include "qtreewidgetwithseparatorline.h"

#include <QPainter>

namespace udg {

QTreeWidgetWithSeparatorLine::QTreeWidgetWithSeparatorLine(QWidget* parent)
 : QTreeWidget(parent)
{
    m_sepatorLineColor = QColor(152, 150, 140); 
}

void QTreeWidgetWithSeparatorLine::drawRow(QPainter* painter, const QStyleOptionViewItem &options, const QModelIndex &rowIndex) const
{
    QTreeWidget::drawRow(painter, options, rowIndex);
    bool isFirstRow = rowIndex.row() == 0;

    if (isFirstRow)
    {
        return;
    }

    for (int column = 0; column < this->columnCount(); column++)
    {
        QRect rect = visualRect(rowIndex.sibling(rowIndex.row(), column));
       
        painter->setPen(m_sepatorLineColor);
        //Dibuixem la línia a la part de dalt de la fila
        painter->drawLine(rect.x(), rect.y(), rect.x() + rect.width(), rect.y());
    }
}

QColor QTreeWidgetWithSeparatorLine::getSepatorLineColor() const
{
    return m_sepatorLineColor;
}

void QTreeWidgetWithSeparatorLine::setSeparatorLineColor(const QColor &separatorLineColor)
{
    m_sepatorLineColor = separatorLineColor;
}

}
