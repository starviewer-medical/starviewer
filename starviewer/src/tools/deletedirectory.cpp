/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include <QString>
#include <QFile>
#include <QDir>

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
bool DeleteDirectory::deleteDirectory(QString directoryPath)
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
   
    filesList =  directory.entryList( QDir::Files );//llista de fitxers del directori
    for ( QStringList::Iterator it = filesList.begin(); it != filesList.end(); ++it )//per cada fitxer
    {
        if ( *it != "." && *it != ".." )
        {
            absoluteFilePath.truncate( 0 );
            absoluteFilePath.append( directoryPath );       
            absoluteFilePath.append( "/" );
            absoluteFilePath.append( *it );
            if ( !fileToDelete.remove (absoluteFilePath ) )  //esborrem el fitxer
            {
                return false;
            }
        }
    }

    directoryList =  directory.entryList( QDir::AllDirs );//obtenim llistat de subdirectoris
    for ( QStringList::Iterator it = directoryList.begin(); it != directoryList.end(); ++it ) //per cada subdirectori
    {
        if ( *it != "." && *it != ".." )
        {
            absoluteDirectoryPath.truncate( 0 );
            absoluteDirectoryPath.append( directoryPath );          
            absoluteDirectoryPath.append( "/" );
            absoluteDirectoryPath.append( *it );
            if ( !deleteDirectory( absoluteDirectoryPath ) ) return false; //invoquem el mateix mètode per a que esborri el subdirectori ( recursivitat )
        }
    }

    if ( !directoryToDelete.rmdir( directoryPath ) ) //esborra el directori pare
    {
        return false;
    }
    else return true;
}

DeleteDirectory::~DeleteDirectory()
{
}

}
