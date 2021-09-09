/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDG_PERSISTENTSETTINGS_H
#define UDG_PERSISTENTSETTINGS_H

#include "settingsinterface.h"

#include "settings.h"

#include <QMap>

class QSettings;

namespace udg {

/**
 * @brief The PersistentSettings class implements SettingsInterface to access persistent settings through QSettings.
 *
 * It's not intended to be used directly, use Settings instead.
 */
class PersistentSettings : public SettingsInterface
{
public:
    PersistentSettings();
    ~PersistentSettings() override;

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

private:
    /// Returns the appropriate QSettings instance (user or system) for the given key.
    QSettings* getSettingsObject(const QString &key) const;

private:
    /// QSettings instances for user level and system level access.
    QMap<int, QSettings*> m_qsettingsObjectsMap;
};

} // namespace udg

#endif // UDG_PERSISTENTSETTINGS_H
