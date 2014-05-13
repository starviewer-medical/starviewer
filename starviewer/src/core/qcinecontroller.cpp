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

#include "qcinecontroller.h"
#include "qviewercinecontroller.h"

#include <QToolButton>
#include <QMenu>
#include <QWidgetAction>

namespace udg {

QCINEController::QCINEController(QWidget *parent)
 : QWidget(parent), m_cineController(0)
{
    setupUi(this);

    m_playToolButton->setPopupMode(QToolButton::MenuButtonPopup);

    QMenu *menu = new QMenu;

    QWidgetAction *velocityWidgetAction = new QWidgetAction(this);
    velocityWidgetAction->setDefaultWidget(m_velocityControl);
    menu->addAction(velocityWidgetAction);

    QWidgetAction *labelWidgetAction = new QWidgetAction(this);
    labelWidgetAction->setDefaultWidget(m_velocityLabel);
    menu->addAction(labelWidgetAction);

    QWidgetAction *loopWidgetAction = new QWidgetAction(this);
    loopWidgetAction->setDefaultWidget(m_loopCheckBox);
    menu->addAction(loopWidgetAction);

    QWidgetAction *boomerangWidgetAction = new QWidgetAction(this);
    boomerangWidgetAction->setDefaultWidget(m_boomerangCheckBox);
    menu->addAction(boomerangWidgetAction);

    m_playToolButton->setMenu(menu);
}

QCINEController::~QCINEController()
{
}

void QCINEController::setQViewer(QViewer *viewer)
{
    if (!m_cineController)
    {
        m_cineController = new QViewerCINEController(this);

        connect(m_loopCheckBox, SIGNAL(toggled(bool)), m_cineController, SLOT(enableLoop(bool)));
        connect(m_boomerangCheckBox, SIGNAL(toggled(bool)), m_cineController, SLOT(enableBoomerang(bool)));
        connect(m_loopCheckBox, SIGNAL(toggled(bool)), m_boomerangCheckBox, SLOT(setEnabled(bool)));

        m_playToolButton->setDefaultAction(m_cineController->getPlayAction());

        connect(m_velocityControl, SIGNAL(valueChanged(int)), m_cineController, SLOT(setVelocity(int)));
        connect(m_cineController, SIGNAL(velocityChanged(int)), SLOT(updateVelocityLabel(int)));
        connect(m_cineController, SIGNAL(velocityChanged(int)), m_velocityControl, SLOT(setValue(int)));
        m_cineController->setInputViewer(viewer);
        m_cineController->enableLoop(m_loopCheckBox->isChecked());
        m_cineController->enableBoomerang(m_boomerangCheckBox->isChecked());
    }
    else
    {
        m_cineController->setInputViewer(viewer);
    }

    m_loopCheckBox->setChecked(true);
}

void QCINEController::updateVelocityLabel(int value)
{
    m_velocityLabel->setText(tr("%1 img/s").arg(value));
}

void QCINEController::updateLoopStatus(bool enabled)
{
    m_boomerangCheckBox->setEnabled(enabled);
    m_cineController->enableLoop(enabled);
    m_cineController->enableBoomerang(m_boomerangCheckBox->isChecked());
}

}
