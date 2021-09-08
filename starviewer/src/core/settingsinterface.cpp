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

#include "settingsinterface.h"

#include <stdexcept>

#include <QList>
#include <QMap>

namespace udg {

SettingsInterface::~SettingsInterface()
{
}

Settings::SettingListType SettingsInterface::getList(const QString&)
{
    throw std::runtime_error("Not implemented");
}

void SettingsInterface::setList(const QString&, const Settings::SettingListType&)
{
    throw std::runtime_error("Not implemented");
}

void SettingsInterface::addListItem(const QString&, const Settings::SettingsListItemType&)
{
    throw std::runtime_error("Not implemented");
}

void SettingsInterface::setListItem(int, const QString&, const Settings::SettingsListItemType&)
{
    throw std::runtime_error("Not implemented");
}

void SettingsInterface::removeListItem(const QString&, int)
{
    throw std::runtime_error("Not implemented");
}

}
