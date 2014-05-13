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

#include "settingsaccesslevelfilereader.h"

#include "logging.h"
#include <QSettings>
#include <QStringList>

namespace udg {

SettingsAccessLevelFileReader::SettingsAccessLevelFileReader()
{
}

SettingsAccessLevelFileReader::~SettingsAccessLevelFileReader()
{
}

bool SettingsAccessLevelFileReader::read(const QString &filePath)
{
    bool ok = true;
    m_accessLevelTable.clear();

    QSettings settings(filePath, QSettings::IniFormat);
    switch (settings.status())
    {
        case QSettings::NoError:
        {
            QStringList list = settings.allKeys();
            QString fileVersion = settings.value("settingsAccessLevelVersion").toString();
            DEBUG_LOG("Versió d'arxiu 'settingsAccessLevel': " + fileVersion);
            if (!fileVersion.isEmpty())
            {
                list.removeAt(list.indexOf("settingsAccessLevelVersion"));
            }
            else
            {
                DEBUG_LOG("No file version specified for " + filePath + " file. Assuming v1.0");
                WARN_LOG("No file version specified for " + filePath + " file. Assuming v1.0");
                fileVersion = "1.0";
            }
            if (fileVersion == "1.0")
            {
                bool ok;
                QString value;
                Settings::AccessLevel accessLevel;
                foreach (const QString &key, list)
                {
                    value = settings.value(key).toString();
                    ok = true;
                    if (value == "user")
                    {
                        accessLevel = Settings::UserLevel;
                    }
                    else if (value == "system")
                    {
                        accessLevel = Settings::SystemLevel;
                    }
                    else
                    {
                        DEBUG_LOG("Valor inesperat d'accessLevel: " + value);
                        WARN_LOG("Valor inesperat d'accessLevel: " + value);
                        ok = false;
                    }
                    if (ok)
                    {
                        DEBUG_LOG(">>>>>>>>>>>>>Inserim la clau: " + key + " amb valor: " + value);
                        m_accessLevelTable.insert(key, accessLevel);
                    }
                }
            }
        }
            break;

        case QSettings::FormatError:
            ok = false;
            DEBUG_LOG("Error de format en l'arxiu: " + filePath + ". Possiblement el format no és el d'un .INI");
            break;

        case QSettings::AccessError:
            ok = false;
            DEBUG_LOG("Error d'accés amb el fitxer: " + filePath);
            break;
    }

    return ok;
}

QMap<QString, Settings::AccessLevel> SettingsAccessLevelFileReader::getAccessLevelTable() const
{
    return m_accessLevelTable;
}

} // End namespace udg
