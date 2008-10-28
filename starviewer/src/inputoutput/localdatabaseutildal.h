/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef UDGLOCALDATABASEUTILDAL_H
#define UDGLOCALDATABASEUTILDAL_H

class QString;

namespace udg {

class DatabaseConnection;

/** Aquesta classe s'encarrega de dur a terme les operacions relacionades amb l'objecte estudi de la cache de l'aplicació.
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class LocalDatabaseUtilDAL
{
public:

    LocalDatabaseUtilDAL();

    ///Compacta la BD
    void compact();

    ///retorna la revisió de la BD a la que està connectada, si no troba a quina revisió pertany retorna -1
    int getDatabaseRevision();

    ///Connexió de la base de dades a utilitzar
    void setDatabaseConnection(DatabaseConnection *dbConnection);

    ///Comprova si la base de dades està corrumpuda
    bool isDatabaseCorrupted();

    ///Retorna l'estat de la última operació realitzada
    int getLastError();

private :

    DatabaseConnection *m_dbConnection;

    int m_lastSqliteError;

    ///Ens retorna un string amb el select a executar per retorna la revisió de la base de dades sobre la qual estem connectats
    QString buildSqlGetDatabaseRevision();

    ///Ens fa un ErrorLog d'una sentència sql
    void logError(QString sqlSentence);

};
}

#endif
