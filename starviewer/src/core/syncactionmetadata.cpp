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

#include "syncactionmetadata.h"

namespace udg {

SyncActionMetaData::SyncActionMetaData()
{
}

SyncActionMetaData::SyncActionMetaData(const QString &name, const QString &interfaceName, const QString &settingsName)
{
    m_name = name;
    m_userInterfaceName = interfaceName;
    m_settingsName = settingsName;
}

SyncActionMetaData::~SyncActionMetaData()
{
}

QString SyncActionMetaData::getName() const
{
    return m_name;
}

QString SyncActionMetaData::getUserInterfaceName() const
{
    return m_userInterfaceName;
}

QString SyncActionMetaData::getSettingsName() const
{
    return m_settingsName;
}

bool SyncActionMetaData::isEmpty() const
{
    return m_name.isEmpty() && m_userInterfaceName.isEmpty() && m_settingsName.isEmpty();
}

bool SyncActionMetaData::operator <(const SyncActionMetaData &syncActionMetaData) const
{
    return m_name < syncActionMetaData.m_name;
}

bool SyncActionMetaData::operator ==(const SyncActionMetaData &syncActionMetaData) const
{
    return m_name == syncActionMetaData.m_name &&
        m_userInterfaceName == syncActionMetaData.m_userInterfaceName &&
        m_settingsName == syncActionMetaData.m_settingsName;
}

} // End namespace udg
