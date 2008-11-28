/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include <QString>
#include <QFile>
#include <QDir>
#include <QCoreApplication>

#include "deletedirectory.h"
#include "logging.h"

namespace udg {

DeleteDirectory::DeleteDirectory()
{
}

/**
 *
 * @param directoryPath
 * @return
 */
bool DeleteDirectory::deleteDirectory(QString directoryPath, bool deleteRootDirectory)
{
    QStringList filesList, directoryList;
    QString absoluteFilePath , absoluteDirectoryPath;
    QDir directory, directoryToDelete;
    QFile fileToDelete;

    //si ens envien un directori amb la '/' al final la trèiem
    if ( directoryPath.at( directoryPath.length() - 1) == '/' )
    {
        directoryPath = directoryPath.remove ( directoryPath.length() - 1 , 1 );
    }

    directory.setPath( directoryPath );

	//Si el directori no existeix sortim
	if (!directory.exists())
	{
		return true;
	}

    filesList =  directory.entryList( QDir::Files );//llista de fitxers del directori
    for ( QStringList::Iterator it = filesList.begin(); it != filesList.end(); ++it )//per cada fitxer
    {
        if ( *it != "." && *it != ".." )
        {
            absoluteFilePath = directoryPath + "/" + (*it);
            if ( !fileToDelete.remove (absoluteFilePath ) )  //esborrem el fitxer
            {
                ERROR_LOG("NO S'HA POGUT ESBORRAR EL FITXER " + absoluteFilePath);            
                return false;
            }
        }
    }

    directoryList =  directory.entryList( QDir::AllDirs );//obtenim llistat de subdirectoris
    for ( QStringList::Iterator it = directoryList.begin(); it != directoryList.end(); ++it ) //per cada subdirectori
    {
        if ( *it != "." && *it != ".." )
        {
            absoluteDirectoryPath = directoryPath + "/" + (*it);
            if ( !deleteDirectory( absoluteDirectoryPath , true ) ) return false; //invoquem el mateix mètode per a que esborri el subdirectori ( recursivitat )
            emit directoryDeleted();
            QCoreApplication::processEvents();
        }
    }

    if ( deleteRootDirectory )
    {
        if ( !directoryToDelete.rmdir( directoryPath ) ) //esborra el directori arrel
        {
            ERROR_LOG("NO S'HA POGUT ESBORRAR EL DIRECTORI " + directoryPath);
            return false;
        }
    }
    return true;
}

DeleteDirectory::~DeleteDirectory()
{
}

}
