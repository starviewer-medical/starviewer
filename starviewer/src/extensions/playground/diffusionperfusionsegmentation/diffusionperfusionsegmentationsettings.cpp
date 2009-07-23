#include "diffusionperfusionsegmentationsettings.h"

namespace udg {

// Definició de les claus
const QString KeyPrefix("StarViewer-App-DiffusionPerfusionSegmentation/");
const QString DiffusionPerfusionSegmentationSettings::HorizontalSplitterGeometry( KeyPrefix + "horizontalSplitter" );
const QString DiffusionPerfusionSegmentationSettings::VerticalSplitterGeometry( KeyPrefix + "verticalSplitter" );
const QString DiffusionPerfusionSegmentationSettings::SavedMaskPath( KeyPrefix + "savingDirectory" );

DiffusionPerfusionSegmentationSettings::DiffusionPerfusionSegmentationSettings()
{
}

DiffusionPerfusionSegmentationSettings::~DiffusionPerfusionSegmentationSettings()
{
}

void DiffusionPerfusionSegmentationSettings::init()
{    
}

} // end namespace udg 