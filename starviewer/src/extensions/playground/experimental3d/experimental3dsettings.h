#ifndef UDGEXPERIMENTAL3DSETTINGS_H
#define UDGEXPERIMENTAL3DSETTINGS_H

#include "defaultsettings.h"

namespace udg {

class Experimental3DSettings : public DefaultSettings
{
public:
    Experimental3DSettings();
    ~Experimental3DSettings();

    void init();

    /// Declaració de claus
    static const QString ViewPointEntropyDir;
    static const QString EntropyDir;
    static const QString ViewpointUnstabilitiesDir;
    static const QString TransferFunctionDir;
    static const QString BestViewsDir;
    static const QString GuidedTourDir;
    static const QString ExploratoryTourDir;
    static const QString CameraDir;
    static const QString ObscuranceDir;
    static const QString ProgramDir;
    static const QString ViewedVolumeDir;
    static const QString TourDir;
    static const QString ViewpointMutualInformationDir;
    static const QString MutualInformationDir;
    static const QString VoxelMutualInformationDir;
    static const QString ViewpointVoxelMutualInformationDir;
    static const QString ColorVoxelMutualInformationPaletteDir;
    static const QString ColorVoxelMutualInformationDir;
    static const QString EnhancedViewpointMutualInformationOpacityDir;
    static const QString EnhancedViewpointMutualInformationVoxelMutualInformationDir;
};

} // end namespace udg 

#endif