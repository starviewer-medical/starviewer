/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "deletedirectory.h"
#include "logging.h"

#include <QDir>
#include <QCoreApplication>

namespace udg {

DeleteDirectory::DeleteDirectory()
{
}

DeleteDirectory::~DeleteDirectory()
{
}

bool DeleteDirectory::deleteDirectory(QString directoryPath, bool deleteRootDirectory)
{
    bool result;
    result = removeDirectory( QDir(directoryPath), deleteRootDirectory );

    if( !result )
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

bool DeleteDirectory::isDirectoryEmpty(const QString &path)
{
    QDir dir(path);

    return dir.entryInfoList( QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files ).count() == 0;
}

bool DeleteDirectory::removeDirectory(const QDir &dir, bool deleteRootDirectory )
{
    bool ok = true;
    if ( dir.exists() )//QDir::NoDotAndDotDot
    {
        QFileInfoList entries = dir.entryInfoList( QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files );
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
                if ( !file.remove() )
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

        if( deleteRootDirectory )
        {
            if ( !dir.rmdir(dir.absolutePath()) )
            {
                ok = false;
                ERROR_LOG("No s'ha pogut esborrar el directori " + dir.absolutePath());
            }
        }
    }

    return ok;
}

}
