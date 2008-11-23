/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
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

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class ItemMenu : public QFrame
{
Q_OBJECT
public:
    ItemMenu( QWidget * parent = 0);

    ~ItemMenu();

    /// Posar una dada
    void setData( QString data );

    /// Obtenir les dades
    QString getData( );

    /// Mètode per fixar que les caselles es quedin seleccionades o es deseleccionin al marxar el mouse
    void setFixed( bool option );

    /// Mètode per fixar l'element com a seleccionat o no seleccionat
    void setSelected( bool option );

signals:

    /// Signal que s'emet al entrar el mouse al widget
    void isActive( ItemMenu * );

    /// Signal que s'emet al seleccionar l'item
    void isSelected( ItemMenu * );

protected:

    /// Sobrecàrrega del mètode que tracta tots els events
    bool event( QEvent * event);

protected:

    /// Dades que pot guardar el widget
    QString m_data;

    /// Atribut que informa si s'ha de quedar seleccionat o s'ha de deseleccionar
    bool m_fixed;

};

}

#endif
