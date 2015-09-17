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

#ifndef UDGITEMMENU_H
#define UDGITEMMENU_H

#include <QFrame>

// FWD declarations
class QVariant;
class QEvent;

namespace udg {

/**
    Item de menu. Envia un senyal quan esta activat i quan és seleccinat. Així com canvia de color al estar activat. Pot guardar informació del tipus QVariant.
    També es pot escollir el comportament del canvi de color.
  */
class ItemMenu : public QFrame {
Q_OBJECT
public:
    ItemMenu(QWidget *parent = 0);

    ~ItemMenu();

    /// Posar una dada
    void setData(QString data);

    /// Obtenir les dades
    QString getData();

    /// Mètode per fixar que les caselles es quedin seleccionades o es deseleccionin al marxar el mouse
    void setFixed(bool option);

    /// Mètode per fixar l'element com a seleccionat o no seleccionat
    void setSelected(bool option);

    /// Returns true if this item has the border enabled and false otherwise.
    bool hasBorder() const;
    /// Enables or disables the border of this item according to the given value.
    void setBorder(bool on);

signals:
    /// Signal que s'emet al entrar el mouse al widget
    void isActive(ItemMenu *);

    /// Signal que s'emet al seleccionar l'item
    void isSelected(ItemMenu *);

protected:
    /// Sobrecàrrega del mètode que tracta tots els events
    bool event(QEvent *event);

protected:
    /// Dades que pot guardar el widget
    QString m_data;

    /// Atribut que informa si s'ha de quedar seleccionat o s'ha de deseleccionar
    bool m_fixed;

    /// True if the item is selected and false otherwise.
    bool m_selected;

    /// True if this item has the border enabled and false otherwise.
    bool m_border;

};

}

#endif
