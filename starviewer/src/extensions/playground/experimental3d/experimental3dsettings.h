#ifndef UDGEXPERIMENTAL3DSETTINGS_H
#define UDGEXPERIMENTAL3DSETTINGS_H

#include "defaultsettings.h"

namespace udg {

class Experimental3DSettings : public DefaultSettings
{
public:
    Experimental3DSettings();
    virtual ~Experimental3DSettings();

    virtual void init();

    /// Declaració de claus
    static const QString TransferFunctionDir;

    static const QString ViewedVolumeDir;
    static const QString ViewpointEntropyDir;
    static const QString EntropyDir;
    static const QString ViewpointMutualInformationDir;
    static const QString MutualInformationDir;
    static const QString ViewpointUnstabilitiesDir;
    static const QString VoxelMutualInformationDir;
    static const QString BestViewsDir;
    static const QString GuidedTourDir;
    static const QString ExploratoryTourDir;
    static const QString CameraDir;
    static const QString ObscuranceDir;
    static const QString ProgramDir;
    static const QString TourDir;
    static const QString ViewpointVoxelMutualInformationDir;
    static const QString ColorVoxelMutualInformationPaletteDir;
    static const QString ColorVoxelMutualInformationDir;
    static const QString EnhancedViewpointMutualInformationOpacityDir;
    static const QString EnhancedViewpointMutualInformationVoxelMutualInformationDir;

    static const QString ViewedVolumeIntensityDir;
    static const QString ViewpointEntropyIntensityDir;
    static const QString EntropyIntensityDir;
    static const QString ViewpointMutualInformationIntensityDir;
    static const QString MutualInformationIntensityDir;
    static const QString ViewpointUnstabilitiesIntensityDir;
    static const QString IntensityMutualInformationDir;
};

} // end namespace udg 

#endif
