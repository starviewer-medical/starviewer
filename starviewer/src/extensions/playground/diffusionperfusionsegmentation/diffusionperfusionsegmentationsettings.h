#ifndef UDGDIFFUSIONPERFUSIONSEGMENTATIONSETTINGS_H
#define UDGDIFFUSIONPERFUSIONSEGMENTATIONSETTINGS_H

#include "defaultsettings.h"

namespace udg {

class DiffusionPerfusionSegmentationSettings : public DefaultSettings
{
public:
    DiffusionPerfusionSegmentationSettings();
    ~DiffusionPerfusionSegmentationSettings();

    void init();

    /// Declaració de claus
    static const QString HorizontalSplitterGeometry;
    static const QString VerticalSplitterGeometry;
    static const QString SavedMaskPath;
};

} // end namespace udg 

#endif