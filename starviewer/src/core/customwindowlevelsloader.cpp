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

#include "windowlevel.h"
#include "customwindowlevelsloader.h"
#include "customwindowlevelsreader.h"
#include "customwindowlevelsrepository.h"
#include "logging.h"
#include "starviewerapplication.h"
#include "coresettings.h"

#include <QString>
#include <QFileInfo>
#include <QApplication>
#include <QDir>
#include <QFileInfoList>

namespace udg {

CustomWindowLevelsLoader::CustomWindowLevelsLoader()
{
}

CustomWindowLevelsLoader::~CustomWindowLevelsLoader()
{
}

void CustomWindowLevelsLoader::loadDefaults()
{
    /// Custom Window Levels definits per l'usuari
    Settings systemSettings;
    QString userPath = systemSettings.getValue(CoreSettings::UserCustomWindowLevelsPath).toString();
    if (!userPath.isEmpty())
    {
        loadXMLFiles(userPath);
    }
}

void CustomWindowLevelsLoader::loadXMLFiles(const QString &path)
{
    QFileInfo fileToRead(path);
    QStringList customWindowLevelsLoaded = loadXMLFiles(fileToRead);

    INFO_LOG(QString("Fitxers que contenen Custom Window Levels carregats: %1").arg(customWindowLevelsLoaded.join(", ")));
}

QStringList CustomWindowLevelsLoader::loadXMLFiles(const QFileInfo &fileInfo)
{
    QStringList filesLoaded;

    if (fileInfo.isDir())
    {
        QDir directory(fileInfo.absoluteFilePath());
        foreach (const QFileInfo &file, directory.entryInfoList())
        {
            if ((file.fileName() != ".") && (file.fileName() != ".."))
            {
                filesLoaded << loadXMLFiles(file);
            }
        }
    }
    else
    {
        if (fileInfo.suffix() == "xml")
        {
            QString fileLoaded = loadXMLFile(fileInfo);
            if (!fileLoaded.isEmpty())
            {
                filesLoaded << fileLoaded;
            }
        }
    }
    return filesLoaded;
}

QString CustomWindowLevelsLoader::loadXMLFile(const QFileInfo &fileInfo)
{
    CustomWindowLevelsReader customWindowLevelsReader;
    QList<WindowLevel*> listOfCustomWindowLevels = customWindowLevelsReader.readFile(fileInfo.absoluteFilePath());

    CustomWindowLevelsRepository *repository = CustomWindowLevelsRepository::getRepository();
    foreach (WindowLevel *customWindowLevel, listOfCustomWindowLevels)
    {
        repository->addItem(customWindowLevel);
    }

    if (listOfCustomWindowLevels.count() > 0)
    {
        return fileInfo.fileName();
    }
    else
    {
        return QString();
    }
}

}
