/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGSLICINGTOOL_H
#define UDGSLICINGTOOL_H

#include <QPoint>
#include <QSize>

#include "tool.h"

namespace udg {

class Q2DViewer;
class Volume;

/**
    Tool que serveix per fer slicing en un visor 2D
  */
class SlicingTool : public Tool {
Q_OBJECT
public:
    /// Estats de la tool
    enum { None, Slicing };

    /// Mode d'slicing, si estem fent per llesques o per fases
    enum SlicingMode { SliceMode, PhaseMode };

    SlicingTool(QViewer *viewer, QObject *parent = 0);
    virtual ~SlicingTool();

    virtual void handleEvent(unsigned long eventID) override;

    /// Retorna el mode de slicing (Slice o Phase)
    SlicingMode getSlicingMode();

protected:
    /// Actualitza el valor de la llesca/fase, en funció del mode en que estem
    /// @param increment nou valor de la llesca/fase
    void updateIncrement(int increment);

    /// Canvia el mode d'slicing tenint en compte l'actual
    void switchSlicingMode();

    /// Calcula les imatges o fases a tenir en compte
    /// pel càlcul del desplaçament a aplicar durant la interacció
    /// depenent del mode en que ens trobem
    void computeImagesForScrollMode();

private:
    /// Comença l'slicing
    void startSlicing();

    /// Calcula la llesca que s'ha de moure
    void doSlicing();

    /// Atura l'estat d'slicing
    void endSlicing();

    /// Segons l'input escull el millor mode d'srcoll per defecte
    /// Per exemple, en el cas que només tinguem fases i una sola imatge
    /// és millor que per defecte estem en PhaseMode
    void chooseBestDefaultScrollMode(Volume *input);

private slots:
    /// Es crida cada cop que l'input del viewer s'ha actualitzat
    void inputChanged(Volume *input);

protected:
    /// Ens guardem aquest punter per ser més còmode
    Q2DViewer *m_2DViewer;

    /// Indica si entre event i event hi ha hagut moviment del ratolí
    bool m_mouseMovement;

    /// Ens indica si l'input actual té fases. S'actualitza cada cop que es canvia d'input
    bool m_inputHasPhases;

    /// Controla si estem forçant el mode de phases amb la tecla Ctrl
    bool m_forcePhaseMode;

    /// El mode en que movem les llesques.
    /// De moment podrà tenir els valors SliceMode o PhaseMode, per defecte SliceMode
    SlicingMode m_slicingMode;

private:
    /// Coordenades per calcular el moviment del mouse que determina com incrmentar o decrementar l'slicing
    QPoint m_startPosition;
    QPoint m_currentPosition;

    /// Estats d'execució de la tool
    int m_state;

    /// Nombre d'imatges (ja siguin fases o llesques) del volum sobre el que fem slicing
    /// útil per calcular l'increment proporcional de llesques
    int m_numberOfImages;

    /// Mida de la pantalla
    /// Útil per calcular l'increment proporcional de llesques
    QSize m_screenSize;
};

}

#endif
