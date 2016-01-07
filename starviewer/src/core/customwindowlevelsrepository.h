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

#ifndef UDGCUSTOMWINDOWLEVELSREPOSITORY_H
#define UDGCUSTOMWINDOWLEVELSREPOSITORY_H

#include "repository.h"
#include "windowlevel.h"

namespace udg {

/**
    Classe que implementa un Repositori de CustomWindowLevels.
    Aquesta classe és una implementació de la classe Repository
  */
class CustomWindowLevelsRepository : public Repository<WindowLevel> {

public:
    /// Ens retorna l'única instància del repositori (Patró Singleton)
    static CustomWindowLevelsRepository* getRepository()
    {
        static CustomWindowLevelsRepository customWindowLevelRepository;
        return &customWindowLevelRepository;
    }

    ~CustomWindowLevelsRepository();

private:
    // Constructor private pq només es puguin definir objectes a través del patro Singleton
    CustomWindowLevelsRepository();
};

}

#endif
