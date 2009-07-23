#include "rectumsegmentationsettings.h"

namespace udg {

// Definició de les claus
const QString KeyPrefix("StarViewer-App-RectumSegmentation/");
const QString RectumSegmentationSettings::HorizontalSplitterGeometry( KeyPrefix + "horizontalSplitter" );
const QString RectumSegmentationSettings::VerticalSplitterGeometry( KeyPrefix + "verticalSplitter" );
const QString RectumSegmentationSettings::SavedMaskPath( KeyPrefix + "savingDirectory" );

RectumSegmentationSettings::RectumSegmentationSettings()
{
}

RectumSegmentationSettings::~RectumSegmentationSettings()
{
}

void RectumSegmentationSettings::init()
{    
}

} // end namespace udg 

