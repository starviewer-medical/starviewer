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
    static const QString OpenFileLastPath;
    static const QString OpenDirectoryLastPath;
    static const QString OpenFileLastFileExtension;
    static const QString ApplicationMainWindowGeometry;
    // Indicarà si permetem tenir més d'una instància de cada extensió (true) o únicament una (false)
    static const QString AllowMultipleInstancesPerExtension;
    // Defineix quina és l'extensió que s'obrirà per defecte
    static const QString DefaultExtension;
};

} // end namespace udg 

#endif
