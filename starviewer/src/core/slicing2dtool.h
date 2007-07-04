/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSLICING2DTOOL_H
#define UDGSLICING2DTOOL_H

#include "tool.h"

namespace udg {

class Q2DViewer;

/**
Tool que serveix per fer slicing en un visor 2D

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Slicing2DTool : public Tool
{
Q_OBJECT
public:
    /// estats de la tool
    enum { NONE , SLICING };

    Slicing2DTool( Q2DViewer *viewer );
    ~Slicing2DTool();

    void handleEvent( unsigned long eventID );

private:
    Q2DViewer *m_2DViewer;

    /// Coordenades per calcular el moviment del mouse que determina com incrmentar o decrementar l'slicing
    int m_startPosition[2], m_currentPosition[2];

/// \TODO potser aquests mètodes slots passen a ser públics
private slots:
    /// Comença l'slicing
    void startSlicing();

    /// Calcula la llesca que s'ha de moure
    void doSlicing();

    /// Atura l'estat d'slicing
    void endSlicing();

};

}

#endif
