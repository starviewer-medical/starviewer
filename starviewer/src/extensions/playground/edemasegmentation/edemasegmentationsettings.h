#ifndef UDGEDEMASEGMENTATIONSETTINGS_H
#define UDGEDEMASEGMENTATIONSETTINGS_H

#include "defaultsettings.h"

namespace udg {

class EdemaSegmentationSettings : public DefaultSettings
{
public:
    EdemaSegmentationSettings();
    ~EdemaSegmentationSettings();

    void init();

    /// Declaració de claus
    static const QString HorizontalSplitterGeometry;
    static const QString VerticalSplitterGeometry;
    static const QString SavedMaskPath;
};

} // end namespace udg 

#endif