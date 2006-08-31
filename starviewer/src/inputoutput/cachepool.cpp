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

Status CachePool::constructState(int numState)
{
//A www.sqlite.org/c_interface.html hi ha al codificacio dels estats que retorna el sqlite
    Status state;
	QString logMessage , codeError;

    switch(numState)
    {//aqui tractem els errors que ens poden afectar de manera més directe, i els quals l'usuari pot intentar solucionbar                         
        case SQLITE_OK :        state.setStatus( "Normal" , true , 0 );
                                break;
        case SQLITE_ERROR :     state.setStatus( "Database missing" , false , 2001 );
                                break;
        case SQLITE_CORRUPT :   state.setStatus( "Database corrupted" , false , 2011 );
                                break;
        case SQLITE_CONSTRAINT: state.setStatus( "Constraint Violation" , false , 2019 );
                                break;
      //aquests errors en principi no es poden donar, pq l'aplicació no altera cap element de l'estructura, si es produeix algun
      //Error d'aquests en principi serà perquè la bdd està corrupte o problemes interns del SQLITE, fent Numerror-2000 de l'estat
      //a la pàgina de www.sqlite.org podrem saber de quin error es tracta.
        default :               state.setStatus( "SQLITE internal error" , false , 2000 + numState ); 
                                break;
    }

	if ( numState != SQLITE_OK )
	{
		logMessage = "Error a la cache número " + codeError.setNum( numState , 10 );
		ERROR_LOG( logMessage.toAscii().constData() );
	}

   return state;
}

void CachePool::removeStudy( std::string absPathStudy )
{
    DeleteDirectory deleteDirectory;

    deleteDirectory.deleteDirectory( absPathStudy.c_str() );
}


//AQUESTA FUNCIO NO S'UTILITZA, JA QUE SEMPRE QUE ACTUALITZEM L'ESPAI ES QUANT INSERIM O ESBORREM UN ESTUDI I AQUESTES ACCIONS
//S'HAN DE FER AMB UN TRANSACCIO SINO ENS PODRIEM TROBAR QUE INSERISSIM UNA IMATGE L'APLICACIO ES TANQUES I NO S'HAGUES ACTUALITAT L'ESPAI OCUPAT
//SI TOT S'ENGLOBA DINS UNA TRANSACCIO NO HI HAURA AQUEST PROBLEMA
 
Status CachePool::updatePoolSpace( int size )
{

    int i;
    Status state;
    std::string sql;
    char *sqlSentence;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState( 50 );
    }
    
    sql.insert( 0 , "Update Pool Set Space = Space + %i " );
    sql.append( "where Param = 'USED'" );
    
    sqlSentence = sqlite3_mprintf( sql.c_str() , size );
    
    m_DBConnect->getLock();
    i = sqlite3_exec( m_DBConnect->getConnection() , sqlSentence , 0 , 0 , 0 );
    m_DBConnect->releaseLock();
                                
    state = constructState( i );
    return state;
}

Status CachePool::updatePoolTotalSize( int space )
{
    int i;
    Status state;
    std::string sql;
    char size[25];
    unsigned long long spaceBytes;
    
    //sqlite no permet en un update entre valors mes gran que un int, a través de la interfície c++ com guardem la mida en bytes fem un string i hi afegim multiplicar l'espai per 1024*1024, per passar a bytes
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState( 50 );
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
                                
    state = constructState( i );

    return state;
}

Status CachePool::resetPoolSpace()
{
    int i;
    Status state;
    std::string sql;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState( 50 );
    }
    
    sql.insert( 0 , "Update Pool Set Space = 0 " );
    sql.append( "where Param = 'USED'" );
    
    m_DBConnect->getLock();
    i = sqlite3_exec( m_DBConnect->getConnection() , sql.c_str() , 0 , 0 , 0 );
    m_DBConnect->releaseLock();
                                
    state = constructState( i );

    return state;
}

Status CachePool::getPoolUsedSpace( unsigned int &space )
{
    Status state;
    std::string sql;
    char **resposta = NULL , **error = NULL;
    int col , rows , i;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState( 50 );
    }    
    
    sql.insert( 0 , "select round(Space/(1024*1024)) from Pool " ); //convertim de bytes a Mb
    sql.append( "where Param = 'USED'" );
    
    m_DBConnect->getLock(); 
    i = sqlite3_get_table( m_DBConnect->getConnection() , sql.c_str() , &resposta , &rows , &col , error );
    m_DBConnect->releaseLock();
                                
    state = constructState( i );

    if ( !state.good() ) return state;
    
    i = 1;//ignorem les capçaleres
   
    space = atoi( resposta[i] );
    
    return state;
}

Status CachePool::getPoolTotalSize( unsigned int &space )
{
    Status state;
    std::string sql;
    char **resposta = NULL , **error = NULL;
    int col , rows ,i;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState( 50 );
    }
    
    sql.insert( 0 , "select round(Space/(1024*1024)) from Pool " ); //convertim de bytes a Mb
    sql.append( "where Param = 'POOLSIZE'" );
    
    m_DBConnect->getLock();
    i = sqlite3_get_table( m_DBConnect->getConnection() , sql.c_str() , &resposta , &rows , &col , error );
    m_DBConnect->releaseLock();
                                
    state = constructState( i );

    if ( !state.good() ) return state;
    
    i = 1;//ignorem les capçaleres
   
    space = atoi( resposta[i] );
    
    return state;
}

Status CachePool::getPoolFreeSpace( unsigned int &freeSpace )
{
    Status state;
    unsigned int usedSpace , totalSpace;
    
    state = getPoolTotalSize( totalSpace );
    
    if ( !state.good() ) return state;
    
    state = getPoolUsedSpace( usedSpace );
    
    freeSpace = totalSpace - usedSpace;
    
    return state;
}

};
