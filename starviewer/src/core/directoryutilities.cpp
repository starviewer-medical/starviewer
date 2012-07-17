#include "directoryutilities.h"

#include "logging.h"

#include <QDir>
#include <QCoreApplication>

namespace udg {

DirectoryUtilities::DirectoryUtilities()
{
}

DirectoryUtilities::~DirectoryUtilities()
{
}

bool DirectoryUtilities::deleteDirectory(const QString &directoryPath, bool deleteRootDirectory)
{
    bool result;
    result = removeDirectory(QDir(directoryPath), deleteRootDirectory);

    if (!result)
    {
        if (deleteRootDirectory)
        {
            ERROR_LOG("Hi ha hagut errors en l'esborrat del directori i del seu contingut de " + directoryPath);
        }
        else
        {
            ERROR_LOG("Hi ha hagut errors en l'esborrat del contingut del directori " + directoryPath);
        }
    }
    return result;
}

bool DirectoryUtilities::copyDirectory(const QString &sourceDirectoryPath, const QString &destinationDirectoryPath)
{
    QDir sourceDirectory(sourceDirectoryPath), destinationDirectory(destinationDirectoryPath);
    QStringList files, directories;

    if (!sourceDirectory.exists())
    {
        ERROR_LOG(QString("El directori origen %1 no existeix").arg(sourceDirectoryPath));
        return false;
    }

    if (!destinationDirectory.exists())
    {
        if (!destinationDirectory.mkdir(destinationDirectoryPath))
        {
            ERROR_LOG("No s'ha pogut crear el directori " + destinationDirectoryPath);
            return false;
        }
    }

    // Copiem els fitxers del directori
    files = sourceDirectory.entryList(QDir::Files);
    for (int i = 0; i < files.count(); i++)
    {
        QString sourceFile = sourceDirectoryPath + QDir::separator() + files[i];
        QString destinationFile = destinationDirectoryPath + QDir::separator() + files[i];

        if (!QFile::copy(sourceFile, destinationFile))
        {
            ERROR_LOG(QString("No s'ha pogut copiar l'arxiu %1 al seu destí %2").arg(sourceFile, destinationFile));
            return false;
        }
    }

    // Copiem els subdirectoris
    directories = sourceDirectory.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < directories.count(); i++)
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

bool DirectoryUtilities::isDirectoryEmpty(const QString &path)
{
    QDir dir(path);

    return dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files).count() == 0;
}

bool DirectoryUtilities::removeDirectory(const QDir &dir, bool deleteRootDirectory)
{
    bool ok = true;
    // QDir::NoDotAndDotDot
    if (dir.exists())
    {
        QFileInfoList entries = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
        int count = entries.size();
        for (int i = 0; i < count && ok; i++)
        {
            QFileInfo entryInfo = entries[i];
            QString path = entryInfo.absoluteFilePath();
            if (entryInfo.isDir())
            {
                ok = removeDirectory(QDir(path), true);
            }
            else
            {
                QFile file(path);
                if (!file.remove())
                {
                    ok = false;
                    ERROR_LOG("No s'ha pogut esborrar el fitxer " + path);
                }
                else
                {
                    emit directoryDeleted();
                    QCoreApplication::processEvents();
                }
            }
        }

        if (deleteRootDirectory)
        {
            if (!dir.rmdir(dir.absolutePath()))
            {
                ok = false;
                ERROR_LOG("No s'ha pogut esborrar el directori " + dir.absolutePath());
            }
        }
    }

    return ok;
}

}
