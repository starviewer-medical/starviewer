/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "databaseconnection.h"
#include <stdlib.h>
#include <stdio.h>
#include <stream.h>
#include "starviewersettings.h"

namespace udg {

/** Constructor de la classe*/
DatabaseConnection::DatabaseConnection()
{
   StarviewerSettings settings;
   m_ConnectionOpened = false;
   
   m_databasePath = settings.getDatabasePath().ascii();
   m_databaseLock = (sem_t*)malloc(sizeof(sem_t));
   sem_init(m_databaseLock,0,1);//semafor que controlarà que nomes un thread a la vegada excedeixi a la cache
}

/** Establei el path de la base de dades, per defecte, si no s'estableix, el va a buscar a la classe StarviewerSettings
  */
void DatabaseConnection::setDatabasePath(std::string path)
{
    m_databasePath = path;
}

/*connecta amb la base de dades segons el path*/
void DatabaseConnection::connectDB()
{
  
  m_db = sqlite_open(m_databasePath.c_str(),0,NULL);
  m_ConnectionOpened = true;
}

/** Retorna la connexió a la base de dades
  *     @return connexio a la base de dades, si el punter és nul, és que hi hagut error alhora de connectar, o que el path no és correcte
  */
sqlite* DatabaseConnection::getConnection()
{
    if (!m_ConnectionOpened) connectDB();
    
    return m_db;
}

/** Demana el candeu per accedir a la base de dades!. S'ha de demanar el candau per poder accedir de manera correcte i segura a la base de dades
  * ja que si hi accedeixen dos objectes, amb la mateixa connexió al mateix temps, donarà error, per això des de la connexió ens hem d'assegurar
  * que només és utilitzada una vegada
  */
void DatabaseConnection::getLock()
{
    sem_wait(m_databaseLock);
}

/** Allibera al candau per a que altres processos puguin accedir a la base de dades
  */
void DatabaseConnection::releaseLock()
{
    sem_post(m_databaseLock);
}

/*tanca la connexió de la base de dades*/
void DatabaseConnection::closeDB()
{
  sqlite_close(m_db);
  m_ConnectionOpened = false;
}

/**destructor de la classe*/
DatabaseConnection::~DatabaseConnection()
{
    closeDB();
}

};
