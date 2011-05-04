#ifndef UDGCUSTOMWINDOWLEVELSLOADER_CPP
#define UDGCUSTOMWINDOWLEVELSLOADER_CPP

#include "customwindowlevel.h"
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
    QList<CustomWindowLevel*> listOfCustomWindowLevels = customWindowLevelsReader.readFile(fileInfo.absoluteFilePath());

    CustomWindowLevelsRepository *repository = CustomWindowLevelsRepository::getRepository();
    foreach (CustomWindowLevel* customWindowLevel, listOfCustomWindowLevels)
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
#endif
