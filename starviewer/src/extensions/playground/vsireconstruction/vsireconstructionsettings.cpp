#include "vsireconstructionsettings.h"

namespace udg { 

const QString keyPrefix("StarViewer-App-VSIReconstruction/");

const QString VSIReconstructionSettings::HorizontalSplitterGeometry( keyPrefix + "horizontalSplitter" );
const QString VSIReconstructionSettings::VerticalSplitterGeometry( keyPrefix + "verticalSplitter" );
const QString VSIReconstructionSettings::HorizontalImageSplitterGeometry( keyPrefix + "horizontalImageSplitter" );
const QString VSIReconstructionSettings::VerticalImageSplitterGeometry( keyPrefix + "verticalImageSplitter" );

VSIReconstructionSettings::VSIReconstructionSettings()
{
}

VSIReconstructionSettings::~VSIReconstructionSettings()
{
}

void VSIReconstructionSettings::init()
{
}

} // end namespace udg
