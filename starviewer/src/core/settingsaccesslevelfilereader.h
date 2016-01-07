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

#ifndef UDG_SETTINGSACCESSLEVELFILEREADER_H
#define UDG_SETTINGSACCESSLEVELFILEREADER_H

#include "settings.h"

namespace udg {

/**
    Lector de fitxers .INI en els que es defineixen els nivells d'accés
    que li volem configurar als settings
  */
class SettingsAccessLevelFileReader {
public:
    SettingsAccessLevelFileReader();
    ~SettingsAccessLevelFileReader();

    /// Llegeix el fitxer amb les dades de nivell d'accés
    bool read(const QString &filePath);

    /// Ens retorna la última taula de nivells d'accés llegida
    QMap<QString, Settings::AccessLevel> getAccessLevelTable() const;

private:
    /// Taula de nivells d'accés
    QMap<QString, Settings::AccessLevel> m_accessLevelTable;
};

} // End namespace udg

#endif
