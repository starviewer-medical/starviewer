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