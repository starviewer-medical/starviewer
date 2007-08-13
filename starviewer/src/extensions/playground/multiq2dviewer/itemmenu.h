/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGITEMMENU_H
#define UDGITEMMENU_H

#include <QWidget>
#include <QVariant>
#include <QEvent>

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ItemMenu : public QWidget
{
public:
    ItemMenu( QWidget * parent = 0);

    ~ItemMenu();

    /// Posar una dada
    void setData( QVariant * data ){ m_data = data; }

    /// Obtenir les dades
    QVariant getData( ){ return m_data; }

protected:

    /// Sobrecàrrega del mètode que tracta tots els events
    bool event( QEvent * event);

    /// Dades que pot guardar el widget
    QVariant * m_data;
};

}

#endif
