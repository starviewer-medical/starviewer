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
     * @return indica si l'operacio s'ha realitzat amb èxit
     */
    bool deleteDirectory( QString directoryPath );

    ~DeleteDirectory();

};

}

#endif
