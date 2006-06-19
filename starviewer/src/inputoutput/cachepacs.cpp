/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 

#include <QString>
#include <time.h>
#include <string>

#include "status.h"
#include "cachepacs.h"
#include "cachepool.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "studymask.h"
#include "seriesmask.h"
#include "imagemask.h"
#include "logging.h"

namespace udg {

CachePacs::CachePacs()
{
   m_DBConnect = DatabaseConnection::getDatabaseConnection();
}

Status CachePacs::constructState( int numState )
{
//A www.sqlite.org/c_interface.html hi ha al codificacio dels estats que retorna el sqlite
    Status state;
	QString logMessage, codeError;

    switch(numState)
    {//aqui tractem els errors que ens poden afectar de manera més directe, i els quals l'usuari pot intentar solucionbar                         
        case SQLITE_OK :        state.setStatus( "Normal" , true , 0 );
                                break;
        case SQLITE_ERROR :     state.setStatus( "Database is corrupted or SQL error syntax " , false , 2001 );
                                break;
        case SQLITE_BUSY :      state.setStatus( "Database is locked" , false , 2006 );
                                break;
        case SQLITE_CORRUPT :   state.setStatus( "Database corrupted" , false , 2011 );
                                break;
        case SQLITE_CONSTRAINT: state.setStatus( "The new register is duplicated" , false , 2019 );
                                break;
        case 50 :               state.setStatus( "Not connected to database" , false , 2050 );
                                break;
      //aquests errors en principi no es poden donar, pq l'aplicació no altera cap element de l'estructura, si es produeix algun
      //Error d'aquests en principi serà perquè la bdd està corrupte o problemes interns del SQLITE, fent Numerror-2000 de l'estat
      //a la pàgina de www.sqlite.org podrem saber de quin error es tracta.
        default :               state.setStatus( "SQLITE internal error" , false , 2000 + numState ); 
                                break;
    }

	if (  numState != SQLITE_OK )
	{
		logMessage = "Error a la cache número " + codeError.setNum( numState , 10 );
		ERROR_LOG( logMessage.toAscii().constData() );
	}

   return state;
}

Status CachePacs::insertImage( Image *image )
{
    //no guardem el path de la imatge perque la el podem saber amb Study.AbsPath/SeriesUID/SopInsUID
    int i;
    Status state;
    std::string sql;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState( 50 );
    }
    
    sql.insert( 0 , "Insert into Image (SopInsUID, StuInsUID, SerInsUID, ImgNum, ImgTim,ImgDat, ImgSiz, ImgNam) " );
    sql.append( "values (%Q,%Q,%Q,%i,%Q,%Q,%i,%Q)" );
    
    m_DBConnect->getLock();
    i = sqlite_exec_printf( m_DBConnect->getConnection() , "BEGIN TRANSACTION ", 0 , 0 , 0 );//comencem la transacció

    state = constructState( i );
    
    if ( !state.good() )
    {
        i = sqlite_exec_printf( m_DBConnect->getConnection() , "ROLLBACK TRANSACTION " , 0 , 0 , 0 );
        m_DBConnect->releaseLock();
        return state;
    }
    i = sqlite_exec_printf( m_DBConnect->getConnection(),sql.c_str() , 0 , 0 , 0 
                                ,image->getSoPUID().c_str()
                                ,image->getStudyUID().c_str()
                                ,image->getSeriesUID().c_str()
                                ,image->getImageNumber()
                                ,"0" //Image time
                                ,"0" //Image Date
                                ,image->getImageSize()
                                ,image->getImageName().c_str() );   //IMage size
                                
    state = constructState( i );
    if ( !state.good() )
    {
        i = sqlite_exec_printf( m_DBConnect->getConnection() , "ROLLBACK TRANSACTION " , 0 , 0 , 0 );
        m_DBConnect->releaseLock();
        return state;
    }
                                    
