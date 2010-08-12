/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef UDGLOCALDATABASEBASEDAL_H
#define UDGLOCALDATABASEBASEDAL_H

class QString;

namespace udg {

class DatabaseConnection;

/** Classe base de les que hereden totes les classes que implementen una DAL per accés a dades
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class LocalDatabaseBaseDAL
{
public:
    LocalDatabaseBaseDAL(DatabaseConnection *dbConnection);

    ///Retorna l'últim error produït
    int getLastError();

protected:
    ///Ens fa un ErrorLog d'una sentència sql. No es té en compte l'error és SQL_CONSTRAINT (clau duplicada)
    void logError(const QString &sqlSentence);

protected:
    int m_lastSqliteError;
    DatabaseConnection *m_dbConnection;

};
}

#endif
