/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include <QDir>

#include "cachepool.h"
#include "status.h"
#include "logging.h"
#include "deletedirectory.h"

namespace udg {

CachePool::CachePool()
{
   m_DBConnect = DatabaseConnection::getDatabaseConnection();
}

CachePool::~CachePool()
{
}

void CachePool::removeStudy( QString absPathStudy )
{
    DeleteDirectory deleteDirectory;

    deleteDirectory.deleteDirectory( absPathStudy , true );
}

//AQUESTA FUNCIO NO S'UTILITZA, JA QUE SEMPRE QUE ACTUALITZEM L'ESPAI ES QUANT INSERIM O ESBORREM UN ESTUDI I AQUESTES ACCIONS
//S'HAN DE FER AMB UN TRANSACCIO SINO ENS PODRIEM TROBAR QUE INSERISSIM UNA IMATGE L'APLICACIO ES TANQUES I NO S'HAGUES ACTUALITAT L'ESPAI OCUPAT
//SI TOT S'ENGLOBA DINS UNA TRANSACCIO NO HI HAURA AQUEST PROBLEMA

Status CachePool::updatePoolSpace( int size )
{
    int i;
    Status state;
    QString sql;

    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }

    sql = QString("Update Pool Set Space = Space + %1 where Param = 'USED'").arg( size );

    m_DBConnect->getLock();
    i = sqlite3_exec( m_DBConnect->getConnection() , qPrintable(sql), 0 , 0 , 0 );
    m_DBConnect->releaseLock();

    state = m_DBConnect->databaseStatus( i );
    if ( !state.good() )
    {
        ERROR_LOG( qPrintable( QString("Error a la cache número %1").arg( state.code() ) ) );
        ERROR_LOG( qPrintable( sql ) );
    }

    return state;
}

Status CachePool::updatePoolTotalSize( int space )
{
    int i;
    Status state;
    char size[25];
    QString sql;
    unsigned long long spaceBytes;

    //sqlite no permet en un update entre valors mes gran que un int, a través de la interfície c++ com guardem la mida en bytes fem un string i hi afegim multiplicar l'espai per 1024*1024, per passar a bytes

    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }

    spaceBytes = space;
    spaceBytes = spaceBytes * 1024 * 1024; //convertim els Mb en bytes, ja que es guarden en bytes les unitats a la base de dades

    sprintf( size , "%Li" , spaceBytes ); //convertim l'espai en bytes a string %Li significa long integer
    sql = QString("Update Pool Set Space = %1 where Param = 'POOLSIZE'").arg( size );

    m_DBConnect->getLock();
    i = sqlite3_exec( m_DBConnect->getConnection() , qPrintable( sql ), 0 , 0 , 0 );
    m_DBConnect->releaseLock();

    state = m_DBConnect->databaseStatus( i );
    if ( !state.good() )
    {
        ERROR_LOG( qPrintable( QString("Error a la cache número %1").arg( state.code() ) ) );
        ERROR_LOG( qPrintable( sql ) );
    }

    return state;
}

Status CachePool::resetPoolSpace()
{
    int i;
    Status state;
    QString sql;

    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }

    sql = QString("Update Pool Set Space = 0 where Param = 'USED'" );

    m_DBConnect->getLock();
    i = sqlite3_exec( m_DBConnect->getConnection() , qPrintable( sql ), 0 , 0 , 0 );
    m_DBConnect->releaseLock();

    state = m_DBConnect->databaseStatus( i );
    if ( !state.good() )
    {
        ERROR_LOG( qPrintable( QString("Error a la cache número %1").arg( state.code() ) ) );
        ERROR_LOG( qPrintable( sql ) );
    }

    return state;
}

Status CachePool::getPoolUsedSpace( unsigned int &space )
{
    Status state;
    char **resposta = NULL , **error = NULL;
    QString sql;
    int col , rows , i;

    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }

    //convertim de bytes a Mb
    sql = QString("select round(Space/(1024*1024)) from Pool where Param = 'USED'" );

    m_DBConnect->getLock();
    i = sqlite3_get_table( m_DBConnect->getConnection() , qPrintable( sql ), &resposta , &rows , &col , error );
    m_DBConnect->releaseLock();

    state = m_DBConnect->databaseStatus( i );
    if ( !state.good() )
    {
        ERROR_LOG( qPrintable( QString("Error a la cache número %1").arg( state.code() ) ) );
        ERROR_LOG( qPrintable( sql ) );
        return state;
    }
    i = 1;//ignorem les capçaleres

    space = atoi( resposta[i] );

    return state;
}

Status CachePool::getPoolTotalSize( unsigned int &space )
{
    Status state;
    char **resposta = NULL , **error = NULL;
    QString sql;
    int col , rows ,i;

    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }

    sql = QString("select round(Space/(1024*1024)) from Pool where Param = 'POOLSIZE'" );

    m_DBConnect->getLock();
    i = sqlite3_get_table( m_DBConnect->getConnection() , qPrintable( sql ), &resposta , &rows , &col , error );
    m_DBConnect->releaseLock();

    state = m_DBConnect->databaseStatus( i );
    if ( !state.good() )
    {
        ERROR_LOG( qPrintable( QString("Error a la cache número %1").arg( state.code() ) ) );
        ERROR_LOG( qPrintable( sql ) );
        return state;
    }
    i = 1;//ignorem les capçaleres

    space = atoi( resposta[i] );

    return state;
}

Status CachePool::getPoolFreeSpace( unsigned int &freeSpace )
{
    Status state;
    unsigned int usedSpace , totalSpace;

    state = getPoolTotalSize( totalSpace );

    if ( !state.good() )
    {
        ERROR_LOG( qPrintable( QString("Error a la cache número %1").arg( state.code() ) ) );
        return state;
    }

    state = getPoolUsedSpace( usedSpace );

    if ( !state.good() )
    {
        ERROR_LOG( qPrintable( QString("Error a la cache número %1").arg( state.code() ) ) );
        return state;
    }

    freeSpace = totalSpace - usedSpace;

    return state;
}

};
