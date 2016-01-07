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

#ifndef UDGOPTIMALVIEWERSGRIDESTIMATOR_H
#define UDGOPTIMALVIEWERSGRIDESTIMATOR_H

#include <QPair>
#include <QMap>

namespace udg {

/**
    Classe per calcular quin és el grid de visors òptim segons els nombre de visors mínims necessaris.
    Hi ha un nombre màxim de visors que es poden crear fixat actualment en 48.
    // TODO La classe hauria d'evolucionar perquè es puguin donar millors layouts òptims, segons orientació de pantalla,
    número de pantalles, etc.
  */
class OptimalViewersGridEstimator {
public:
    OptimalViewersGridEstimator();
    ~OptimalViewersGridEstimator();

    /// Donat un nombre mínim de cel·les necessàries, ens dóna la parella de files i columnes òptima
    QPair<int, int> getOptimalGrid(int minimumCells);

private:
    /// Mapa que ens diu pel nombre de viewers, quantes files i columnes són les idònies
    QMap<int, QPair<int, int> > m_gridsMap;

    /// Nombre màxim de viewers que es poden crear. Vindrà determinat pel contingut de m_gridsMap;
    int m_maximumNumberOfViewers;
};

} // End namespace udg

#endif
