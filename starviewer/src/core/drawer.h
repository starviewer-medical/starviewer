/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDRAWER_H
#define UDGDRAWER_H

#include <QObject>

namespace udg {

class Q2DViewer;
class DrawerPrimitive;

/**
    Classe encarregada de pintar els objectes de primitiva gràfica en el viewer assignat
*/
class Drawer : public QObject {
Q_OBJECT
public:
    Drawer(Q2DViewer *viewer, QObject *parent = 0);
    ~Drawer();

    /// Dibuixa la primitiva donada
    void draw(DrawerPrimitive *primitive);

    /// Actualitza l'escena
    void updateRenderer();

public slots:
    /// Elimina la primitiva donada
    void erasePrimitive(DrawerPrimitive *primitive);

private:
    /// Viewer sobre el qual pintarem les primitives
    Q2DViewer *m_2DViewer;
};

}

#endif