    sql.clear();  
    sql.insert( 0 , "Update Pool Set Space = Space + %i " );
    sql.append( "where Param = 'USED'" );
    
    i = sqlite_exec_printf( m_DBConnect->getConnection() , sql.c_str() , 0 , 0 , 0 
                                ,image->getImageSize() );
    
    state = constructState (i );
    if ( !state.good() )
    {
        i = sqlite_exec_printf( m_DBConnect->getConnection() , "ROLLBACK TRANSACTION ", 0 , 0 , 0 );
        m_DBConnect->releaseLock();
        return state;
    }
    
    i = sqlite_exec_printf( m_DBConnect->getConnection() , "COMMIT TRANSACTION " , 0 , 0 , 0 );
    
    m_DBConnect->releaseLock();
                                
    state = constructState( i );
    
    return state;
}

/****************************************************************************************************
                                                        ZONA DE LES QUERIES                               
 ****************************************************************************************************/


Status CachePacs::queryOldStudies( std::string OldStudiesDate , StudyList &ls )
{
    int col , rows , i = 0 , estat;
    Study stu;
    std::string sql;
    
    sql.insert( 0 , "select PatId, StuID, StuDat, StuTim, StuDes, StuInsUID, AbsPath, Modali " );
    sql.append( " from Study" );
    sql.append( " where AccDat < " );
    sql.append( OldStudiesDate );
    sql.append( " and Status = 'RETRIEVED' " );
    sql.append( " order by StuDat,StuTim " );

    char **resposta = NULL , **error = NULL;
    Status state;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState( 50 );
    }
    
    m_DBConnect->getLock();
    estat = sqlite_get_table( m_DBConnect->getConnection() , sql.c_str() , &resposta , &rows , &col , error ); //connexio a la bdd,sentencia sql,resposta, numero de files,numero de cols.
    m_DBConnect->releaseLock();
    state = constructState( estat );
    
    if ( !state.good() ) return state;
    
    i = 1;//ignorem les capçaleres
    while ( i <= rows )
    {   
        stu.setPatientId( resposta [ 0 + i * col ] );
        stu.setStudyId( resposta [ 1 + i * col ] );
        stu.setStudyDate( resposta [ 2 + i * col ] );
        stu.setStudyTime( resposta [ 3 + i * col ] );
        stu.setStudyDescription( resposta [ 4 + i * col ] );
        stu.setStudyUID( resposta [ 5 + i * col ] );
        stu.setAbsPath( resposta [ 6 + i * col ] );
        stu.setStudyModality( resposta [ 7 + i * col ] );
        ls.insert( stu );
        i++;
    }
    
    return state;
}


/********************************************************************************************************
                                                     ZONA DELETE                                           
 ********************************************************************************************************/

