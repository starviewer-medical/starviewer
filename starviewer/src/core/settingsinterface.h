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

#ifndef SETTINGSINTERFACE_H
#define SETTINGSINTERFACE_H

#include "settings.h"

class QString;
class QVariant;

namespace udg {

/**
 *  Common interface for classes that implement settings.
 */
class SettingsInterface
{
public:
    virtual ~SettingsInterface();

    /// Returns the value of the setting identified by the given key. If the value is not set, returns the default value of the setting.
    virtual QVariant getValue(const QString &key) const = 0;

    /// Sets the value of the setting identified by the given key.
    virtual void setValue(const QString &key, const QVariant &value) = 0;

    /// Returns true if the setting identified by the given key has a set value, and false otherwise.
    virtual bool contains(const QString &key) const = 0;

    /// Unsets the value of the setting identified by the given key.
    virtual void remove(const QString &key) = 0;

    // Methods to manage settings representing lists

    /// Returns true if there is a list under the given key, and false otherwise.
    virtual bool containsList(const QString &key) const = 0;

    /// Returns a list stored with the given key.
    virtual Settings::SettingListType getList(const QString &key) = 0;

    /// Stores the given list of settings under the given key, overwriting a previous list if present.
    virtual void setList(const QString &key, const Settings::SettingListType &list) = 0;

    /// Adds settings list item (a map) to the list stored with the given key.
    virtual void addListItem(const QString &key, const Settings::SettingsListItemType &item) = 0;

    /// Sets the value of the settings list item (a map) at the given index in the list stored with the given key.
    virtual void setListItem(int index, const QString &key, const Settings::SettingsListItemType &item) = 0;

    /// Removes the settings list item (a map) at the given index from the list stored with the given key.
    virtual void removeListItem(const QString &key, int index) = 0;
};

} // namespace udg

#endif
