#ifndef UDGGLIALESTIMATIONSETTINGS_H
#define UDGGLIALESTIMATIONSETTINGS_H

#include "defaultsettings.h"

namespace udg {

class GlialEstimationSettings : public DefaultSettings
{
public:
    GlialEstimationSettings();
    ~GlialEstimationSettings();

    void init();

    /// Declaració de claus (aquesta extensió no té encara cap setting!)
};

} // end namespace udg 

#endif