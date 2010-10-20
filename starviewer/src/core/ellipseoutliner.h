/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGELLIPSEOUTLINER_H
#define UDGELLIPSEOUTLINER_H

#include <QObject>
#include <QPointer>
#include "outliner.h"

namespace udg {

class DrawerPolygon;

/**
    Outliner per dibuixar una el·lipse
*/
class EllipseOutliner : public Outliner {
Q_OBJECT
public:
    EllipseOutliner(Q2DViewer *viewer, QObject *parent = 0);
    ~EllipseOutliner();

public slots:
    void handleEvent(long unsigned eventID);

private:
    /// Gestiona les accions a realitzar quan es clica el ratolí
    void handlePointAddition();
    
    /// Simula la forma de l'el·lipse quan tenim el primer punt i movem el mouse
    void simulateEllipse();

    /// Calcula el centre de l'el·lipse a partir dels punts introduits mitjançant la interacció de l'usuari
    void computeEllipseCentre(double centre[3]);

    /// Actualitza els punts del polígon perquè resulti el dibuix de l'el·lipse
    void updatePolygonPoints();

    /// Dona el dibuix de l'el·lipse per finalitzat
    void closeForm();

private:
    /// Estats de l'outliner
    enum { Ready, FirstPointFixed };
    
    /// Punts que annotem de la interacció de l'usuari per crear l'el·lipse
    double m_firstPoint[3];
    double m_secondPoint[3];

    /// Estat de l'outliner
    int m_state;

    /// Primitiva per dibuixar l'el·lipse
    QPointer<DrawerPolygon> m_ellipsePolygon;

};

}

#endif
