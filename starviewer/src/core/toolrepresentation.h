/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef TOOLREPRESENTATION_H
#define TOOLREPRESENTATION_H

#include <QObject>

namespace udg {

class QViewer;
class DrawerPrimitive;
class Drawer;

/**
Classe contenidor de ToolRepresentation

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ToolRepresentation : public QObject
{
Q_OBJECT
public:
    ToolRepresentation( Drawer *drawer, QObject *parent = 0 );
    ~ToolRepresentation();

    /**
    * Rep events des de Tool per dibuixar-se
    * @param eventID tipus d'event
    * @param posX posició X de l'event
    * @param posY posició Y de l'event
    */
    virtual void handleEvent(unsigned int eventID, double posX, double posY);

signals:
    void finished();

protected:
    ///Actualitza el viewer
    void refresh();

protected:
    ///Drawer del viewer amb el que es pintaran les primitives
    Drawer *m_drawer;

    ///Llista de primitives a pintar
    QList<DrawerPrimitive *> m_primitivesList;
};

}

#endif
