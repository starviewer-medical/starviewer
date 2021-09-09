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

#include "settings.h"

#include "persistentsettings.h"

#include <QHeaderView>
#include <QSplitter>
#include <QTreeWidget>
#include <QVariant>

namespace udg {

SettingsInterface *Settings::m_staticTestingSettings = nullptr;

Settings::Settings()
{
    if (m_staticTestingSettings) {
        m_settings = m_staticTestingSettings;
    }
    else {
        m_settings = new PersistentSettings();
    }
}

Settings::~Settings()
{
    if (m_settings != m_staticTestingSettings) {
        delete m_settings;
    }
}

QVariant Settings::getValue(const QString &key) const
{
    return m_settings->getValue(key);
}

void Settings::setValue(const QString &key, const QVariant &value)
{
    m_settings->setValue(key, value);
}

bool Settings::contains(const QString &key) const
{
    return m_settings->contains(key);
}

void Settings::remove(const QString &key)
{
    m_settings->remove(key);
}

QStringList Settings::getValueAsQStringList(const QString &key, const QString &separator) const
{
    return getValue(key).toString().split(separator, QString::SkipEmptyParts);
}

Settings::SettingListType Settings::getList(const QString &key)
{
    return m_settings->getList(key);
}

void Settings::setList(const QString &key, const SettingListType &list)
{
    m_settings->setList(key, list);
}

void Settings::addListItem(const QString &key, const SettingsListItemType &item)
{
    m_settings->addListItem(key, item);
}

void Settings::setListItem(int index, const QString &key, const SettingsListItemType &item)
{
    m_settings->setListItem(index, key, item);
}

void Settings::removeListItem(const QString &key, int index)
{
    m_settings->removeListItem(key, index);
}

void Settings::saveColumnsWidths(const QString &key, const QTreeWidget *treeWidget)
{
    Q_ASSERT(treeWidget);

    int columnCount = treeWidget->columnCount();

    for (int column = 0; column < columnCount; column++)
    {
        QString columnKey = key + "/columnWidth" + QString::number(column);
        this->setValue(columnKey, treeWidget->columnWidth(column));
    }
}

void Settings::restoreColumnsWidths(const QString &key, QTreeWidget *treeWidget)
{
    Q_ASSERT(treeWidget);

    int columnCount = treeWidget->columnCount();

    for (int column = 0; column < columnCount; column++)
    {
        QString columnKey = key + "/columnWidth" + QString::number(column);

        if (!this->contains(columnKey))
        {
            treeWidget->resizeColumnToContents(column);
        }
        else
        {
            treeWidget->header()->resizeSection(column, this->getValue(columnKey).toInt());
        }
    }
}

void Settings::saveGeometry(const QString &key, const QWidget *widget)
{
    Q_ASSERT(widget);
    this->setValue(key, widget->saveGeometry());
}

void Settings::restoreGeometry(const QString &key, QWidget *widget)
{
    Q_ASSERT(widget);
    widget->restoreGeometry(this->getValue(key).toByteArray());
}

void Settings::saveGeometry(const QString &key, const QSplitter *splitter)
{
    Q_ASSERT(splitter);
    this->setValue(key, splitter->saveState());
}

void Settings::restoreGeometry(const QString &key, QSplitter *splitter)
{
    Q_ASSERT(splitter);
    splitter->restoreState(this->getValue(key).toByteArray());
}

void Settings::setStaticTestingSettings(SettingsInterface *settings) {
    m_staticTestingSettings = settings;
}

}  // End namespace udg
