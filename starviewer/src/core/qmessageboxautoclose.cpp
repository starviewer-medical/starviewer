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

#include "qmessageboxautoclose.h"

namespace udg {

QMessageBoxAutoClose::QMessageBoxAutoClose(int secondsToAutoClose, QWidget *parent): QMessageBox(parent)
{
    initialize(secondsToAutoClose);
}

QMessageBoxAutoClose::QMessageBoxAutoClose(int secondsToAutoClose, Icon icon, const QString &title, const QString &text, StandardButtons buttons, QWidget * parent,
                                           Qt::WindowFlags flags) : QMessageBox(icon, title, text, buttons, parent, flags)
{
    initialize(secondsToAutoClose);
}

void QMessageBoxAutoClose::setButtonToShowAutoCloseTimer(QPushButton *buttonToShowTimer)
{
    m_buttonToShowTimer = buttonToShowTimer;
    m_originalTextButtonToShowTimer = buttonToShowTimer->text();

    this->setDefaultButton(buttonToShowTimer);
}

void QMessageBoxAutoClose::showEvent(QShowEvent *event)
{
    m_secondsLeftToShowInButton = m_sencondsToAutoClose;

    m_timerToCloseQMessageBox.setInterval(m_sencondsToAutoClose * 1000);
    m_timerToRefreshButtonText.setInterval(1000);

    updateButtonTextWithSecondsToClose();

    QMessageBox::showEvent(event);

    m_timerToCloseQMessageBox.start();
    m_timerToRefreshButtonText.start();
}

void QMessageBoxAutoClose::hideEvent(QHideEvent *event)
{
    m_timerToCloseQMessageBox.stop();
    m_timerToRefreshButtonText.stop();

    QMessageBox::hideEvent(event);
}

void QMessageBoxAutoClose::initialize(int secondsToAutoClose)
{
    m_sencondsToAutoClose = secondsToAutoClose;
    m_buttonToShowTimer = NULL;

    connect(&m_timerToCloseQMessageBox, SIGNAL(timeout()), SLOT(closeQMessageBox()));
    connect(&m_timerToRefreshButtonText, SIGNAL(timeout()), SLOT(refreshTimerButtonText()));
}

void QMessageBoxAutoClose::updateButtonTextWithSecondsToClose()
{
    if (m_buttonToShowTimer)
    {
        m_buttonToShowTimer->setText(m_originalTextButtonToShowTimer + QString(" (%1)").arg(QString::number(m_secondsLeftToShowInButton)));
    }
}

void QMessageBoxAutoClose::refreshTimerButtonText()
{
    m_secondsLeftToShowInButton--;

    updateButtonTextWithSecondsToClose();
}

void QMessageBoxAutoClose::closeQMessageBox()
{
    m_buttonToShowTimer->click();
}

}
