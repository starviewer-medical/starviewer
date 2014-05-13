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

#ifndef UDGDEFAULTSETTINGS_H
#define UDGDEFAULTSETTINGS_H

// Incloem Settings perquè sempre que fem servir una de les seves classes
// derivades voldrem accedir a settings, per tant estalviem l'include
#include "settings.h"

namespace udg {

/**
    Classe base per definir i registrar els settings per defecte de cada mòdul d'starviewer.
    Cada implementació haurà d'implementar el mètode init() que s'encarrega de registrar els
    settings amb els corresponents valors per defecte.
    Per una part, cada subclasse definirà amb constants al .h totes les claus dels settings perquè aquestes
    siguin accessibles per a tothom, independentment de si tenen un valor definit per defecte o no.
    D'altra banda els settings que tinguin un valor per defecte es registraran en el mètode init()
  */
class DefaultSettings {
public:
    DefaultSettings();
    ~DefaultSettings();

    /// Mètode virtual pur. Cada subclasse el reimplentarà registrant els settings
    virtual void init() = 0;

};

} // End namespace udg

#endif
