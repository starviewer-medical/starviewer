/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include <string>

#include "pacslistdb.h"
#include "status.h"
#include "pacsparameters.h"
#include "logging.h"

namespace udg {

PacsListDB::PacsListDB()
{
    m_DBConnect = DatabaseConnection::getDatabaseConnection();
}


Status PacsListDB::insertPacs( PacsParameters *pacs )
{
    Status state , stateQuery;
    int i;
    char *sqlSentence, errorNumber[5];
    std::string logMessage , sql;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }
    //hem de comprovar que el pacs no existis ja abans! i ara estigui donat de baixa
    stateQuery = queryPacsDeleted(pacs);
    
    if ( stateQuery.code() == 2099 )
    {//El pacs no estava en està estat de baixa
        //El PACSid s'autoincrementa sol amb max(PACSID)+1
        sql.insert( 0 , "Insert into PacsList " );
        sql.append( "(AETitle,Server,Port,Inst,Loc,Desc,Def,PacsID,Del) " );
        sql.append( "Values (%Q,%Q,%Q,%Q,%Q,%Q,%Q,(select max(PacsID) from PacsList)+1,'N')" );
    
    
        sqlSentence = sqlite3_mprintf( sql.c_str(),
                                        pacs->getAEPacs().c_str() ,
                                        pacs->getPacsAdr().c_str() ,
                                        pacs->getPacsPort().c_str() ,
                                        pacs->getInstitution().c_str() ,
                                        pacs->getLocation().c_str() ,
                                        pacs->getDescription().c_str() ,
                                        pacs->getDefault().c_str() );
        m_DBConnect->getLock(); 
        i=sqlite3_exec( m_DBConnect->getConnection() , sqlSentence  , 0 , 0 , 0 ) ;
        m_DBConnect->releaseLock();
        
        state = m_DBConnect->databaseStatus( i );
    }
    else if ( stateQuery.code() == 0 )
    {//existeix un PAcs amb el mateix AEtitle, actualitzem les dades i el donem d'alta
    
        state = updatePacs( pacs );
    }
    else return stateQuery;

    if ( !state.good() )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
    }    

    return state;                    
}

Status PacsListDB::updatePacs( PacsParameters *pacs )
{
    Status state;
    int i;
    char *sqlSentence, errorNumber[5];
    std::string logMessage , sql;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }
    
    sql.insert( 0 , "Update PacsList " );
    sql.append( "set AETitle = %Q, " );
    sql.append( "Server = %Q, " );
    sql.append( "Port = %Q, " );
    sql.append( "Inst = %Q, " );
    sql.append( "Loc = %Q, " );
    sql.append( "Desc = %Q, " );
    sql.append( "Def = %Q, " );
    sql.append( "Del = %Q " );
    sql.append ( " where PacsID = %i" );
    
    sqlSentence = sqlite3_mprintf(sql.c_str() ,
                                    pacs->getAEPacs().c_str() ,
                                    pacs->getPacsAdr().c_str() ,
                                    pacs->getPacsPort().c_str() ,
                                    pacs->getInstitution().c_str() ,
                                    pacs->getLocation().c_str() ,
                                    pacs->getDescription().c_str() ,
                                    pacs->getDefault().c_str() ,
                                    "N" ,
                                    pacs->getPacsID() );
    
    m_DBConnect->getLock();
    i = sqlite3_exec( m_DBConnect->getConnection() , sqlSentence  , 0 , 0 , 0 );
    m_DBConnect->releaseLock();
    
    state = m_DBConnect->databaseStatus( i );
    if ( !state.good() )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        return state;
    }        
    
    return state;                    
}

Status PacsListDB::queryPacsList( PacsList &list )
{
    int col , rows , i = 0 , estat;
    PacsParameters pacs;
    char **resposta = NULL , **error = NULL, errorNumber[5];
    std::string logMessage , sql;
    Status state;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }
    
    sql.insert( 0 , "select AETitle, Server, Port, Inst, Loc, Desc, Def,PacsID " );
    sql.append( "from PacsList " );
    sql.append( "where del = 'N' " );
    sql.append( "order by AETitle" );
    
    m_DBConnect->getLock();
    estat = sqlite3_get_table( m_DBConnect->getConnection() , sql.c_str() , &resposta , &rows , &col , error ); //connexio a la bdd,sentencia sql ,resposta, numero de files,numero de cols.
    m_DBConnect->releaseLock();
    state = m_DBConnect->databaseStatus( estat );
    
    if ( !state.good() )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        return state;
    }    
    
    i = 1;//ignorem les capçaleres
    while (i <= rows)
    {   
        pacs.setAEPacs( resposta[ 0 + i*col ] );
        pacs.setPacsAdr( resposta[ 1 + i*col ] );
        pacs.setPacsPort( resposta[ 2 + i*col ] );
        pacs.setInstitution( resposta[ 3 + i*col ] );
        pacs.setLocation( resposta[ 4 + i*col ] );
        pacs.setDescription( resposta[ 5 + i*col ] );
        pacs.setDefault( resposta[ 6 + i*col ] );
        pacs.setPacsID(atoi( resposta[ 7 + i*col]));
        
        i++;
        list.insertPacs(pacs);
    }
    
    return state;
}

