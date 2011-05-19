
#ifndef UDGLOCALDATABASEUTILDAL_H
#define UDGLOCALDATABASEUTILDAL_H

#include "localdatabasebasedal.h"

class QString;

namespace udg {

/** Aquesta classe s'encarrega de dur a terme les operacions relacionades amb l'objecte estudi de la cache de l'aplicació.
*/
class LocalDatabaseUtilDAL : public LocalDatabaseBaseDAL {
public:

    LocalDatabaseUtilDAL(DatabaseConnection *dbConnection);

    /// Compacta la BD
    void compact();

    /// Retorna la revisió de la BD a la que està connectada, si no troba a quina revisió pertany retorna -1
    int getDatabaseRevision();

private :

    /// Ens retorna un string amb el select a executar per retorna la revisió de la base de dades sobre la qual estem connectats
    QString buildSqlGetDatabaseRevision();
};
}

#endif
