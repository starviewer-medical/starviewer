/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDELETEDIRECTORY_H
#define UDGDELETEDIRECTORY_H

#include <QObject>

class QString;
class QDir;

namespace udg {

/** Aquesta classe, esborra tot els fitxers i subdirectoris que contingui un directori
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DeleteDirectory : public QObject
{
Q_OBJECT
public:

    DeleteDirectory();
    ~DeleteDirectory();

    /** Esborrar el contingut del directori i el directori passat per paràmetres
     * @param directoryPath path del directori a esborrar 
     * @param deleteRootDirectory indica si s'ha d'esborrar només el contingu del directori o també el directori arrel passat per paràmetre. Si fals només s'esborra el contingut, si és cert s'esborra el contingut i el directori passat per paràmetre
     * @return indica si l'operacio s'ha realitzat amb èxit
     */
    bool deleteDirectory( QString directoryPath , bool deleteRootDirectory );

    ///Ens indica si un directori està buit
    /*TODO:No és el millor lloc per posar aquest mètode, potser s'hauria de fer una classe tipus helper amb mètodes per accedir al sistema de fitxers
        que ajunti aquesta classe i la CopyDirectory*/
    bool isDirectoryEmpty(const QString &directoryPath );

signals:
    void directoryDeleted();

private:
    bool removeDirectory(const QDir &dir, bool deleteRootDirectory);

};

}

#endif
