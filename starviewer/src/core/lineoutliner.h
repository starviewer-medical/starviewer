/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGLINEOUTLINER_H
#define UDGLINEOUTLINER_H

#include <QObject>
#include <QPointer>

#include "outliner.h"

namespace udg {

class Q2DViewer;
class DrawerLine;

/**
    Classe que dibuixa una DrawerLine
*/
class LineOutliner : public Outliner {
Q_OBJECT
public:
    LineOutliner(Q2DViewer *viewer, QObject *parent = 0);
    ~LineOutliner();

public slots:
    void handleEvent(long unsigned eventID);

private:
    /// Ens permet anotar el seguent punt de la polilinia. Si la primitiva no ha sigut creada, abans d'afegir el nou punt, la crea.
    void annotateNewPoint();

    /// Simula la linia quan es mou el ratolí
    void simulateLine();

private:
    /// Estat de la línia
    enum { NoPoints, FirstPoint, Finished };

    /// Línia que es dibuixa
    QPointer<DrawerLine> m_line;

    /// Estat de la línia
    int m_lineState;
};

}

#endif
