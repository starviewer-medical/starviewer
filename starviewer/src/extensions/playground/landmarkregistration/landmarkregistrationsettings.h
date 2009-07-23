#ifndef UDGLANDMARKREGISTRATIONSETTINGS_H
#define UDGLANDMARKREGISTRATIONSETTINGS_H

#include "defaultsettings.h"

namespace udg {

class LandmarkRegistrationSettings : public DefaultSettings
{
public:
    LandmarkRegistrationSettings();
    ~LandmarkRegistrationSettings();

    void init();

    /// Declaració de claus
    static const QString VerticalSplitterGeometry;
    static const QString VerticalSplitter2Geometry;
    static const QString SavedFilesPath;
};

} // end namespace udg 

#endif
