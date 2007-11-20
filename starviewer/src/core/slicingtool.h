/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSLICINGTOOL_H
#define UDGSLICINGTOOL_H

#include "tool.h"

class QTime;

namespace udg {

class Q2DViewer;
class Volume;

/**
Tool que serveix per fer slicing en un visor 2D

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class SlicingTool : public Tool
{
Q_OBJECT
public:
    /// estats de la tool
    enum { NONE , SLICING };

    SlicingTool( QViewer *viewer, QObject *parent = 0 );
    ~SlicingTool();

    void handleEvent( unsigned long eventID );

private slots:
    /// Comença l'slicing
    void startSlicing();

    /// Calcula la llesca que s'ha de moure
    void doSlicing();

    /// Atura l'estat d'slicing
    void endSlicing();

private:
    /**
     * Canvia el mode d'slicing tenint en compte l'actual
     */
    void switchSlicingMode();

    /**
     * Actualitza el valor de la llesca/fase, en funció del mode en que estem
     * @param value nou valor de la llesca/fase
     */
    void updateIncrement(int increment);

    /**
     * Ens diu si l'input actual té fases o no
     * @return
     */
    bool currentInputHasPhases();

private:
    enum { SliceMode, PhaseMode };

    /// Ens guardem aquest punter per ser més còmode
    Q2DViewer *m_2DViewer;

    /// Coordenades per calcular el moviment del mouse que determina com incrmentar o decrementar l'slicing
    int m_startPosition[2], m_currentPosition[2];

    /// El mode en que movem les llesques.
    /// De moment podrà tenir els valors SliceMode o PhaseMode, per defecte SliceMode
    int m_slicingMode;

    /// Temps que ha passat des de que s'ha iniciat l'slicing
    QTime * m_time;

    /// Temps en milisegons de l'ultim event tractat
    int m_latestTime;

    /// estats d'execució de la tool
    int m_state;

    /// indica si entre event i event hi ha hagut moviment del ratolí
    bool m_mouseMovement;
};

}

#endif
