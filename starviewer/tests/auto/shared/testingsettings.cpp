#include "testingsettings.h"

#include "coresettings.h"
#include "inputoutputsettings.h"
#include "interfacesettings.h"
#include "mprsettings.h"
#include "settingsregistry.h"
#include "q2dviewersettings.h"
#include "q3dviewerextensionsettings.h"

namespace testing {

TestingSettings::TestingSettings()
{
    CoreSettings().init();
    InputOutputSettings().init();
    InterfaceSettings().init();
    MPRSettings().init();
    Q2DViewerSettings().init();
    Q3DViewerExtensionSettings().init();
}

QVariant TestingSettings::getValue(const QString &key) const
{
    if (this->contains(key))
    {
        return m_settings[key];
    }
    else
    {
        return SettingsRegistry::instance()->getDefaultValue(key);
    }
}

void TestingSettings::setValue(const QString &key, const QVariant &value)
{
    m_settings[key] = value;
}

bool TestingSettings::contains(const QString &key) const
{
    return m_settings.contains(key);
}

void TestingSettings::remove(const QString &key)
{
    m_settings.remove(key);
}

}