Status PacsListDB::queryPacs( PacsParameters *pacs , std::string AETitle )
{
    int col , rows = 0 , i = 0 , estat;
    char **resposta = NULL , **error = NULL, errorNumber[5];
    std::string logMessage , sql;
    Status state;

    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }

    sql.insert( 0 , "select AETitle, Server, Port, Inst, Loc, Desc, Def,PacsID " );
    sql.append( "from PacsList " );
    sql.append( " where AEtitle = '" );
    sql.append( AETitle.c_str() );
    sql.append( "'" );
   
    
    m_DBConnect->getLock();
    estat = sqlite3_get_table( m_DBConnect->getConnection() , sql.c_str() , &resposta , &rows , &col , error ); //connexio a la bdd,sentencia sql ,resposta, numero de files,numero de cols.
    m_DBConnect->releaseLock();
    
    //sqlite no té estat per indica que no s'ha trobat dades, li assigno jo aquest estat!!
    if ( rows == 0 && estat == 0 ) estat = 99;
    
    state = m_DBConnect->databaseStatus( estat );
    
    if ( !state.good() )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        return state;
    }    
    
    if ( rows > 0 )
    {
        i = 1;//ignorem les capçaleres
        pacs->setAEPacs( resposta[0 + i*col ] );
        pacs->setPacsAdr( resposta[1 + i*col ] );
        pacs->setPacsPort( resposta[2 + i*col ] );
        pacs->setInstitution( resposta[3 + i*col ] );
        pacs->setLocation( resposta[4 + i*col ] );
        pacs->setDescription( resposta[5 + i*col ] );
        pacs->setDefault( resposta[6 + i*col ] );
        pacs->setPacsID(atoi( resposta[7 + i*col]));    
    }
    
    return state;
}

Status PacsListDB::queryPacs( PacsParameters *pacs , int pacsID )
{
    int col , rows = 0 , i = 0 , estat;
    char **resposta = NULL , **error = NULL , id[6], errorNumber[5];
    std::string logMessage , sql;
    Status state;

    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }

    sql.insert( 0 , "select AETitle, Server, Port, Inst, Loc, Desc, Def,PacsID " );
    sql.append( "from PacsList " );
    sprintf( id , "%i" ,pacsID );
    sql.append( " where PacsID = " );
    sql.append( id );

    m_DBConnect->getLock();
    estat = sqlite3_get_table( m_DBConnect->getConnection() , sql.c_str() , &resposta , &rows , &col , error ); //connexio a la bdd,sentencia sql ,resposta, numero de files,numero de cols.
    m_DBConnect->releaseLock();
    
    //sqlite no té estat per indica que no s'ha trobat dades, li assigno jo aquest estat!!
    if ( rows == 0 && estat == 0 ) estat = 99;
    
    state = m_DBConnect->databaseStatus( estat );
    if ( !state.good() )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        return state;
    }    
    
    if ( rows > 0 )
    {
        i = 1;//ignorem les capçaleres
        pacs->setAEPacs( resposta[0 + i*col ] );
        pacs->setPacsAdr( resposta[1 + i*col ] );
        pacs->setPacsPort( resposta[2 + i*col ] );
        pacs->setInstitution( resposta[3 + i*col ] );
        pacs->setLocation( resposta[4 + i*col ] );
        pacs->setDescription( resposta[5 + i*col ] );
        pacs->setDefault( resposta[6 + i*col ] );
        pacs->setPacsID(atoi( resposta[7 + i*col]));    
    }
    
    return state;
}

Status PacsListDB::deletePacs( PacsParameters *pacs )
{
    Status state;
    int i;
    char *sqlSentence, errorNumber[5];
    std::string logMessage , sql;
   
    sql.insert( 0 , "update PacsList set Del = 'S'" );
    sql.append ( " where PacsID = %i" );
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }
        
    sqlSentence = sqlite3_mprintf( sql.c_str() , pacs->getPacsID() );
    
    m_DBConnect->getLock();
    i = sqlite3_exec( m_DBConnect->getConnection() , sqlSentence  , 0 , 0 , 0 );
          
    m_DBConnect->releaseLock();
        
    state = m_DBConnect->databaseStatus( i );
    if ( !state.good() )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
    }                 
    return state;

}

Status PacsListDB::queryPacsDeleted( PacsParameters *pacs )
{
    int col , rows = 0 , estat;
    char **resposta = NULL ,**error = NULL, errorNumber[5];
    std::string logMessage , sql;
    Status state;

    sql.insert( 0 , "select PacsID " );
    sql.append( "from PacsList " );
    sql.append( " where AEtitle = '" );
    sql.append(pacs->getAEPacs());
    sql.append( "' and Del = 'S'" );
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }
    
    m_DBConnect->getLock();
    estat=sqlite3_get_table( m_DBConnect->getConnection() , sql.c_str() , &resposta , &rows , &col , error ); //connexio a la bdd,sentencia sql ,resposta, numero de files,numero de cols.
    m_DBConnect->releaseLock();
    
    //sqlite no té estat per indica que no s'ha trobat dades, li assigno jo aquest estat!!
    if ( rows == 0 && estat == 0 ) estat = 99;
    
    if ( rows > 0 ) pacs->setPacsID( atoi( resposta[1]) );
    state = m_DBConnect->databaseStatus( estat );
    
    if ( !state.good() )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
    }   
    return state;
}

PacsListDB::~PacsListDB()
{
}

};
