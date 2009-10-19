#include "experimental3dsettings.h"

namespace udg {

// Definició de les claus
const QString KeyPrefix("Experimental3D/");

const QString Experimental3DSettings::ViewPointEntropyDir( KeyPrefix + "viewpointEntropyDir" );
const QString Experimental3DSettings::EntropyDir( KeyPrefix + "entropyDir" );
const QString Experimental3DSettings::ViewpointUnstabilitiesDir( KeyPrefix + "viewpointUnstabilitiesDir" );
const QString Experimental3DSettings::TransferFunctionDir( KeyPrefix + "transferFunctionDir" );
const QString Experimental3DSettings::BestViewsDir( KeyPrefix + "bestViewsDir" );
const QString Experimental3DSettings::GuidedTourDir( KeyPrefix + "guidedTourDir" );
const QString Experimental3DSettings::ExploratoryTourDir( KeyPrefix + "exploratoryTourDir" );
const QString Experimental3DSettings::CameraDir( KeyPrefix + "cameraDir" );
const QString Experimental3DSettings::ObscuranceDir( KeyPrefix + "obscuranceDir" );
const QString Experimental3DSettings::ProgramDir( KeyPrefix + "programDir" );
const QString Experimental3DSettings::ViewedVolumeDir( KeyPrefix + "viewedVolumeDir" );
const QString Experimental3DSettings::TourDir( KeyPrefix + "tourDir" );
const QString Experimental3DSettings::ViewpointMutualInformationDir( KeyPrefix + "vmiDir" );
const QString Experimental3DSettings::MutualInformationDir( KeyPrefix + "miDir" );
const QString Experimental3DSettings::VoxelMutualInformationDir( KeyPrefix + "vomiDir" );
const QString Experimental3DSettings::ViewpointVoxelMutualInformationDir( KeyPrefix + "viewpointVomiDir" );
const QString Experimental3DSettings::ColorVoxelMutualInformationPaletteDir( KeyPrefix + "colorVomiPaletteDir" );
const QString Experimental3DSettings::ColorVoxelMutualInformationDir( KeyPrefix + "colorVomiDir" );
const QString Experimental3DSettings::EnhancedViewpointMutualInformationOpacityDir( KeyPrefix + "evmiOpacityDir" );
const QString Experimental3DSettings::EnhancedViewpointMutualInformationVoxelMutualInformationDir( KeyPrefix + "evmiVomiDir" );


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