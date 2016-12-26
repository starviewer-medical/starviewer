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

#include "qnotificationpopup.h"

#include <QTimer>
#include <QMovie>
#include <QDesktopWidget>

#include "settings.h"
#include "systeminformation.h"

namespace udg {

const int QNotificationPopup::MoveAnimationDelayInMilliSeconds = 5000;

QNotificationPopup::QNotificationPopup(QWidget *parent)
 : QDialog(parent)
{
    setupUi(this);
    this->setWindowFlags(Qt::SubWindow | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    
    setupOngoingOperationAnimation();
    setupTimers();
    setupHideAnimation();
    setupMoveAnimation();
    
    checkFadeOutEffectCanBeEnabled();

    // HACK This is the only way found at this time to achieve text is not being cut when its size is bigger
    // The dialogh should be enhanced to avoid this hack and take care about the current method used to place it because now it's a bit tricky
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);

    // Put an EventFilter to the Widget and GroupBox in order to hide it when the user clicks it.
    // This way, the user can hide it if it bothers him
    this->installEventFilter(this);
    m_groupBox->installEventFilter(this);

    setHideDelay(5000);
    setHeadline(QString());
    setOngoingOperationText(QString());
}

QNotificationPopup::~QNotificationPopup()
{
}

void QNotificationPopup::setHeadline(const QString &text)
{
    m_headlineLabel->setText(text);
}

void QNotificationPopup::setOngoingOperationText(const QString &text)
{
    m_ongoingOperationLabel->setText(text);
}

void QNotificationPopup::showOngoingOperationAnimation()
{
    m_ongoingOperationAnimationMovie->start();
    m_progressAnimationLabel->show();
}

void QNotificationPopup::hideOngoingOperationAnimation()
{
    m_ongoingOperationAnimationMovie->stop();
    m_progressAnimationLabel->hide();
}

void QNotificationPopup::cancelTriggeredAnimations()
{
    m_hideDelayTimer->stop();
    m_hideAnimation.stop();

    m_moveAnimationDelayTimer->stop();
    m_moveAnimation.stop();
}

void QNotificationPopup::setHideDelay(int delayInMs)
{
    if (delayInMs < 0)
    {
        return;
    }
    
    m_hideDelayInMilliSeconds = delayInMs;
}

void QNotificationPopup::hideWithDelay()
{
    m_hideDelayTimer->start(m_hideDelayInMilliSeconds);
}

void QNotificationPopup::startMoveAnimation(bool delayed)
{
    int delay = 0;
    if (delayed)
    {
        delay = MoveAnimationDelayInMilliSeconds;
    }

    m_moveAnimationDelayTimer->start(delay);
}

bool QNotificationPopup::isMoveAnimationOnDelayPeriod()
{
    return m_moveAnimationDelayTimer->isActive();
}

void QNotificationPopup::showEvent(QShowEvent *)
{
    // Popup is placed at the center of the main window screen
    // TODO Tricky way for picking interface settings geometry, should be done in a better manner
    Settings settings;
    QWidget fakeMainWindow;

    settings.restoreGeometry(QString("geometry"), &fakeMainWindow);

    this->move(QApplication::desktop()->screenGeometry(&fakeMainWindow).center() - this->rect().center());

    startMoveAnimation(true);
}

bool QNotificationPopup::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        // Parem els rellotges perquè no saltin les animacions amb el PopUp amagat, sinó ens podríem trobar que si rebem una altra petició
        // aparegués el PopUp movent-se
        // TODO Replace by cancelTriggeredAnimations()?
        m_hideDelayTimer->stop();
        m_moveAnimationDelayTimer->stop(); 

        hideMe();
        return true;
    }
    else
    {
        return false;
    }
}

void QNotificationPopup::setupOngoingOperationAnimation()
{
    m_ongoingOperationAnimationMovie = new QMovie(this);
    m_ongoingOperationAnimationMovie->setFileName(":/images/animations/loader.gif");
    m_progressAnimationLabel->setMovie(m_ongoingOperationAnimationMovie);
    
    hideOngoingOperationAnimation();
}

void QNotificationPopup::setupTimers()
{
    m_hideDelayTimer = new QTimer(this);
    m_hideDelayTimer->setSingleShot(true);
    
    m_moveAnimationDelayTimer = new QTimer(this);
    m_moveAnimationDelayTimer->setSingleShot(true);
    
    connect(m_hideDelayTimer, SIGNAL(timeout()), SLOT(hideMe()));
    connect(m_moveAnimationDelayTimer, SIGNAL(timeout()), SLOT(runMoveAnimation()));
}

void QNotificationPopup::setupHideAnimation()
{
    m_hideAnimation.setTargetObject(this);
    m_hideAnimation.setPropertyName("windowOpacity");
    m_hideAnimation.setDuration(1000);
    m_hideAnimation.setEndValue(0.0);

    connect(&m_hideAnimation, SIGNAL(finished()), SLOT(finishHideAnimation()));
}

void QNotificationPopup::setupMoveAnimation()
{
    m_moveAnimation.setTargetObject(this);
    m_moveAnimation.setPropertyName("pos");
    m_moveAnimation.setDuration(2000);
    m_moveAnimation.setEasingCurve(QEasingCurve::OutQuint);
}

void QNotificationPopup::hideMe()
{
    if (m_fadeOutEffectCanBeEnabled)
    {
        runSmoothHideAnimation();
    }
    else
    {
        this->close();
    }
}

void QNotificationPopup::checkFadeOutEffectCanBeEnabled()
{
    // Fade out can be enabled always except when desktop composition is available but disabled
    m_fadeOutEffectCanBeEnabled = true;
    
    SystemInformation *systemInfo = SystemInformation::newInstance();
    if (systemInfo->isDesktopCompositionAvailable())
    {
        m_fadeOutEffectCanBeEnabled = systemInfo->isDesktopCompositionEnabled();
    }

    delete systemInfo;
}

void QNotificationPopup::runSmoothHideAnimation()
{
    if (m_hideAnimation.state() != QAbstractAnimation::Running)
    {
        m_hideAnimation.start();
    }
}

void QNotificationPopup::runMoveAnimation()
{
    if (m_moveAnimation.state() != QAbstractAnimation::Running)
    {
        // TODO The position should be enhanced with the information provided by ScreenManager and ScreenLayout classes
        m_moveAnimation.setEndValue(QApplication::desktop()->availableGeometry(this).bottomRight() - this->rect().bottomRight());
        m_moveAnimation.start();
    }
}

void QNotificationPopup::finishHideAnimation()
{
    this->hide();
    this->setWindowOpacity(1.0);
}

} // End namespace udg
