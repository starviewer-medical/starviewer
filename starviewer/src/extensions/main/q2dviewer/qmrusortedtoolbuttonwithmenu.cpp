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

#include "qmrusortedtoolbuttonwithmenu.h"

#include <QMenu>
#include <QSignalMapper>

namespace udg {

QMruSortedToolButtonWithMenu::QMruSortedToolButtonWithMenu(QWidget *parent)
    : QToolButton(parent), m_signalMapper(new QSignalMapper(this))
{
    setPopupMode(QToolButton::MenuButtonPopup);
    setMenu(new QMenu(this));

    connect(m_signalMapper, static_cast<void(QSignalMapper::*)(QObject*)>(&QSignalMapper::mapped), [this](QObject *object) {
        setDefaultAction(static_cast<QAction*>(object));
    });
}

void QMruSortedToolButtonWithMenu::addAction(QAction *action)
{
    // Don't add it twice
    if (!action || hasAction(action))
    {
        return;
    }

    if (!defaultAction())
    {
        QToolButton::setDefaultAction(action);
    }
    else
    {
        menu()->addAction(action);
    }

    connect(action, &QAction::triggered, m_signalMapper, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));
    m_signalMapper->setMapping(action, action);
}

void QMruSortedToolButtonWithMenu::addActions(const QList<QAction *> &actions)
{
    foreach (QAction *action, actions)
    {
        addAction(action);
    }
}

QList<QAction*> QMruSortedToolButtonWithMenu::actions() const
{
    return !defaultAction() ? QList<QAction*>() : (QList<QAction*>() << defaultAction() << menu()->actions());
}

bool QMruSortedToolButtonWithMenu::hasAction(QAction *action) const
{
    return action && (defaultAction() == action || menu()->actions().contains(action));
}

void QMruSortedToolButtonWithMenu::setDefaultAction(QAction *action)
{
    if (!action)
    {
        return;
    }

    // Make sure that we have the action
    if (!hasAction(action))
    {
        addAction(action);
    }

    // Rearrange if needed
    if (defaultAction() != action)
    {
        // Move current default action to first position in menu
        if (menu()->actions().isEmpty())
        {
            menu()->addAction(defaultAction());
        }
        else
        {
            menu()->insertAction(menu()->actions().first(), defaultAction());
        }

        // Remove given action from menu
        if (menu()->actions().contains(action))
        {
            menu()->removeAction(action);
        }

        // Set the given action as the default
        QToolButton::setDefaultAction(action);
    }
}

} // namespace udg
