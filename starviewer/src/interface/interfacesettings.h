#ifndef UDGINTERFACESETTINGS_H
#define UDGINTERFACESETTINGS_H

#include "defaultsettings.h"

namespace udg {

class InterfaceSettings : public DefaultSettings
{
public:
    InterfaceSettings();
    ~InterfaceSettings();

    void init();

    /// Declaració de claus
    static const QString openFileLastPathKey;
    static const QString openDirectoryLastPathKey;
    static const QString openFileLastFileExtensionKey;
    static const QString applicationMainWindowGeometryKey;
    // Indicarà si permetem tenir més d'una instància de cada extensió (true) o únicament una (false)
    static const QString allowMultipleInstancesPerExtensionKey;
    // Defineix quina és l'extensió que s'obrirà per defecte
    static const QString defaultExtensionKey;
};

} // end namespace udg 

#endif
