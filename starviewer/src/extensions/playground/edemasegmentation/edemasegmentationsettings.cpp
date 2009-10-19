#include "edemasegmentationsettings.h"

namespace udg {

// Definició de les claus
const QString KeyPrefix("StarViewer-App-EdemaSegmentation/");

const QString EdemaSegmentationSettings::HorizontalSplitterGeometry( KeyPrefix + "horizontalSplitter" );
const QString EdemaSegmentationSettings::VerticalSplitterGeometry( KeyPrefix + "verticalSplitter" );
const QString EdemaSegmentationSettings::SavedMaskPath( KeyPrefix + "savingDirectory" );

EdemaSegmentationSettings::EdemaSegmentationSettings()
{
}

EdemaSegmentationSettings::~EdemaSegmentationSettings()
{
}

void EdemaSegmentationSettings::init()
{    
}

} // end namespace udg 