#ifndef UDG_VSIRECONSTRUCTIONSETTINGS_H
#define UDG_VSIRECONSTRUCTIONSETTINGS_H

#include "defaultsettings.h"

namespace udg{

class VSIReconstructionSettings : public DefaultSettings
{
public:
    VSIReconstructionSettings();
    ~VSIReconstructionSettings();

    void init();

    /// Declaració de claus
    static const QString HorizontalSplitterGeometry;
    static const QString VerticalSplitterGeometry;
    static const QString HorizontalImageSplitterGeometry;
    static const QString VerticalImageSplitterGeometry;
};

} // end namespace udg

#endif
