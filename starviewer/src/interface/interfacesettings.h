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
    static const QString languageLocaleKey;
};

} // end namespace udg 

#endif
