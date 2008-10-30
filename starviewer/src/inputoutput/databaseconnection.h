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

/** Com tenim més d'una classe que han d'accedir a la mateixa Base de dades, i amb SQLITE només podem tenir una connexió per la BD creem una classe singleton que s'encarregarà de gestionar la connexió a la Base de Dades. De la mateixa manera només un thread alhora pot accedir a la BD, aquesta classe implementarà mètodes per evitar que dos threads es trobin a la vegada dins la BD
@author marc
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
    void setDatabasePath(QString);

    /** Retorna la connexió a la base de dades
     * @return connexio a la base de dades, si el punter és nul, és que hi hagut error alhora de connectar, o que el path no és correcte
     */
     sqlite3 * getConnection();

    /** Indica s'esta connectat a la base de dades
     * @return indica si s'esta connectat a la base de dades
     */
    bool connected();

    ///connecta amb la base de dades segons el path
    void open();

    /// tanca la connexió de la base de dades*/
    void close();

    /// Comença/finalitza/Fa rollback una transacció a la base de dades. Només pot haver una transacció a la vegada amb la mateixa connexió, per això aquests mètodes tenen implantat un semàfor, qeu control l'accés a les transaccions
    //TODO: S'hauria de repassar l'ubicació ja que no semblaria gaire correcte com a responsabilitat de la connexió. Quan es faci refactoring...
    void beginTransaction();
    void endTransaction();
    void rollbackTransaction();

private :

    sqlite3 *m_databaseConnection;
    /*Sqlite només permet una transacció a la vegada amb la mateixa connexió, en un futur tenen previst permetre-ho però ara mateix 
      no per tant per assegurar que no tenim dos transaccions a la vegada implantem aquests semàfor*/
    QSemaphore *m_transactionLock;

    QString m_databasePath;
};
};//end namespace

#endif
