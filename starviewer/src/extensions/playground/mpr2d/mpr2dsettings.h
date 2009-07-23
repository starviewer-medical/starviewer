#ifndef UDGMPR2DSETTINGS_H
#define UDGMPR2DSETTINGS_H

#include "defaultsettings.h"

namespace udg {

class MPR2DSettings : public DefaultSettings
{
public:
    MPR2DSettings();
    ~MPR2DSettings();

    void init();

    /// Declaració de claus
    static const QString HorizontalSplitterGeometry;
    static const QString VerticalSplitterGeometry;
};

} // end namespace udg 

#endif