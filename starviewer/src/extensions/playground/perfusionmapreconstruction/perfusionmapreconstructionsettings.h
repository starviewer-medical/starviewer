#ifndef UDGPERFUSIONMAPRECONSTRUCTIONSETTINGS_H
#define UDGPERFUSIONMAPRECONSTRUCTIONSETTINGS_H

#include "defaultsettings.h"

namespace udg {

class PerfusionMapReconstructionSettings : public DefaultSettings
{
public:
    PerfusionMapReconstructionSettings();
    ~PerfusionMapReconstructionSettings();

    void init();

    /// Declaració de claus
    static const QString HorizontalSplitterGeometry;
    static const QString VerticalSplitterGeometry;
};

} // end namespace udg 

#endif
