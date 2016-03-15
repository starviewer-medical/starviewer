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

#include "qhangingprotocolsgroupwidget.h"

#include "gridicon.h"
#include "griditerator.h"
#include "hangingprotocol.h"
#include "hangingprotocoldisplayset.h"
#include "itemmenu.h"

namespace udg {

QHangingProtocolsGroupWidget::QHangingProtocolsGroupWidget(QWidget *parent)
    : QWidget(parent), m_numberOfColumns(5)
{
    setupUi(this);
}

QHangingProtocolsGroupWidget::~QHangingProtocolsGroupWidget()
{
}

QString QHangingProtocolsGroupWidget::getCaption() const
{
    return m_caption->text();
}

void QHangingProtocolsGroupWidget::setCaption(const QString &caption)
{
    m_caption->setText(caption);
}

int QHangingProtocolsGroupWidget::getNumberOfColumns() const
{
    return m_numberOfColumns;
}

void QHangingProtocolsGroupWidget::setNumberOfColumns(int numberOfColumns)
{
    m_numberOfColumns = numberOfColumns;
}

void QHangingProtocolsGroupWidget::setItems(const QList<HangingProtocol*> &hangingProtocols)
{
    clearItems();

    GridIterator iterator(hangingProtocols.size(), m_numberOfColumns);

    foreach (HangingProtocol *hangingProtocol, hangingProtocols)
    {
        ItemMenu *item = createItem(hangingProtocol);
        m_layout->addWidget(item, iterator.row(), iterator.column());
        m_itemList.append(item);
        iterator.next();
    }

    // This is needed to make the widget use the minimum possible size
    adjustSize();
}

void QHangingProtocolsGroupWidget::setActiveItem(HangingProtocol *activeHangingProtocol)
{
    foreach (ItemMenu *item, m_itemList)
    {
        if (activeHangingProtocol && activeHangingProtocol->getIdentifier() == item->getData().toInt())
        {
            item->setSelected(true);
        }
        else
        {
            item->setSelected(false);
        }
    }
}

ItemMenu* QHangingProtocolsGroupWidget::createItem(const HangingProtocol *hangingProtocol)
{
    ItemMenu *item = new ItemMenu(this);
    item->setData(QString::number(hangingProtocol->getIdentifier()));
    item->setFixedSize(64, 80);

    QLabel *sizeText = new QLabel(item);
    sizeText->setText(hangingProtocol->getName());
    sizeText->setAlignment(Qt::AlignHCenter);
    sizeText->setGeometry(0, 64, 64, 80);

    foreach (HangingProtocolDisplaySet *displaySet, hangingProtocol->getDisplaySets())
    {
        QString iconType = displaySet->getIconType();

        if (iconType.isEmpty())
        {
            iconType = hangingProtocol->getIconType();
        }

        GridIcon *newIcon = new GridIcon(iconType, item);

        QStringList listOfPositions = displaySet->getPosition().split("\\");
        double x1 = listOfPositions.value(0).toDouble();
        double y1 = listOfPositions.value(1).toDouble();
        double x2 = listOfPositions.value(2).toDouble();
        double y2 = listOfPositions.value(3).toDouble();

        newIcon->setGeometry(x1 * 64, (1 - y1) * 64, ((x2 - x1) * 64), (y1 - y2) * 64);
        newIcon->show();
    }

    connect(item, &ItemMenu::isSelected, this, &QHangingProtocolsGroupWidget::emitSelected);

    return item;
}

void QHangingProtocolsGroupWidget::clearItems()
{
    foreach (ItemMenu *item, m_itemList)
    {
        m_layout->removeWidget(item);
        delete item;
    }

    m_itemList.clear();
}

void QHangingProtocolsGroupWidget::emitSelected(ItemMenu *selectedItem)
{
    emit selected(selectedItem->getData().toInt());
}

} // namespace udg
