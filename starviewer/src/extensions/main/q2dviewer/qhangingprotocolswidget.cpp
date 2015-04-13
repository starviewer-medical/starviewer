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

#include "qhangingprotocolswidget.h"

#include "gridicon.h"
#include "itemmenu.h"
#include "hangingprotocol.h"
#include "hangingprotocoldisplayset.h"

#include <QGridLayout>
#include <QLabel>

namespace udg {

const int QHangingProtocolsWidget::MaximumNumberOfColumns = 5;

QHangingProtocolsWidget::QHangingProtocolsWidget(QWidget *parent)
 : QWidget(parent), m_caption(0), m_noHangingProtocolsAvailableLabel(0)
{
    setWindowFlags(Qt::Popup);

    initializeWidget();
}

QHangingProtocolsWidget::~QHangingProtocolsWidget()
{
}

void QHangingProtocolsWidget::initializeWidget()
{
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setSpacing(6);
    m_gridLayout->setMargin(6);

    m_nextHangingProtocolRow = 0;
    m_nextHangingProtocolColumn = 0;

    m_gridLayoutHanging = new QGridLayout();
    m_gridLayoutHanging->setSpacing(6);
    m_gridLayoutHanging->setMargin(6);
    QSpacerItem *spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_gridLayoutHanging->addItem(spacerItem, 0, MaximumNumberOfColumns, 1, 1);

    m_caption = new QLabel(this);
    m_caption->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_caption->setText(tr("Hanging protocols"));
    QVBoxLayout *vBoxLayoutHanging = new QVBoxLayout();
    vBoxLayoutHanging->setMargin(0);
    vBoxLayoutHanging->setSpacing(6);
    vBoxLayoutHanging->addWidget(m_caption);

    m_noHangingProtocolsAvailableLabel = new QLabel(this);
    m_noHangingProtocolsAvailableLabel->setText(tr("No hanging protocols available"));
    m_noHangingProtocolsAvailableLabel->setAlignment(Qt::AlignHCenter);
    m_noHangingProtocolsAvailableLabel->setGeometry(0, 64, 64, 80);
    m_noHangingProtocolsAvailableLabel->setMargin(6);
    vBoxLayoutHanging->addWidget(m_noHangingProtocolsAvailableLabel);

    m_gridLayout->addLayout(vBoxLayoutHanging, 2, 0, 1, 1);
    m_gridLayout->addLayout(m_gridLayoutHanging, 3, 0, 1, 1);
}

ItemMenu* QHangingProtocolsWidget::createIcon(const HangingProtocol *hangingProtocol)
{
    ItemMenu *icon = new ItemMenu(this);
    icon->setData(QString(tr("%1").arg(hangingProtocol->getIdentifier())));
    icon->setGeometry(0, 0, 64, 80);
    icon->setMaximumWidth(64);
    icon->setMinimumWidth(64);
    icon->setMinimumHeight(80);
    icon->setMaximumHeight(80);
    icon->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    QLabel *sizeText = new QLabel(icon);
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

        GridIcon *newIcon = new GridIcon(icon, iconType);

        QStringList listOfPositions = displaySet->getPosition().split("\\");
        double x1 = listOfPositions.value(0).toDouble();
        double y1 = listOfPositions.value(1).toDouble();
        double x2 = listOfPositions.value(2).toDouble();
        double y2 = listOfPositions.value(3).toDouble();

        newIcon->setGeometry(x1 * 64, (1 - y1) * 64, ((x2 - x1) * 64), (y1 - y2) * 64);
        newIcon->show();
    }

    icon->show();
    connect(icon, SIGNAL(isSelected(ItemMenu*)), this, SLOT(emitSelected(ItemMenu*)));
    return icon;
}

void QHangingProtocolsWidget::emitSelected(ItemMenu *selected)
{
    hide();
    emit selectedGrid(selected->getData().toInt());
}

void QHangingProtocolsWidget::dropContent()
{
    foreach (ItemMenu *item, m_itemList)
    {
        m_gridLayoutHanging->removeWidget(item);
        delete item;
    }
    m_itemList.clear();
}

void QHangingProtocolsWidget::setHangingItems(const QList<HangingProtocol*> &listOfCandidates)
{
    dropContent();
    if (listOfCandidates.isEmpty())
    {
        m_noHangingProtocolsAvailableLabel->show();
        m_caption->hide();
    }
    else
    {
        addHangingItems(listOfCandidates);
    }
}

void QHangingProtocolsWidget::addHangingItems(const QList<HangingProtocol*> &items)
{
    int positionRow = 0;
    int positionColumn = 0;

    if (!items.isEmpty())
    {
        m_noHangingProtocolsAvailableLabel->hide();
        m_caption->show();
    }

    foreach (HangingProtocol *hangingProtocol, items)
    {
        ItemMenu *icon = createIcon(hangingProtocol);

        m_gridLayoutHanging->addWidget(icon, positionRow, positionColumn);
        m_itemList.push_back(icon);
        ++positionColumn;

        if (positionColumn == MaximumNumberOfColumns)
        {
            positionColumn = 0;
            ++positionRow;
        }
    }

    m_nextHangingProtocolRow = positionRow;
    m_nextHangingProtocolColumn = positionColumn;
}

}
