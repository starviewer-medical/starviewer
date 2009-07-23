#include "mprsettings.h"

namespace udg {

// Definició de les claus
const QString KeyPrefix("Starviewer-App-MPR/");
const QString MPRSettings::HorizontalSplitterGeometry( KeyPrefix + "horizontalSplitter" );
const QString MPRSettings::VerticalSplitterGeometry( KeyPrefix + "verticalSplitter" );

MPRSettings::MPRSettings()
{
}

MPRSettings::~MPRSettings()
{
}

void MPRSettings::init()
{    
}

} // end namespace udg 