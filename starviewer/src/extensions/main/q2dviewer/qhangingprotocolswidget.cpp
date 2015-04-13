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

#include "qhangingprotocolsgroupwidget.h"

namespace udg {

const int QHangingProtocolsWidget::MaximumNumberOfColumns = 5;

QHangingProtocolsWidget::QHangingProtocolsWidget(QWidget *parent)
 : QWidget(parent), m_noHangingProtocolsAvailableLabel(0)
{
    setWindowFlags(Qt::Popup);

    initializeWidget();
}

QHangingProtocolsWidget::~QHangingProtocolsWidget()
{
}

void QHangingProtocolsWidget::initializeWidget()
{
    QVBoxLayout *vBoxLayoutHanging = new QVBoxLayout(this);
    vBoxLayoutHanging->setMargin(6);
    vBoxLayoutHanging->setSpacing(6);

    m_hangingProtocolsGroupWidget = new QHangingProtocolsGroupWidget(this);
    m_hangingProtocolsGroupWidget->setCaption(tr("Hanging protocols"));
    m_hangingProtocolsGroupWidget->setNumberOfColumns(MaximumNumberOfColumns);
    connect(m_hangingProtocolsGroupWidget, &QHangingProtocolsGroupWidget::selected, this, &QHangingProtocolsWidget::hide);
    connect(m_hangingProtocolsGroupWidget, &QHangingProtocolsGroupWidget::selected, this, &QHangingProtocolsWidget::selectedGrid);
    vBoxLayoutHanging->addWidget(m_hangingProtocolsGroupWidget);

    m_noHangingProtocolsAvailableLabel = new QLabel(this);
    m_noHangingProtocolsAvailableLabel->setText(tr("No hanging protocols available"));
    m_noHangingProtocolsAvailableLabel->setAlignment(Qt::AlignHCenter);
    m_noHangingProtocolsAvailableLabel->setGeometry(0, 64, 64, 80);
    m_noHangingProtocolsAvailableLabel->setMargin(6);
    vBoxLayoutHanging->addWidget(m_noHangingProtocolsAvailableLabel);
}

void QHangingProtocolsWidget::setHangingItems(const QList<HangingProtocol*> &listOfCandidates)
{
    m_hangingProtocolsGroupWidget->setItems(listOfCandidates);

    if (listOfCandidates.isEmpty())
    {
        m_hangingProtocolsGroupWidget->hide();
        m_noHangingProtocolsAvailableLabel->show();
    }
    else
    {
        m_hangingProtocolsGroupWidget->show();
        m_noHangingProtocolsAvailableLabel->hide();
    }
}

}
