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

namespace udg {

QHangingProtocolsWidget::QHangingProtocolsWidget(QWidget *parent)
 : QWidget(parent, Qt::Popup)
{
    setupUi(this);

    // Set initial status
    setItems(QList<HangingProtocol*>(), QList<HangingProtocol*>(), QList<HangingProtocol*>());

    connect(m_combinedGroupWidget, &QHangingProtocolsGroupWidget::selected, this, &QHangingProtocolsWidget::hide);
    connect(m_combinedGroupWidget, &QHangingProtocolsGroupWidget::selected, this, &QHangingProtocolsWidget::selectedCombined);
    connect(m_currentGroupWidget, &QHangingProtocolsGroupWidget::selected, this, &QHangingProtocolsWidget::hide);
    connect(m_currentGroupWidget, &QHangingProtocolsGroupWidget::selected, this, &QHangingProtocolsWidget::selectedCurrent);
    connect(m_currentGroupWidget, &QHangingProtocolsGroupWidget::selected, this, &QHangingProtocolsWidget::selectedGrid);
    connect(m_priorGroupWidget, &QHangingProtocolsGroupWidget::selected, this, &QHangingProtocolsWidget::hide);
    connect(m_priorGroupWidget, &QHangingProtocolsGroupWidget::selected, this, &QHangingProtocolsWidget::selectedPrior);
}

QHangingProtocolsWidget::~QHangingProtocolsWidget()
{
}

void QHangingProtocolsWidget::setItems(const QList<HangingProtocol *> &combined, const QList<HangingProtocol *> &current, const QList<HangingProtocol *> &prior)
{
    m_combinedGroupWidget->setItems(combined);
    m_currentGroupWidget->setItems(current);
    m_priorGroupWidget->setItems(prior);

    m_combinedGroupWidget->setVisible(!combined.isEmpty());
    m_currentGroupWidget->setVisible(!current.isEmpty());
    m_priorGroupWidget->setVisible(!prior.isEmpty());
    m_noHangingProtocolsAvailableLabel->setVisible(combined.isEmpty() && current.isEmpty() && prior.isEmpty());

    // This is needed to make the widget use the minimum possible size
    adjustSize();
}

void QHangingProtocolsWidget::setHangingItems(const QList<HangingProtocol*> &listOfCandidates)
{
    setItems(QList<HangingProtocol*>(), listOfCandidates, QList<HangingProtocol*>());
}

void QHangingProtocolsWidget::setActiveCombinedHangingProtocol(HangingProtocol *hangingProtocol)
{
    m_combinedGroupWidget->setActiveItem(hangingProtocol);
}

void QHangingProtocolsWidget::setActiveCurrentHangingProtocol(HangingProtocol *hangingProtocol)
{
    m_currentGroupWidget->setActiveItem(hangingProtocol);
}

void QHangingProtocolsWidget::setActivePriorHangingProtocol(HangingProtocol *hangingProtocol)
{
    m_priorGroupWidget->setActiveItem(hangingProtocol);
}

}
