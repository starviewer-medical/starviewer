#ifndef UDGDICOMDUMPDEFAULTTAGSLOADER_CPP
#define UDGDICOMDUMPDEFAULTTAGSLOADER_CPP

#include "dicomdumpdefaulttagsloader.h"
#include "dicomdumpdefaulttagsreader.h"
#include "dicomdumpdefaulttagsrepository.h"
#include "logging.h"
#include "starviewerapplication.h"
#include "coresettings.h"

#include <QString>
#include <QFileInfo>
#include <QApplication>
#include <QDir>
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
    QString defaultPath = "/etc/xdg/" + OrganizationNameString + "/" + ApplicationNameString + "/dicomdumpdefaulttags/"; // Path linux
    
    if (!QFile::exists(defaultPath))
    {    
        defaultPath = qApp->applicationDirPath() + "/dicomdumpdefaulttags/";
    }
    if (!QFile::exists(defaultPath))
    {
        // En entorn de desenvolupament Windows & Linux
        defaultPath = qApp->applicationDirPath() + "/../dicomdumpdefaulttags/";
    }
    if (!QFile::exists(defaultPath))
    {
        // En entorn de desenvolupament Mac OS X
        defaultPath = qApp->applicationDirPath() + "/../../../../dicomdumpdefaulttags/";
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
#endif
