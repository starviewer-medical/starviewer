#ifndef UDG_SETTINGSACCESSLEVELFILEREADER_H
#define UDG_SETTINGSACCESSLEVELFILEREADER_H

#include "settings.h"

namespace udg {

/**
    Lector de fitxers .INI en els que es defineixen els nivells d'accés
    que li volem configurar als settings
 */
class SettingsAccessLevelFileReader
{
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

} // end namespace udg

#endif
