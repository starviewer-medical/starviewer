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

#include "dicomdumpdefaulttagsloader.h"
#include "dicomdumpdefaulttagsreader.h"
#include "dicomdumpdefaulttagsrepository.h"
#include "logging.h"
#include "starviewerapplication.h"
#include "coresettings.h"

#include <QFileInfo>
#include <QFileInfoList>

namespace udg {

DICOMDumpDefaultTagsLoader::DICOMDumpDefaultTagsLoader()
{
}

DICOMDumpDefaultTagsLoader::~DICOMDumpDefaultTagsLoader()
{
}

void DICOMDumpDefaultTagsLoader::loadDefaults()
{
    /// DICOM Dump Default Tags definits per defecte, agafa el directori de l'executable TODO això podria ser un setting més
    // Path linux
    QString defaultPath = "/etc/xdg/" + OrganizationNameString + "/" + ApplicationNameString + "/dicomdumpdefaulttags/";

    if (!QFile::exists(defaultPath))
    {
        defaultPath = installationPath() + "/dicomdumpdefaulttags/";
    }
    if (!QFile::exists(defaultPath))
    {
        defaultPath = sourcePath() + "/dicomdumpdefaulttags/";
    }

    if (QFile::exists(defaultPath))
    {
        INFO_LOG(QString("Directori a on es van a buscar els hanging protocols per defecte: %1").arg(defaultPath));
        loadXMLFiles(defaultPath);
    }
    else
    {
        INFO_LOG(QString("El directori per defecte dels dicom dump default tags files no existeix. No es carregaran."));
    }

    /// Default Tags definits per l'usuari
    Settings systemSettings;
    QString userPath = systemSettings.getValue(CoreSettings::UserDICOMDumpDefaultTagsPath).toString();
    if (!userPath.isEmpty())
    {
        loadXMLFiles(userPath);
    }
}

void DICOMDumpDefaultTagsLoader::loadXMLFiles(const QString &path)
{
    QFileInfo fileToRead(path);
    QStringList defaultTagsLoaded = loadXMLFiles(fileToRead);

    INFO_LOG(QString("DICOM Dump Default Tags carregats: %1").arg(defaultTagsLoaded.join(", ")));
}

QStringList DICOMDumpDefaultTagsLoader::loadXMLFiles(const QFileInfo &fileInfo)
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

QString DICOMDumpDefaultTagsLoader::loadXMLFile(const QFileInfo &fileInfo)
{
    DICOMDumpDefaultTagsReader dicomDumpDefaultTagsReader;
    DICOMDumpDefaultTags *dicomDumpDefaultTags = dicomDumpDefaultTagsReader.readFile(fileInfo.absoluteFilePath());

    if (dicomDumpDefaultTags != NULL)
    {
        DICOMDumpDefaultTagsRepository::getRepository()->addItem(dicomDumpDefaultTags);
        return fileInfo.fileName();
    }

    return QString();
}

}