Status CachePacs::delStudy( std::string studyUID )
{
    Status state;
    int estat;
    char **resposta = NULL , **error = NULL;
    int col , rows , studySize , i;
    std::string sql , absPathStudy;
    CachePool cachePool;

    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState( 50 );
    }

    /* La part d'esborrar un estudi com que s'ha d'accedir a diverses taules, ho farem en un transaccio per si falla alguna sentencia sql fer un rollback, i així deixa la taula en estat estable, no deixem anar el candau fins al final */ 
    m_DBConnect->getLock();
    estat = sqlite_exec_printf( m_DBConnect->getConnection() , "BEGIN TRANSACTION " , 0 , 0 , 0 ); //comencem la transacció

    state = constructState( estat );
    if ( !state.good() )
    {
        estat = sqlite_exec_printf( m_DBConnect->getConnection() , "ROLLBACK TRANSACTION " , 0 , 0 , 0 );
        m_DBConnect->releaseLock();
        return state;
    }

    //sql per saber el directori on es guarda l'estudi
    sql.clear();
    sql.insert( 0 , "select AbsPath from study where StuInsUID = '" );
    sql.append( studyUID );
    sql.append( "'" );
      
    estat = sqlite_get_table(m_DBConnect->getConnection(),sql.c_str() , &resposta , &rows , &col , error ); //connexio a la bdd,sentencia sql,resposta, numero de files,numero de cols.
     
    state = constructState( estat );
    if ( !state.good() )
    {
        estat = sqlite_exec_printf( m_DBConnect->getConnection() , "ROLLBACK TRANSACTION " , 0 , 0 , 0 );
        m_DBConnect->releaseLock();
        return state;
    }       
    else if (  rows == 0 )
    {
        estat = sqlite_exec_printf( m_DBConnect->getConnection() , "ROLLBACK TRANSACTION " , 0 , 0 , 0 );
        m_DBConnect->releaseLock();
        return constructState( 99 );//error 99 registre no trobat           
    }
    else
    {
        absPathStudy = resposta[1];
    }
    
    //sql per saber quants estudis te el pacient
    sql.clear();
    sql.insert( 0 , "select count(*) from study where PatID in (select PatID from study where StuInsUID = '" );
    sql.append( studyUID );
    sql.append( "')" );

    estat = sqlite_get_table( m_DBConnect->getConnection() , sql.c_str() , &resposta , &rows , &col , error ); //connexio a la bdd,sentencia sql,resposta, numero de files,numero de cols.
     
    state = constructState( estat );
    if ( !state.good() )
    {
        estat = sqlite_exec_printf( m_DBConnect->getConnection() , "ROLLBACK TRANSACTION " , 0 , 0 , 0 );
        m_DBConnect->releaseLock();
        return state;
    }   
    else if (  rows == 0 )
    {    
        estat = sqlite_exec_printf( m_DBConnect->getConnection() , "ROLLBACK TRANSACTION " , 0 , 0 , 0 );
        m_DBConnect->releaseLock();
        return constructState ( 99 );//error 99 registre no trobat   
    }
    
    //ignorem el resposta [0], perque hi ha la capçalera
    if ( atoi( resposta [1]) == 1)
    {//si aquell pacient nomes te un estudi l'esborrem de la taula Patient
        estat = sqlite_exec_printf( m_DBConnect->getConnection(),"delete from Patient where PatID in (select PatID from study where StuInsUID = %Q)", 0 , 0 , 0 
                                , studyUID.c_str() );
                                
        state = constructState( estat );
        if ( !state.good() )
        {
            estat=sqlite_exec_printf( m_DBConnect->getConnection() , "ROLLBACK TRANSACTION " , 0 , 0 , 0 );
            m_DBConnect->releaseLock();
            return state;
        }    
    }
    
    //esborrem de la taula estudi    
    estat = sqlite_exec_printf( m_DBConnect->getConnection() , "delete from study where StuInsUID= %Q" , 0 , 0 , 0 ,studyUID.c_str() );
    
    state = constructState( estat );
    if ( !state.good() )
    {
        estat = sqlite_exec_printf( m_DBConnect->getConnection() , "ROLLBACK TRANSACTION " , 0 , 0 , 0 );
        m_DBConnect->releaseLock();
        return state;
    }

    //esborrem de la taula series
    estat = sqlite_exec_printf( m_DBConnect->getConnection() , "delete from series where StuInsUID= %Q" , 0 , 0 , 0 , studyUID.c_str() );
    state = constructState( estat );
    if ( !state.good() )
    {
        estat = sqlite_exec_printf( m_DBConnect->getConnection() , "ROLLBACK TRANSACTION " , 0 , 0 , 0 );
        m_DBConnect->releaseLock();
        return state;
    }
    
    //calculem el que ocupava l'estudi per actualitzar l'espai actualitzat
    sql.clear();
    sql.insert( 0 , "select sum(ImgSiz) from image where StuInsUID= '" );
    sql.append(studyUID);
    sql.append( "'" );
    estat = sqlite_get_table(m_DBConnect->getConnection(),sql.c_str() , &resposta , &rows , &col , error );
    
    state = constructState( estat );
    if ( !state.good() )
    {
        estat = sqlite_exec_printf( m_DBConnect->getConnection() , "ROLLBACK TRANSACTION " , 0 , 0 , 0 );
        m_DBConnect->releaseLock();
        return state;
    }
    i = 1;//ignorem les capçaleres
    studySize = atoi( resposta [i] );

    //esborrem de la taula image
    estat = sqlite_exec_printf( m_DBConnect->getConnection() , "delete from image where StuInsUID= %Q" , 0 , 0 , 0 , studyUID.c_str() );
    state = constructState( estat );
    if ( !state.good() )
    {
        estat = sqlite_exec_printf( m_DBConnect->getConnection() , "ROLLBACK TRANSACTION " , 0 , 0 , 0 );
        m_DBConnect->releaseLock();
        return state;
    }

    sql.clear();    
    sql.insert( 0 , "Update Pool Set Space = Space - %i " );
    sql.append( "where Param = 'USED'" );
    
    estat = sqlite_exec_printf( m_DBConnect->getConnection(),sql.c_str() , 0 , 0 , 0 
                                , studySize );
                                
    state = constructState( estat );
    if ( !state.good() )
    {
        estat = sqlite_exec_printf( m_DBConnect->getConnection() , "ROLLBACK TRANSACTION " , 0 , 0 , 0 );
        m_DBConnect->releaseLock();
        return state;
    }
        
    estat = sqlite_exec_printf( m_DBConnect->getConnection(),"COMMIT TRANSACTION", 0 , 0 , 0 ); //fem commit
    state = constructState( estat );
    if ( !state.good() )
    {
        return state;
    }
    
    m_DBConnect->releaseLock();        
    
    //una vegada hem esborrat les dades de la bd, podem esborrar les imatges, això s'ha de fer al final, perqué si hi ha un error i esborrem les
    //imatges al principi, no les podrem recuperar i la informació a la base de dades hi continuarà estant
    cachePool.removeStudy( absPathStudy );
    
    return state;
   
}

