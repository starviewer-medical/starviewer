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

#ifndef QENHANCEDMENUTOOLBUTTON_H
#define QENHANCEDMENUTOOLBUTTON_H

#include <QToolButton>

namespace udg {

/**
    Subclass of QToolButton that allows to customize the position of the menu.

    It has one issue: it doesn't support polymorphic behaviour through a pointer to the base class. This means that you have to set and get the menu always
    through a pointer to QEnhancedMenuToolButton and not a pointer to QToolButton. Menu handling is implemented locally in this class without using
    QToolButton's implementation, i.e., the menu is stored in a pointer here, discarding the parent's own menu pointer, and the methods menu() and
    setMenu(QMenu*) hide the parent's ones without truly overriding them. Thus, if you use a pointer to QToolButton you will call QToolButton's implementation
    instead of QEnhancedMenuToolButton's one, leading to unexpected results.

    It has been done this way because QToolButton doesn't have the appropriate virtual methods to allow a true polymorphic implementation.
 */
class QEnhancedMenuToolButton : public QToolButton {

    Q_OBJECT

public:
    /// Menu position with respect to the button.
    enum MenuPosition { Above, Below };
    /// Menu alignment with respect to the button once its position has been determined. AlignLeft = align left sides; AlignRight = align right sides.
    enum MenuAlignment { AlignLeft, AlignRight };

    /// Creates a tool button with no menu, menu position below, and left alignment.
    QEnhancedMenuToolButton(QWidget *parent = 0);
    virtual ~QEnhancedMenuToolButton();

    /// Returns the current menu or null if there is none.
    /// \note This method isn't virtual here nor in the base class, so calling menu() through a QToolButton pointer will call QToolButton's implementation and
    ///       return a different menu.
    QMenu* menu() const;
    /// Sets the current menu.
    /// \note This method isn't virtual here nor in the base class, so calling setMenu() through a QToolButton pointer will call QToolButton's implementation
    ///       and set a menu that won't be controlled by this class.
    void setMenu(QMenu *menu);

    /// Sets the menu position to the given one.
    void setMenuPosition(MenuPosition position);
    /// Sets the menu alignment to the given one.
    void setMenuAlignment(MenuAlignment alignment);

public slots:
    /// Shows the current menu. If there is no current menu it does nothing.
    /// \note The current menu must be set using this class's setMenu(), not QToolButton::setMenu().
    void showMenu();

private:
    /// Creates the connection to show the menu when this button is pressed.
    Q_INVOKABLE void connectButtonToMenu();

private:
    /// The current menu.
    /// \note The current menu must be set using this class's setMenu(), not QToolButton::setMenu().
    QMenu *m_menu;

    /// Current menu position.
    MenuPosition m_menuPosition;
    /// Current menu alignment.
    MenuAlignment m_menuAlignment;

};

}

#endif
