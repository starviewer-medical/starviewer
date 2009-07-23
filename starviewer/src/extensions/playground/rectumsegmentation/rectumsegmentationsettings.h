#ifndef UDGRECTUMSEGMENTATIONSETTINGS_H
#define UDGRECTUMSEGMENTATIONSETTINGS_H

#include "defaultsettings.h"

namespace udg {

class RectumSegmentationSettings : public DefaultSettings
{
public:
    RectumSegmentationSettings();
    ~RectumSegmentationSettings();

    void init();

    /// Declaració de claus
    static const QString HorizontalSplitterGeometry;
    static const QString VerticalSplitterGeometry;
    static const QString SavedMaskPath;
};

} // end namespace udg 

#endif