Status CachePacs::delNotRetrievedStudies()
{
    Status state;
    int estat;
    char **resposta = NULL , **error = NULL;
    int col,rows,i;
    std::string sql , studyUID;

    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState( 50 );
    }
    
    //cerquem els estudis pendents de finalitzar la descarrega
    sql.insert( 0 , "select StuInsUID from Study where Status in ('PENDING','RETRIEVING')" );
   
    m_DBConnect->getLock();
    estat = sqlite_get_table( m_DBConnect->getConnection() , sql.c_str() , &resposta , &rows , &col , error ); //connexio a la bdd,sentencia sql,resposta, numero de files,numero de cols.
    m_DBConnect->releaseLock();
    
    state = constructState( estat );
    if ( !state.good() ) return state;
   
    //ignorem el resposta [0], perque hi ha la capçalera
    i = 1;
    
    while ( i <= rows )
    {   
        studyUID.erase();
        studyUID.insert( 0 , resposta[i] );
        state = delStudy( studyUID );
        if ( !state.good() )
        {
            break;
        }
        i++;
    }
    
    return state;
}



/*********************************************************************************************************                                                MANTENIMENT DE LA CACHE                                     
 *********************************************************************************************************/

Status CachePacs::compactCachePacs()
{
    int i;
    Status state;
    std::string sql;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState( 50 );
    }
    
    sql.insert( 0 , "vacuum" );//amb l'acció vacuum es compacta la base de dades
    
    m_DBConnect->getLock();
    i = sqlite_exec_printf( m_DBConnect->getConnection(),sql.c_str() , 0 , 0 , 0 );
    m_DBConnect->releaseLock();
                                
    state = constructState( i );

    return state;
}

CachePacs::~CachePacs()
{
}

};
