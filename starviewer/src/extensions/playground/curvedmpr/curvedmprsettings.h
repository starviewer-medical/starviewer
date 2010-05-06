#ifndef UDGCURVEDMPRSETTINGS_H
#define UDGCURVEDMPRSETTINGS_H

#include "defaultsettings.h"

namespace udg {

class CurvedMPRSettings : public DefaultSettings
{
public:
    CurvedMPRSettings();
    ~CurvedMPRSettings();

    void init();
};

} // end namespace udg 

#endif