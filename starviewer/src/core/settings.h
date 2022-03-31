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

#ifndef UDGSETTINGS_H
#define UDGSETTINGS_H

#include <QFlags>
#include <QString>
#include <QVariant> // included for convenience: any class that reads or writes settings will need to include it anyway

class QSplitter;
class QWidget;

namespace udg {

class SettingsInterface;

/**
 * @brief The Settings class allows to read and write settings.
 *
 * The correct way to use it is to declare it locally in the method where settings access is needed, not declare it as a member.
 *
 * When testing you should call setStaticTestingSettings() with a TestingSettings instance if you want to avoid reading or writing real settings.
 */
class Settings
{
public:
    enum AccessLevel { UserLevel, SystemLevel };
    enum PropertiesFlag { None = 0x0, Parseable = 0x1 };
    Q_DECLARE_FLAGS(Properties, PropertiesFlag)

    Settings();
    ~Settings();

    /// Returns the value of the setting identified by the given key. If the value is not set, returns the default value of the setting.
    QVariant getValue(const QString &key) const;

    /// Sets the value of the setting identified by the given key.
    void setValue(const QString &key, const QVariant &value);

    /// Returns true if the setting identified by the given key has a set value, and false otherwise.
    bool contains(const QString &key) const;

    /// Unsets the value of the setting identified by the given key.
    void remove(const QString &key);

    /// Helper method to return a value as a QStringList in case values are convertible to string and the adequate values separator is used.
    QStringList getValueAsQStringList(const QString &key, const QString &separator = ";") const;

    // Methods to manage settings representing lists. Lists do not contain values directly, but instead have a map at each position.

    // Note: equivalent to QSettings::SettingsMap
    typedef QMap<QString, QVariant> SettingsListItemType;
    typedef QList<SettingsListItemType> SettingListType;

    /// Returns true if there is a list under the given key, and false otherwise.
    bool containsList(const QString &key) const;

    /// Returns a list stored with the given key.
    SettingListType getList(const QString &key);

    /// Stores the given list of settings under the given key, overwriting a previous list if present.
    void setList(const QString &key, const SettingListType &list);

    /// Adds settings list item (a map) to the list stored with the given key.
    void addListItem(const QString &key, const SettingsListItemType &item);

    /// Sets the value of the settings list item (a map) at the given index in the list stored with the given key.
    void setListItem(int index, const QString &key, const SettingsListItemType &item);

    /// Removes the settings list item (a map) at the given index from the list stored with the given key.
    void removeListItem(const QString &key, int index);

    // Methods to save and restore geometry of some widgets

    /// Saves column widths from the given widget (QTableView/Widget or QTreeView/Widget) under the given key. The width of each column is stored under the
    /// subkey columnWidthX, with X being the column number.
    template<class C>
    void saveColumnsWidths(const QString &key, const C *widget);
    /// Restores column widths to the given widget (QTableView/Widget or QTreeView/Widget) from values under the given key.
    template<class C>
    void restoreColumnsWidths(const QString &key, C *widget);

    /// Saves the geometry of the given widget under the given key.
    void saveGeometry(const QString &key, const QWidget *widget);
    /// Restores the geometry of the given widget stored under the given key.
    void restoreGeometry(const QString &key, QWidget *widget);

    /// Saves the geometry of the given splitter under the given key.
    void saveGeometry(const QString &key, const QSplitter *splitter);
    /// Restores the geometry of the given splitter stored under the given key.
    void restoreGeometry(const QString &key, QSplitter *splitter);

    /// Sets the settings object for testing.
    static void setStaticTestingSettings(SettingsInterface *settings);

private:
    /// Settings object that will be used.
    SettingsInterface *m_settings;

    /// To be used when testing to avoid used real settings.
    static SettingsInterface *m_staticTestingSettings;
};

} // End namespace udg

Q_DECLARE_OPERATORS_FOR_FLAGS(udg::Settings::Properties)

#include "settings.tpp"

#endif
