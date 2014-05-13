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

#ifndef UDGDISTANCETOOL_H
#define UDGDISTANCETOOL_H

#include "genericdistancetool.h"

namespace udg {

/**
    Eina per mesurar distàncies en un visor 2D.
    Ens permet fixar dos punts a la pantalla i calcular la distància que els separa.

    Per marcar cadascun dels punts es farà amb un clic o doble-clic amb el botó esquerre
    del ratolí.
    Un cop marcat el primer punt de la distància a calcular, es mostrarà una línia que unirà
    el primer punt marcat amb el punt on es trobi el punter del mouse. Mentres l'usuari no
    hagi marcat el segon punt, no es mostrarà cap informació respecte a la distància en aquell moment.

    Un cop marcat el segon punt, la mesura es mostrarà en mil·límetres, si existeix informació
    d'espaiat, en píxels altrament.

    Un cop acabada la mesura, es poden annotar successivament tantes mesures com es desitji.

    Les mesures annotades només apareixeran sobre les imatges sobre les que s'han realitzat,
    però no s'esborren si canviem d'imatge. Si canviem d'imatge però tornem a mostrar-la més endavant
    les annotacions fetes sobre aquesta anteriorment, tornaran a aparèixer.

    Quan es canvïi l'input del visor, les annotacions fetes fins aquell moment s'esborraran.
    Quan es desactivi l'eina, les annotacions fetes fins aquell moment es mantindran.
  */
class DistanceTool : public GenericDistanceTool {
Q_OBJECT
public:
    DistanceTool(QViewer *viewer, QObject *parent = 0);
    ~DistanceTool();

protected:
    void handleLeftButtonPress();
    void handleMouseMove();
    void abortDrawing();

private:
    /// Gestiona quin punt de la distància estem dibuixant. Es cridarà cada cop que
    /// haguem fet un clic amb el botó esquerre del mouse.
    void handlePointAddition();

    /// Marca un nou punt de la distància. Si la corresponent primitiva
    /// no s'ha creat es crea abans d'afegir el nou punt.
    void annotateNewPoint();

    /// Simula la línia quan es mou el ratolí i tenim el primer punt marcat.
    void simulateLine();

    /// Elimina la representacio temporal de la tool
    void deleteTemporalRepresentation();

    /// Equalitza la profunditat dels elements que formen la distància final.
    void equalizeDepth();

private slots:
    /// Inicialitza l'estat de la tool.
    void initialize();

private:
    /// Estats possibles de la línia dibuixada.
    enum { NoPointFixed, FirstPointFixed };

    /// Estat de la línia.
    int m_lineState;
};

}

#endif
