/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDELETEDIRECTORY_H
#define UDGDELETEDIRECTORY_H

class QString;

namespace udg {

/** Aquesta classe, esborra tot els fitxers i subdirectoris que contingui un directori
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DeleteDirectory{
public:

    DeleteDirectory();
    
    /** Esborrar el contingut del directori i el directori passat per paràmetres
     * @param directoryPath path del directori a esborrar 
     * @param deleteRootDirectory indica si s'ha d'esborrar només el contingu del directori o també el directori arrel passat per paràmetre. Si fals només s'esborra el contingut, si és cert s'esborra el contingut i el directori passat per paràmetre
     * @return indica si l'operacio s'ha realitzat amb èxit
     */
    bool deleteDirectory( QString directoryPath , bool deleteRootDirectory );

    ~DeleteDirectory();

};

}

#endif
