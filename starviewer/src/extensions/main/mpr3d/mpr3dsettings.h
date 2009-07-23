#ifndef UDGMPR3DSETTINGS_H
#define UDGMPR3DSETTINGS_H

#include "defaultsettings.h"

namespace udg {

class MPR3DSettings : public DefaultSettings
{
public:
    MPR3DSettings();
    ~MPR3DSettings();

    void init();

    /// Declaració de claus (aquesta extensió no té encara cap!)
};

} // end namespace udg 

#endif
