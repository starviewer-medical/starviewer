#ifndef UDGQ3DVIEWEREXTENSIONSETTINGS_H
#define UDGQ3DVIEWEREXTENSIONSETTINGS_H

#include "defaultsettings.h"

namespace udg {

class Q3DViewerExtensionSettings : public DefaultSettings {
public:
    Q3DViewerExtensionSettings();
    ~Q3DViewerExtensionSettings();

    void init();

    /// Declaració de claus
    static const QString CustomClutsDirPath;
};

} // end namespace udg

#endif
