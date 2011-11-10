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
    m_secondsLeftToClose = m_sencondsToAutoClose;

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
        m_buttonToShowTimer->setText(m_originalTextButtonToShowTimer + QString(" (%1)").arg(QString::number(m_secondsLeftToClose)));
    }
}

void QMessageBoxAutoClose::refreshTimerButtonText()
{
    m_secondsLeftToClose--;

    updateButtonTextWithSecondsToClose();
}

void QMessageBoxAutoClose::closeQMessageBox()
{
    m_buttonToShowTimer->click();
}

}
