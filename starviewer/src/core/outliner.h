/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGOUTLINER_H
#define UDGOUTLINER_H

#include <QObject>

namespace udg {

class Q2DViewer;
class DrawerPrimitive;

/**
Classe que dibuixa una DrawerPrimitive

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Outliner : public QObject
{
Q_OBJECT
public:
    Outliner( Q2DViewer *viewer, QObject *parent = 0 );
    ~Outliner();

public slots:
    /**
    * Rep events des de Tool per dibuixar
    * @param eventID tipus d'event
    */
    virtual void handleEvent( long unsigned eventID ) = 0;

signals:
    void finished( DrawerPrimitive * );


protected:
    ///Drawer del viewer amb el que es pintaran les primitives
    Q2DViewer *m_2DViewer;
};

}

#endif
