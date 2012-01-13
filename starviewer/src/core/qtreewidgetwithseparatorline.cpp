#include "qtreewidgetwithseparatorline.h"

#include <QPainter>

namespace udg
{
QTreeWidgetWithSeparatorLine::QTreeWidgetWithSeparatorLine(QWidget* parent) : QTreeWidget(parent)
{
    setContentsMargins( 0, 0, 0, 0);
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
        QRect rect = visualRect(rowIndex.sibling( rowIndex.row(), column));
       
        painter->setPen(m_sepatorLineColor);
        //Dibuixem la línia a la part de dalt de la fila
        painter->drawLine( rect.x(), rect.y(), rect.x() + rect.width(), rect.y());
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