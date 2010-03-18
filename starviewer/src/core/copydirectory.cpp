#include "copydirectory.h"

#include <QDir>
#include <QStringList>
#include <QFile>

#include "deletedirectory.h"
#include "logging.h"

namespace udg
{

bool CopyDirectory::copyDirectory(const QString &sourceDirectoryPath, const QString &destinationDirectoryPath)
{
    QDir sourceDirectory(sourceDirectoryPath), destinationDirectory(destinationDirectoryPath);
    QStringList files, directories;

    if(!sourceDirectory.exists())
    {
        ERROR_LOG(QString("El directori origen %1 no existeix").arg(sourceDirectoryPath));
        return false;
    }

    if(!destinationDirectory.exists())
    {
        if (!destinationDirectory.mkdir(destinationDirectoryPath))
        {
            ERROR_LOG("No s'ha pogut crear el directori " + destinationDirectoryPath);
            return false;
        }
    }

    //Copiem els fitxers del directori
    files = sourceDirectory.entryList(QDir::Files);
    for(int i = 0; i < files.count(); i++)
    {
        QString sourceFile = sourceDirectoryPath + QDir::separator() + files[i];
        QString destinationFile = destinationDirectoryPath + QDir::separator() + files[i];

        if (!QFile::copy(sourceFile, destinationFile))
        {
            ERROR_LOG(QString("No s'ha pogut copiar l'arxiu %1 al seu destí %2").arg(sourceFile, destinationFile));
            return false;
        }
    }

    //Copiem els subdirectoris
    directories = sourceDirectory.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for(int i = 0; i < directories.count(); i++)
    {
        QString sourceSubDirectory = sourceDirectoryPath + QDir::separator() + directories[i];
        QString destinationSubDirectory = destinationDirectoryPath + QDir::separator() + directories[i];
        if (!copyDirectory(sourceSubDirectory, destinationSubDirectory))
        {
            return false;
        }
    }

    return true;
}

}