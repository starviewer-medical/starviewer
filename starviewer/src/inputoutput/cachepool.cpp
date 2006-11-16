/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include <QDir>
#include <string>

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

void CachePool::removeStudy( std::string absPathStudy )
{
    DeleteDirectory deleteDirectory;

    deleteDirectory.deleteDirectory( absPathStudy.c_str() , true );
}

//AQUESTA FUNCIO NO S'UTILITZA, JA QUE SEMPRE QUE ACTUALITZEM L'ESPAI ES QUANT INSERIM O ESBORREM UN ESTUDI I AQUESTES ACCIONS
//S'HAN DE FER AMB UN TRANSACCIO SINO ENS PODRIEM TROBAR QUE INSERISSIM UNA IMATGE L'APLICACIO ES TANQUES I NO S'HAGUES ACTUALITAT L'ESPAI OCUPAT
//SI TOT S'ENGLOBA DINS UNA TRANSACCIO NO HI HAURA AQUEST PROBLEMA
 
Status CachePool::updatePoolSpace( int size )
{
    int i;
    Status state;
    char *sqlSentence , errorNumber[5];
    std::string logMessage , sql;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }
    
    sql.insert( 0 , "Update Pool Set Space = Space + %i " );
    sql.append( "where Param = 'USED'" );
    
    sqlSentence = sqlite3_mprintf( sql.c_str() , size );
    
    m_DBConnect->getLock();
    i = sqlite3_exec( m_DBConnect->getConnection() , sqlSentence , 0 , 0 , 0 );
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

Status CachePool::updatePoolTotalSize( int space )
{
    int i;
    Status state;
    char size[25] , errorNumber[5];
    std::string logMessage , sql;
    unsigned long long spaceBytes;
    
    //sqlite no permet en un update entre valors mes gran que un int, a través de la interfície c++ com guardem la mida en bytes fem un string i hi afegim multiplicar l'espai per 1024*1024, per passar a bytes
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }
    
    spaceBytes = space;
    spaceBytes = spaceBytes * 1024 * 1024; //convertim els Mb en bytes, ja que es guarden en bytes les unitats a la base de dades
    
    sprintf( size , "%Li" , spaceBytes ); //convertim l'espai en bytes a string %Li significa long integer
    sql.insert( 0 , "Update Pool Set Space = " );
    sql.append( size );
    sql.append( " where Param = 'POOLSIZE'" );
    
    m_DBConnect->getLock();
    i = sqlite3_exec( m_DBConnect->getConnection() , sql.c_str() , 0 , 0 , 0 );
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

Status CachePool::resetPoolSpace()
{
    int i;
    Status state;
    char errorNumber[5];
    std::string logMessage , sql;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }
    
    sql.insert( 0 , "Update Pool Set Space = 0 " );
    sql.append( "where Param = 'USED'" );
    
    m_DBConnect->getLock();
    i = sqlite3_exec( m_DBConnect->getConnection() , sql.c_str() , 0 , 0 , 0 );
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

Status CachePool::getPoolUsedSpace( unsigned int &space )
{
    Status state;
    char **resposta = NULL , **error = NULL , errorNumber[5];
    std::string logMessage , sql;
    int col , rows , i;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }    
    
    sql.insert( 0 , "select round(Space/(1024*1024)) from Pool " ); //convertim de bytes a Mb
    sql.append( "where Param = 'USED'" );
    
    m_DBConnect->getLock(); 
    i = sqlite3_get_table( m_DBConnect->getConnection() , sql.c_str() , &resposta , &rows , &col , error );
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
    i = 1;//ignorem les capçaleres
   
    space = atoi( resposta[i] );
    
    return state;
}

Status CachePool::getPoolTotalSize( unsigned int &space )
{
    Status state;
    char **resposta = NULL , **error = NULL , errorNumber[5];
    std::string logMessage , sql;
    int col , rows ,i;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return m_DBConnect->databaseStatus( 50 );
    }
    
    sql.insert( 0 , "select round(Space/(1024*1024)) from Pool " ); //convertim de bytes a Mb
    sql.append( "where Param = 'POOLSIZE'" );
    
    m_DBConnect->getLock();
    i = sqlite3_get_table( m_DBConnect->getConnection() , sql.c_str() , &resposta , &rows , &col , error );
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
    i = 1;//ignorem les capçaleres
   
    space = atoi( resposta[i] );
    
    return state;
}

Status CachePool::getPoolFreeSpace( unsigned int &freeSpace )
{
    Status state;
    unsigned int usedSpace , totalSpace;
    char errorNumber[5];
    std::string logMessage;
    
    state = getPoolTotalSize( totalSpace );
    
    if ( !state.good() )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        return state;
    }    
    
    state = getPoolUsedSpace( usedSpace );
    
    if ( !state.good() )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        return state;
    }
    
    freeSpace = totalSpace - usedSpace;
    
    return state;
}

};
