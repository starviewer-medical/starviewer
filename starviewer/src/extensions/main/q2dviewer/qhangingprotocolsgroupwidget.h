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

#ifndef UDG_QHANGINGPROTOCOLSGROUPWIDGET_H
#define UDG_QHANGINGPROTOCOLSGROUPWIDGET_H

#include <QWidget>
#include "ui_qhangingprotocolsgroupwidgetbase.h"

namespace udg {

class HangingProtocol;
class ItemMenu;

/**
 * @brief The QHangingProtocolsGroupWidget class is a widget that shows a group of hanging protocols represented as a grid of icons and a caption for the group.
 */
class QHangingProtocolsGroupWidget : public QWidget, private Ui::QHangingProtocolsGroupWidgetBase
{
    Q_OBJECT
    Q_PROPERTY(QString caption READ getCaption WRITE setCaption)
    Q_PROPERTY(int numberOfColumns MEMBER m_numberOfColumns READ getNumberOfColumns WRITE setNumberOfColumns)

public:
    /// Creates the widget with an empty caption and no items.
    explicit QHangingProtocolsGroupWidget(QWidget *parent = 0);
    virtual ~QHangingProtocolsGroupWidget();

    /// Returns the caption.
    QString getCaption() const;
    /// Sets the caption.
    void setCaption(const QString &caption);

    /// Returns the last set number of columns.
    int getNumberOfColumns() const;
    /// Sets the number of columns that the grid will have the next time that setItems() is called.
    void setNumberOfColumns(int numberOfColumns);

    /// Sets the items to be displayed in the widget and arranges them in a grid with the set number of columns. Previous items are deleted.
    void setItems(const QList<HangingProtocol*> &hangingProtocols);

public slots:
    /// Marks the item corresponding to the given hanging protocol as active and the other items as inactive.
    /// If the parameter is null all items are marked as inactive.
    void setActiveItem(HangingProtocol *activeHangingProtocol);

signals:
    /// Emitted when an item has been selected. The parameter is the identifier of the hanging protocol.
    void selected(int id);

private:
    /// Creates an item for a hanging protocol.
    ItemMenu* createItem(const HangingProtocol *hangingProtocol);
    /// Deletes all the items.
    void clearItems();

private slots:
    /// Emits the selected() signal with data from the given item.
    void emitSelected(ItemMenu *selectedItem);

private:
    /// Number of colums to be used for the grid on the next call to setItems().
    int m_numberOfColumns;

    /// List of hanging protocol items.
    QList<ItemMenu*> m_itemList;

};

} // namespace udg

#endif // UDG_QHANGINGPROTOCOLSGROUPWIDGET_H
