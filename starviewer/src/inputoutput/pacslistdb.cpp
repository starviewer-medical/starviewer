/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

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
    QString sqlSentence;

    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }
    //hem de comprovar que el pacs no existis ja abans! i ara estigui donat de baixa
    stateQuery = queryPacsDeleted(pacs);

    if ( stateQuery.code() == 2099 )
    {//El pacs no estava en està estat de baixa
        //El PACSid s'autoincrementa sol amb max(PACSID)+1

        sqlSentence = QString( "Insert into PacsList "
                            "(AETitle,Server,Port,Inst,Loc,Desc,Def,PacsID,Del) "
                            "Values ('%1','%2','%3','%4','%5','%6','%7',(select max(PacsID) from PacsList)+1,'N')"
            )
            .arg( pacs->getAEPacs() )
            .arg( pacs->getPacsAdr() )
            .arg( pacs->getPacsPort() )
            .arg( pacs->getInstitution() )
            .arg( pacs->getLocation() )
            .arg( pacs->getDescription() )
            .arg( pacs->getDefault() );

        m_DBConnect->getLock();

        i = sqlite3_exec( m_DBConnect->getConnection() , qPrintable(sqlSentence), 0 , 0 , 0) ;

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
        ERROR_LOG( QString("Error a la cache número %1").arg( state.code() ) );
        if ( stateQuery.code() == 2099 )
            ERROR_LOG( sqlSentence );
    }

    return state;
}

Status PacsListDB::updatePacs( PacsParameters *pacs )
{
    Status state;
    int i;
    QString sqlSentence;

    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }

    sqlSentence = QString("Update PacsList set AETitle = '%1', Server = '%2', Port = '%3', Inst = '%4', Loc = '%5', Desc = '%6', Def = '%7', Del = 'N' where PacsID = %8" )
        .arg( pacs->getAEPacs() )
        .arg( pacs->getPacsAdr() )
        .arg( pacs->getPacsPort() )
        .arg( pacs->getInstitution() )
        .arg( pacs->getLocation() )
        .arg( pacs->getDescription() )
        .arg( pacs->getDefault() )
        .arg( pacs->getPacsID() );

    m_DBConnect->getLock();
    i = sqlite3_exec( m_DBConnect->getConnection() , qPrintable(sqlSentence), 0 , 0 , 0 );
    m_DBConnect->releaseLock();

    state = m_DBConnect->databaseStatus( i );
    if ( !state.good() )
    {
        ERROR_LOG( QString("Error a la cache número %1").arg( state.code() ) );
        ERROR_LOG( sqlSentence );
        return state;
    }

    return state;
}

Status PacsListDB::queryPacsList( PacsList &list )
{
    int col , rows , i = 0 , estat;
    PacsParameters pacs;
    char **resposta = NULL , **error = NULL;
    QString sqlSentence;
    Status state;

    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }

    sqlSentence = "select AETitle, Server, Port, Inst, Loc, Desc, Def,PacsID from PacsList where del = 'N' order by AETitle";

    m_DBConnect->getLock();
    estat = sqlite3_get_table( m_DBConnect->getConnection() , qPrintable(sqlSentence), &resposta , &rows , &col , error ); //connexio a la bdd,sentencia sql ,resposta, numero de files,numero de cols.
    m_DBConnect->releaseLock();
    state = m_DBConnect->databaseStatus( estat );

    if ( !state.good() )
    {
        ERROR_LOG( QString("Error a la cache número %1").arg( state.code() ) );
        ERROR_LOG( sqlSentence );
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

Status PacsListDB::queryPacs( PacsParameters *pacs , QString AETitle )
{
    QString sqlSentence = this->getQueryPACSByAETitleSQLSentence( AETitle );
    return this->queryPACSInformation( pacs, sqlSentence );
}

Status PacsListDB::queryPacs( PacsParameters *pacs , int pacsID )
{
    QString sqlSentence = this->getQueryPACSByIDSQLSentence( pacsID );
    return this->queryPACSInformation( pacs, sqlSentence );
}

QString PacsListDB::getQueryPACSByIDSQLSentence( int id )
{
    return QString("select AETitle, Server, Port, Inst, Loc, Desc, Def,PacsID from PacsList where PacsID = %1").arg( id );
}

QString PacsListDB::getQueryPACSByAETitleSQLSentence( QString AETitle )
{
    return QString("select AETitle, Server, Port, Inst, Loc, Desc, Def,PacsID from PacsList where AEtitle = '%1'").arg( AETitle );
}

Status PacsListDB::queryPACSInformation( PacsParameters *pacs, QString sqlSentence )
{
    int col , rows = 0 , i = 0 , estat;
    char **resposta = NULL , **error = NULL;
    Status state;

    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }

    m_DBConnect->getLock();
    estat = sqlite3_get_table( m_DBConnect->getConnection() , qPrintable(sqlSentence), &resposta , &rows , &col , error ); //connexio a la bdd,sentencia sql ,resposta, numero de files,numero de cols.
    m_DBConnect->releaseLock();

    //sqlite no té estat per indica que no s'ha trobat dades, li assigno jo aquest estat!!
    if ( rows == 0 && estat == 0 )
        estat = 99;

    state = m_DBConnect->databaseStatus( estat );
    if ( !state.good() )
    {
        ERROR_LOG( QString("Error a la cache número %1").arg( state.code() ) );
        ERROR_LOG( sqlSentence );
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
    QString sqlSentence;

    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }
    sqlSentence = QString("update PacsList set Del = 'S' where PacsID = %1" ).arg( pacs->getPacsID() );

    m_DBConnect->getLock();
    i = sqlite3_exec( m_DBConnect->getConnection() , qPrintable(sqlSentence), 0 , 0 , 0 );

    m_DBConnect->releaseLock();

    state = m_DBConnect->databaseStatus( i );
    if ( !state.good() )
    {
        ERROR_LOG( QString("Error a la cache número %1").arg( state.code() ) );
        ERROR_LOG( sqlSentence );
    }
    return state;

}

Status PacsListDB::queryPacsDeleted( PacsParameters *pacs )
{
    int col , rows = 0 , estat;
    char **resposta = NULL ,**error = NULL;
    QString sqlSentence;
    Status state;

    sqlSentence = QString("select PacsID from PacsList where AEtitle = '%1' and Del = 'S'").arg( pacs->getAEPacs() );

    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }

    m_DBConnect->getLock();
    estat=sqlite3_get_table( m_DBConnect->getConnection() , qPrintable(sqlSentence), &resposta , &rows , &col , error ); //connexio a la bdd,sentencia sql ,resposta, numero de files,numero de cols.
    m_DBConnect->releaseLock();

    //sqlite no té estat per indica que no s'ha trobat dades, li assigno jo aquest estat!!
    if ( rows == 0 && estat == 0 ) estat = 99;

    if ( rows > 0 ) pacs->setPacsID( atoi( resposta[1]) );
    state = m_DBConnect->databaseStatus( estat );

    if ( !state.good() )
    {
        ERROR_LOG( QString("Error a la cache número %1").arg( state.code() ) );
        ERROR_LOG( sqlSentence );
    }
    return state;
}

PacsListDB::~PacsListDB()
{
}

};
