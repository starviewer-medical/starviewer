#include "interfacesettings.h"
#include "settingsregistry.h"

namespace udg {

InterfaceSettings::InterfaceSettings()
{
}

InterfaceSettings::~InterfaceSettings()
{
}

void InterfaceSettings::init()
{
    SettingsRegistry *settingsRegistry = SettingsRegistry::instance();
}

} // end namespace udg 

