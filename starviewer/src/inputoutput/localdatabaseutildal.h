/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGLOCALDATABASEUTILDAL_H
#define UDGLOCALDATABASEUTILDAL_H

#include "localdatabasebasedal.h"

class QString;

namespace udg {

/** Aquesta classe s'encarrega de dur a terme les operacions relacionades amb l'objecte estudi de la cache de l'aplicació.
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class LocalDatabaseUtilDAL: public LocalDatabaseBaseDAL
{
public:

    LocalDatabaseUtilDAL(DatabaseConnection *dbConnection);

    ///Compacta la BD
    void compact();

    ///retorna la revisió de la BD a la que està connectada, si no troba a quina revisió pertany retorna -1
    int getDatabaseRevision();

    ///Comprova si la base de dades està corrumpuda
    bool isDatabaseCorrupted();

private :

    ///Ens retorna un string amb el select a executar per retorna la revisió de la base de dades sobre la qual estem connectats
    QString buildSqlGetDatabaseRevision();
};
}

#endif
