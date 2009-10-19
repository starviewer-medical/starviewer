#ifndef UDGVOLUME3DVIEWTESTINGSETTINGS_H
#define UDGVOLUME3DVIEWTESTINGSETTINGS_H

#include "defaultsettings.h"

namespace udg {

class Volume3DViewTestingSettings : public DefaultSettings
{
public:
    Volume3DViewTestingSettings();
    ~Volume3DViewTestingSettings();

    void init();

    /// Declaració de claus
    static const QString CustomClutsDirPath;
};

} // end namespace udg 

#endif
