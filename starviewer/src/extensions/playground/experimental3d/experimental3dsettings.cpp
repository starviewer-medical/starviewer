#include "experimental3dsettings.h"

namespace udg {

// Definició de les claus
const QString KeyPrefix("Experimental3D/");

const QString Experimental3DSettings::TransferFunctionDir( KeyPrefix + "transferFunctionDir" );

const QString Experimental3DSettings::ViewedVolumeDir( KeyPrefix + "viewedVolumeDir" );
const QString Experimental3DSettings::HVzDir( KeyPrefix + "HVzDir" );
const QString Experimental3DSettings::HVDir( KeyPrefix + "HVDir" );
const QString Experimental3DSettings::HZvDir( KeyPrefix + "HZvDir" );
const QString Experimental3DSettings::HZVDir( KeyPrefix + "HZVDir" );
const QString Experimental3DSettings::HZDir( KeyPrefix + "HZDir" );
const QString Experimental3DSettings::ViewpointMutualInformationDir( KeyPrefix + "vmiDir" );
const QString Experimental3DSettings::MutualInformationDir( KeyPrefix + "miDir" );
const QString Experimental3DSettings::ViewpointUnstabilitiesDir( KeyPrefix + "viewpointUnstabilitiesDir" );
const QString Experimental3DSettings::VoxelMutualInformationDir( KeyPrefix + "vomiDir" );
const QString Experimental3DSettings::VoxelMutualInformation2Dir( KeyPrefix + "vomi2Dir" );
const QString Experimental3DSettings::BestViewsDir( KeyPrefix + "bestViewsDir" );
const QString Experimental3DSettings::GuidedTourDir( KeyPrefix + "guidedTourDir" );
const QString Experimental3DSettings::ExploratoryTourDir( KeyPrefix + "exploratoryTourDir" );
const QString Experimental3DSettings::CameraDir( KeyPrefix + "cameraDir" );
const QString Experimental3DSettings::ObscuranceDir( KeyPrefix + "obscuranceDir" );
const QString Experimental3DSettings::ProgramDir( KeyPrefix + "programDir" );
const QString Experimental3DSettings::TourDir( KeyPrefix + "tourDir" );
const QString Experimental3DSettings::ViewpointVoxelMutualInformationDir( KeyPrefix + "viewpointVomiDir" );
const QString Experimental3DSettings::ColorVoxelMutualInformationPaletteDir( KeyPrefix + "colorVomiPaletteDir" );
const QString Experimental3DSettings::ColorVoxelMutualInformationDir( KeyPrefix + "colorVomiDir" );
const QString Experimental3DSettings::EnhancedViewpointMutualInformationOpacityDir( KeyPrefix + "evmiOpacityDir" );
const QString Experimental3DSettings::EnhancedViewpointMutualInformationVoxelMutualInformationDir( KeyPrefix + "evmiVomiDir" );

const QString Experimental3DSettings::ViewedVolumeIntensityDir( KeyPrefix + "viewedVolumeIDir" );
const QString Experimental3DSettings::ViewpointEntropyIntensityDir( KeyPrefix + "viewpointEntropyIDir" );
const QString Experimental3DSettings::EntropyIntensityDir( KeyPrefix + "entropyIDir" );
const QString Experimental3DSettings::ViewpointMutualInformationIntensityDir( KeyPrefix + "vmiiDir" );
const QString Experimental3DSettings::MutualInformationIntensityDir( KeyPrefix + "miiDir" );
const QString Experimental3DSettings::ViewpointUnstabilitiesIntensityDir( KeyPrefix + "viewpointUnstabilitiesIDir" );
const QString Experimental3DSettings::IntensityMutualInformationDir( KeyPrefix + "imiDir" );


Experimental3DSettings::Experimental3DSettings()
{
}

Experimental3DSettings::~Experimental3DSettings()
{
}

void Experimental3DSettings::init()
{    
}

} // end namespace udg
