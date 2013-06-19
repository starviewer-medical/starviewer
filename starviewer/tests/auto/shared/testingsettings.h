#ifndef TESTINGSETTINGS_H
#define TESTINGSETTINGS_H

#include "settingsinterface.h"

#include <QMap>

using namespace udg;

namespace testing {

/**
 * Settings class for use in unit tests. Allows to get default values and get/set values for testing.
 */
class TestingSettings : public SettingsInterface {

public:

    TestingSettings();

    virtual QVariant getValue(const QString &key) const;
    virtual void setValue(const QString &key, const QVariant &value);
    virtual bool contains(const QString &key) const;
    virtual void remove(const QString &key);

private:

    QMap<QString, QVariant> m_settings;

};

}

#endif // TESTINGSETTINGS_H
