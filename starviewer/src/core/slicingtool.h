/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSLICINGTOOL_H
#define UDGSLICINGTOOL_H

#include "tool.h"

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

private:
    /// Comença l'slicing
    void startSlicing();

    /// Calcula la llesca que s'ha de moure
    void doSlicing();

    /// Atura l'estat d'slicing
    void endSlicing();

    /**
     * Canvia el mode d'slicing tenint en compte l'actual
     */
    void switchSlicingMode();

    /**
     * Actualitza el valor de la llesca/fase, en funció del mode en que estem
     * @param value nou valor de la llesca/fase
     */
    void updateIncrement(int increment);

    /// Calcula les imatges o fases a tenir en compte
    /// pel càlcul del desplaçament a aplicar durant la interacció
    /// depenent del mode en que ens trobem
    void computeImagesForScrollMode();

    /// Segons l'input escull el millor mode d'srcoll per defecte
    /// Per exemple, en el cas que només tinguem fases i una sola imatge 
    /// és millor que per defecte estem en PhaseMode
    void chooseBestDefaultScrollMode( Volume *input );

private slots:
    /// Es crida cada cop que l'input del viewer s'ha actualitzat
    void inputChanged(Volume *input);

private:
    enum { SliceMode, PhaseMode };

    /// Ens guardem aquest punter per ser més còmode
    Q2DViewer *m_2DViewer;

    /// Coordenades per calcular el moviment del mouse que determina com incrmentar o decrementar l'slicing
    int m_startPosition[2], m_currentPosition[2];

    /// El mode en que movem les llesques.
    /// De moment podrà tenir els valors SliceMode o PhaseMode, per defecte SliceMode
    int m_slicingMode;

    /// estats d'execució de la tool
    int m_state;

    /// indica si entre event i event hi ha hagut moviment del ratolí
    bool m_mouseMovement;

	/// Nombre d'imatges ( ja siguin fases o llesques ) del volum sobre el que fem slicing
    /// útil per calcular l'increment proporcional de llesques
    int m_numberOfImages;
	
	/// mida de la pantalla
	/// útil per calcular l'increment proporcional de llesques
	int *m_screenSize;

    /// Ens indica si l'input actual té fases. S'actualitza cada cop que es canvia d'input
    bool m_inputHasPhases;

    /// Controla si estem forçant el mode de phases amb la tecla Ctrl
    bool m_forcePhaseMode;

    /// NOMÉS PER ESTADÍSTIQUES
    // compta el nombre de passos (endavant o enrera) que es fan seguits amb la rodeta
    QString m_wheelSteps;
    QString m_scrollSteps;
};

}

#endif
