#ifndef UDGQ2DVIEWERSETTINGS_H
#define UDGQ2DVIEWERSETTINGS_H

#include "defaultsettings.h"

namespace udg {

class Q2DViewerSettings : public DefaultSettings
{
public:
    Q2DViewerSettings();
    ~Q2DViewerSettings();

    void init();

    /// Declaració de claus
    static const QString UserProfile;
};

} // end namespace udg 

#endif
