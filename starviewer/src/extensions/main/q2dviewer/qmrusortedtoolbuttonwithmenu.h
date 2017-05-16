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

#ifndef UDG_QMRUSORTEDTOOLBUTTONWITHMENU_H
#define UDG_QMRUSORTEDTOOLBUTTONWITHMENU_H

#include <QToolButton>

class QSignalMapper;

namespace udg {

/**
 * @brief The QMruSortedToolButtonWithMenu class is a QToolButton with a menu that keeps all its actions sorted in most recently used order, with the first one
 *        being the default action and the rest in the menu. When an action in the menu is triggered it becomes the new default action and the previous default
 *        becomes the first in the menu.
 */
class QMruSortedToolButtonWithMenu : public QToolButton
{
    Q_OBJECT

public:
    explicit QMruSortedToolButtonWithMenu(QWidget *parent = 0);

    /// Adds the given action to the last position.
    /// If there is no current default action, the given one becomes the default action; otherwise it's appended to the menu.
    /// If this button already has the given action or the action is null, it does nothing.
    /// Whenever the action is triggered the actions in the button and the menu will be rearranged.
    void addAction(QAction *action);
    /// Adds the given actions in the same order that they are.
    void addActions(const QList<QAction*> &actions);
    /// Returns the list of actions contained in this button in most recently used order.
    QList<QAction*> actions() const;
    /// Return true if the given action is not null and is the default action or it's in the menu.
    bool hasAction(QAction *action) const;

public slots:
    /// Sets the given action as the default action and moves the previous default action to the first position in the menu.
    void setDefaultAction(QAction *action);

private:
    /// Used to call setDefaultAction() with the triggered action.
    QSignalMapper *m_signalMapper;

};

} // namespace udg

#endif // UDG_QMRUSORTEDTOOLBUTTONWITHMENU_H
