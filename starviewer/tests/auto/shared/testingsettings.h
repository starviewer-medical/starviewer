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
    /// Initializes the testing settings with default values.
    TestingSettings();

    /// Returns the value of the setting identified by the given key. If the value is not set, returns the default value of the setting.
    QVariant getValue(const QString &key) const override;

    /// Sets the value of the setting identified by the given key.
    void setValue(const QString &key, const QVariant &value) override;

    /// Returns true if the setting identified by the given key has a set value, and false otherwise.
    bool contains(const QString &key) const override;

    /// Unsets the value of the setting identified by the given key.
    void remove(const QString &key) override;

    // Methods to manage settings representing lists

    /// Returns a list stored with the given key.
    Settings::SettingListType getList(const QString &key) override;

    /// Stores the given list of settings under the given key, overwriting a previous list if present.
    void setList(const QString &key, const Settings::SettingListType &list) override;

    /// Adds settings list item (a map) to the list stored with the given key.
    void addListItem(const QString &key, const Settings::SettingsListItemType &item) override;

    /// Sets the value of the settings list item (a map) at the given index in the list stored with the given key.
    void setListItem(int index, const QString &key, const Settings::SettingsListItemType &item) override;

    /// Removes the settings list item (a map) at the given index from the list stored with the given key.
    void removeListItem(const QString &key, int index) override;

    bool operator==(const TestingSettings &that) const;

private:
    /// A map that stores the settings to memory.
    QMap<QString, QVariant> m_settings;

};

}

#endif // TESTINGSETTINGS_H
