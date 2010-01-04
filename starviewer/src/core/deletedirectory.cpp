/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "deletedirectory.h"
#include "logging.h"

#include <QDir>

namespace udg {

DeleteDirectory::DeleteDirectory()
{
}

bool DeleteDirectory::deleteDirectory(QString directoryPath, bool deleteRootDirectory)
{
    bool result;
    result = removeDirectory( QDir(directoryPath), deleteRootDirectory );

    if( result )
        DEBUG_LOG("S'han esborrat");
    else
        DEBUG_LOG("Hi ha hagut errors en l'esborrat");

    return result;
}

DeleteDirectory::~DeleteDirectory()
{
}

bool DeleteDirectory::removeDirectory(const QDir &dir, bool deleteRootDirectory )
{
    bool failed = false;
    if ( dir.exists() )//QDir::NoDotAndDotDot
    {
        QFileInfoList entries = dir.entryInfoList( QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files );
        int count = entries.size();
        for (int i = 0; i < count && !failed; i++)
        {
            QFileInfo entryInfo = entries[i];
            QString path = entryInfo.absoluteFilePath();
            if (entryInfo.isDir())
            {
                failed = !removeDirectory(QDir(path),true);
            }
            else
            {
                QFile file(path);
                if ( !file.remove() )
                    failed = true;
            }
        }

        if( deleteRootDirectory )
        {
            if ( !dir.rmdir(dir.absolutePath()) )
                failed = true;   
        }
    }

    return(!failed);
}

}
