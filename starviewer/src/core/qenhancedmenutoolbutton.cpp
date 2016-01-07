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

#include "qenhancedmenutoolbutton.h"

#include <QMenu>

namespace udg {

namespace {

// Returns the top left point of the button's menu necessary for it to be positioned above the given button.
QPoint getAbovePosition(const QEnhancedMenuToolButton *button)
{
    return button->mapToGlobal(QPoint(0, -button->menu()->sizeHint().height()));
}

// Returns the top left point of the button's menu necessary for it to be positioned below the given button.
QPoint getBelowPosition(const QEnhancedMenuToolButton *button)
{
    return button->mapToGlobal(button->rect().bottomLeft());
}

// Modifies the given menu point so that the given button and its menu are right-aligned.
void alignRight(QPoint &menuPoint, const QEnhancedMenuToolButton *button)
{
    menuPoint.rx() -= button->menu()->sizeHint().width() - button->width();
}

}

QEnhancedMenuToolButton::QEnhancedMenuToolButton(QWidget *parent)
 : QToolButton(parent), m_menu(0), m_menuPosition(Below), m_menuAlignment(AlignLeft)
{
    connectButtonToMenu();
}

QEnhancedMenuToolButton::~QEnhancedMenuToolButton()
{
}

QMenu* QEnhancedMenuToolButton::menu() const
{
    return m_menu;
}

void QEnhancedMenuToolButton::setMenu(QMenu *menu)
{
    m_menu = menu;
}

void QEnhancedMenuToolButton::setMenuPosition(MenuPosition position)
{
    m_menuPosition = position;
}

void QEnhancedMenuToolButton::setMenuAlignment(MenuAlignment alignment)
{
    m_menuAlignment = alignment;
}

void QEnhancedMenuToolButton::showMenu()
{
    if (!menu())
    {
        return;
    }

    this->setDown(true);
    QPoint menuPoint;

    switch (m_menuPosition)
    {
        case Above:
            menuPoint = getAbovePosition(this);
            break;

        default:
        case Below:
            menuPoint = getBelowPosition(this);
            break;
    }

    switch (m_menuAlignment)
    {
        default:
        case AlignLeft:
            break;

        case AlignRight:
            alignRight(menuPoint, this);
            break;
    }

    // Remove the connection so that the menu is closed on the next click
    disconnect(this, SIGNAL(pressed()), this, SLOT(showMenu()));

    menu()->exec(menuPoint);
    this->setDown(false);

    // Create the connection again on the next event loop iteration (not immediately)
    QMetaObject::invokeMethod(this, "connectButtonToMenu", Qt::QueuedConnection);
}

void QEnhancedMenuToolButton::connectButtonToMenu()
{
    connect(this, SIGNAL(pressed()), SLOT(showMenu()));
}

}
