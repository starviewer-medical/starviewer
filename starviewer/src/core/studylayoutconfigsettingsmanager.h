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

#ifndef UDGSTUDYLAYOUTCONFIGSETTINGSMANAGER_H
#define UDGSTUDYLAYOUTCONFIGSETTINGSMANAGER_H

#include <QList>

namespace udg {

class StudyLayoutConfig;

/** 
    Classe que s'encarrega d'obtenir, afegir, actualitzar i esborrar els StudyLayoutConfig desats a settings.
  */
class StudyLayoutConfigSettingsManager {
public:
    StudyLayoutConfigSettingsManager();
    ~StudyLayoutConfigSettingsManager();

    /// Ens retorna la llista de configuracions desades a settings
    QList<StudyLayoutConfig> getConfigList() const;

    /// Mètodes per afegir una nova configuració, actualitzar-la o esborrar-la. Retorna cert si l'operació es duu a terme amb èxit, fals altrament.

    /// Afegeix una nova configuració si no n'existeix cap per aquella modalitat
    bool addItem(const StudyLayoutConfig &config);

    /// Actualitza la configuració en cas que n'existeixi una de configurada per aquella modalitat
    bool updateItem(const StudyLayoutConfig &config);

    /// Esborra la configuració en cas que n'existeixi una de configurada per aquella modalitat
    bool deleteItem(const StudyLayoutConfig &config);
};

}

#endif
