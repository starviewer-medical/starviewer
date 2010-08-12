/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDATABASECONNECTION_H
#define UDGDATABASECONNECTION_H

#include <QString>

class QSemaphore;
struct sqlite3;

namespace udg {

class Status;

/** Classe que proporciona la connexió a la base dades. Utilitzant aquesta classe no cal preocupar-se d'obrir o tancar la connexió a la BD, ja que es fa
  * automàticament per aquest classe, quan s'invoca el mètode getConnection() si no hi ha cap connexió oberta l'obre i quan es destrueix l'objecte es tanca 
  *la connexió (és important recordar que cal sempre DESTRUIR l'objecte DatabaseConnection perquè sinó no es tancarà la connexió.
  */
class DatabaseConnection
{
public:

    /// Constructor de la classe
    DatabaseConnection();

    ///destructor de la classe
    ~DatabaseConnection();

     /** Establei el path de la base de dades, per defecte, si no s'estableix, el va a buscar a la classe StarviewerSettings
      * @param path de la base de dades
      */
    void setDatabasePath(const QString &databasePath);

    /** Retorna la connexió a la base de dades
     * @return connexio a la base de dades, si el punter és nul, és que hi hagut error alhora de connectar, o que el path no és correcte
     */
    sqlite3 * getConnection();

    ///Retorna l'últim missatge d'error produït a la base de dades
    QString getLastErrorMessage();

    ///Retorna l'últim codi d'error produït a la base de dades
    int getLastErrorCode();

    ///Comença/finalitza/Fa rollback una transacció a la base de dades. Només pot haver una transacció a la vegada amb
    ///la mateixa connexió, per això aquests mètodes tenen implantat un semàfor, qeu control l'accés a les transaccions. Si es fa una transacció
    ///i no s'arriba mai a invocar endTransaction() quan es tanqui la connexió amb la base de dades sqlite automàticament fa un rollback dels canvis.
    //TODO: S'hauria de repassar l'ubicació ja que no semblaria gaire correcte com a responsabilitat de la connexió. Quan es faci refactoring...
    void beginTransaction();
    void endTransaction();
    void rollbackTransaction();

    /// Formata l'string de forma que no contingui caràcters extranys que puguin fer
    /// que l'execució d'una comanda SQL sigui incorrecta
    static QString formatTextToValidSQLSyntax( QString string );

    ///Formata un qchar perquè no contingui caràcters estranys o Nulls que pugin fer que la sentència sql sigui incorrecte
    static QString formatTextToValidSQLSyntax( QChar qchar);

private:
    ///connecta amb la base de dades segons el path
    void open();

    /// tanca la connexió de la base de dades*/
    void close();

    /** Indica s'esta connectat a la base de dades
     * @return indica si s'esta connectat a la base de dades
     */
    bool isConnected();

private :

    sqlite3 *m_databaseConnection;
    /*Sqlite només permet una transacció a la vegada amb la mateixa connexió, en un futur tenen previst permetre-ho però ara mateix 
      no per tant per assegurar que no tenim dos transaccions a la vegada implantem aquests semàfor*/
    QSemaphore *m_transactionLock;

    QString m_databasePath;
};
};//end namespace

#endif
