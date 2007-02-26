/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include <string>
#include <sqlite3.h>
#include "image.h"
#include "status.h"
#include "databaseconnection.h"
#include "imagemask.h"
#include "logging.h"

#include "cacheimagedal.h"

namespace udg {

CacheImageDAL::CacheImageDAL()
{
}

Status CacheImageDAL::insertImage( Image *image )
{
    //no guardem el path de la imatge perque la el podem saber amb Study.AbsPath/SeriesUID/SopInsUID
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();
    int stateDatabase;
    Status state;
    char *sqlSentence , errorNumber[5];
    std::string logMessage , sql;

    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }

    sql.insert( 0 , "Insert into Image (SopInsUID, StuInsUID, SerInsUID, ImgNum, ImgTim,ImgDat, ImgSiz, ImgNam) " );
    sql.append( "values (%Q,%Q,%Q,%i,%Q,%Q,%i,%Q)" );

    databaseConnection->getLock();

    stateDatabase = sqlite3_exec( databaseConnection->getConnection() , "BEGIN TRANSACTION ", 0 , 0 , 0 );//comencem la transacció

    state = databaseConnection->databaseStatus( stateDatabase );

    if ( !state.good() )
    {
         stateDatabase = sqlite3_exec( databaseConnection->getConnection() , "ROLLBACK TRANSACTION " , 0 , 0 , 0 );
        databaseConnection->releaseLock();
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        return state;
    }

    sqlSentence = sqlite3_mprintf( sql.c_str()
                                ,image->getSoPUID().c_str()
                                ,image->getStudyUID().c_str()
                                ,image->getSeriesUID().c_str()
                                ,image->getImageNumber()
                                ,"0" //Image time
                                ,"0" //Image Date
                                ,image->getImageSize()
                                ,image->getImageName().c_str() );   //Image size

    stateDatabase = sqlite3_exec( databaseConnection->getConnection(), sqlSentence , 0, 0, 0);

    state = databaseConnection->databaseStatus( stateDatabase );
    if ( !state.good() )
    {
        stateDatabase = sqlite3_exec( databaseConnection->getConnection() , "ROLLBACK TRANSACTION " , 0 , 0 , 0 );
        databaseConnection->releaseLock();
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        ERROR_LOG( sqlSentence );
        return state;
    }

    //Actualitzem l'espai ocupat de la cache , per la nova imatge descarregada
    sql.clear();
    sql.insert( 0 , "Update Pool Set Space = Space + %i " );
    sql.append( "where Param = 'USED'" );

    sqlSentence = sqlite3_mprintf( sql.c_str() , image->getImageSize() );

    stateDatabase = sqlite3_exec( databaseConnection->getConnection(), sqlSentence , 0, 0, 0);

    state = databaseConnection->databaseStatus( stateDatabase );
    if ( !state.good() )
    {
        stateDatabase = sqlite3_exec( databaseConnection->getConnection() , "ROLLBACK TRANSACTION ", 0 , 0 , 0 );
        databaseConnection->releaseLock();
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        ERROR_LOG( sqlSentence );
        return state;
    }

    stateDatabase = sqlite3_exec( databaseConnection->getConnection() , "COMMIT TRANSACTION " , 0 , 0 , 0 );

    databaseConnection->releaseLock();

    state = databaseConnection->databaseStatus( stateDatabase );
    if ( !state.good() )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
    }

    return state;
}

Status CacheImageDAL::queryImages( ImageMask imageMask , ImageList &ls )
{
    int columns , rows , i = 0 , stateDatabase;
    Image image;
    char **resposta = NULL , **error = NULL , errorNumber[5];
    std::string logMessage , absPath;
    Status state;
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();

    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }

    databaseConnection->getLock();
    stateDatabase = sqlite3_get_table( databaseConnection->getConnection() ,
                                      buildSqlQueryImages( &imageMask ).c_str() ,
                                    &resposta , &rows , &columns , error );
                                    //connexio a la bdd,sentencia sql ,resposta, numero de files,numero de cols.
    databaseConnection->releaseLock();

    state = databaseConnection->databaseStatus( stateDatabase );
    if ( !state.good() )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        ERROR_LOG( buildSqlQueryImages( &imageMask ).c_str()  );
        return state;
    }

    i = 1;//ignorem les capçaleres
    while (i <= rows )
    {
        image.setImageNumber(atoi( resposta [ 0 + i * columns ] ) );

        //creem el path absolut
        absPath.erase();
        absPath.insert( 0 , resposta[1 + i * columns ] );
        absPath.append( resposta [ 3 + i * columns ] ); //incloem el directori de la serie
        absPath.append( "/" );
        absPath.append( resposta [ 5 + i * columns ] ); //incloem el nom de la imatge
        image.setImagePath( absPath.c_str() );

        image.setStudyUID( resposta [ 2 + i * columns ] );
        image.setSeriesUID( resposta [ 3 + i * columns ] );
        image.setSoPUID( resposta [ 4 + i * columns ] );
        image.setImageName( resposta [ 5 + i * columns ] );

        ls.insert( image );
        i++;
    }

    return state;
}

Status CacheImageDAL::countImageNumber( ImageMask imageMask , int &imageNumber )
{
    int columns , rows , i = 0 , stateDatabase;
    char **resposta = NULL , **error = NULL , errorNumber[5];
    Status state;
    std::string sql , logMessage;
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();

    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus ( 50 );
    }

    databaseConnection->getLock();
    stateDatabase = sqlite3_get_table( databaseConnection->getConnection() , buildSqlCountImageNumber( &imageMask ).c_str() , &resposta , &rows , &columns , error );
    databaseConnection->releaseLock();

    state = databaseConnection->databaseStatus ( stateDatabase );
    if ( !state.good() )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        ERROR_LOG( buildSqlCountImageNumber( &imageMask ).c_str() );
        return state;
    }
    i = 1;//ignorem les capçaleres

    imageNumber = atoi( resposta [i] );

   return state;
}

Status CacheImageDAL::imageSize (  ImageMask imageMask , unsigned long &size )
{
    int columns , rows , stateDatabase;
    char **resposta = NULL , **error = NULL , errorNumber[5];
    std::string logMessage , sql;
    Status state;
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();

    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus ( 50 );
    }

    databaseConnection->getLock();
    stateDatabase = sqlite3_get_table( databaseConnection->getConnection() , buildSqlSizeImage( &imageMask ).c_str() , &resposta , &rows , &columns , error );
    databaseConnection->releaseLock();

    state = databaseConnection->databaseStatus ( stateDatabase );

    if ( !state.good() )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        ERROR_LOG( buildSqlSizeImage( &imageMask ).c_str() );
        return state;
    }

    if ( resposta[1] != NULL )
    {
        size = atol( resposta [1] );
    }
    else size = 0;

   return state;
}
Status CacheImageDAL::deleteImages( std::string studyUID )
{
    std::string sql;
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();
    int stateDatabase;
    char *sqlSentence , errorNumber[5];
    std::string logMessage;
    Status state;

    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }

    sql.insert( 0 , "delete from image where StuInsUID = %Q" );

    sqlSentence = sqlite3_mprintf( sql.c_str() , studyUID.c_str() );

    databaseConnection->getLock();//nomes un proces a la vegada pot entrar a la cache

    stateDatabase = sqlite3_exec( databaseConnection->getConnection(), sqlSentence , 0, 0, 0);

    databaseConnection->releaseLock();

    state =  databaseConnection->databaseStatus( stateDatabase );
    if ( !state.good() )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        ERROR_LOG( sqlSentence );
    }

    return state;

}

std::string CacheImageDAL::buildSqlCountImageNumber( ImageMask *imageMask )
{
    std::string sql , whereClause = "";

    sql.insert( 0 , "select count(*) from image " );

    //si hi ha UID study
    if ( imageMask->getStudyUID().length() > 0 )
    {
        whereClause.insert( 0 , " where StuInsUID = '" );
        whereClause.append( imageMask->getStudyUID() );
        whereClause.append( "'" );
    }

    //si hi ha UID de la sèrie
    if ( imageMask->getSeriesUID().length() > 0 )
    {
        if ( whereClause.length() > 0 )
        {
            whereClause.append( " and SerInsUID = '" );
            whereClause.append( imageMask->getSeriesUID() );
            whereClause.append( "'" );
        }
        else
        {
            whereClause.insert( 0 , " where SerInsUID = '" );
            whereClause.append( imageMask->getSeriesUID() );
            whereClause.append( "'" );
        }
    }

    //si hi ha número d'imatge
    if ( imageMask->getImageNumber().length() > 0 )
    {
        if ( whereClause.length() > 0 )
        {
            whereClause.append( " and ImgNum = '" );
            whereClause.append( imageMask->getImageNumber() );
            whereClause.append( "'" );
        }
        else
        {
            whereClause.insert( 0 , " where ImgNum = '" );
            whereClause.append( imageMask->getImageNumber() );
            whereClause.append( "'" );
        }
    }

    if ( whereClause.length() > 0 )
    {
        sql.append( whereClause );
    }

    return sql;
}

std::string CacheImageDAL::buildSqlSizeImage( ImageMask *imageMask )
{
    std::string sql , whereClause ="";

    sql.insert( 0 , "select sum(ImgSiz) from image " );

    //si hi ha UID study
    if ( imageMask->getStudyUID().length() > 0 )
    {
        whereClause.insert( 0 , " where StuInsUID = '" );
        whereClause.append( imageMask->getStudyUID() );
        whereClause.append( "'" );
    }

    //si hi ha UID de la sèrie
    if ( imageMask->getSeriesUID().length() > 0 )
    {
        if ( whereClause.length() > 0 )
        {
            whereClause.append( " and SerInsUID = '" );
            whereClause.append( imageMask->getSeriesUID() );
            whereClause.append( "'" );
        }
        else
        {
            whereClause.insert( 0 , " where SerInsUID = '" );
            whereClause.append( imageMask->getSeriesUID() );
            whereClause.append( "'" );
        }
    }

    //si hi ha número d'imatge
    if ( imageMask->getImageNumber().length() > 0 )
    {
        if ( whereClause.length() > 0 )
        {
            whereClause.append( " and ImgNum = '" );
            whereClause.append( imageMask->getImageNumber() );
            whereClause.append( "'" );
        }
        else
        {
            whereClause.insert( 0 , " where ImgNum = '" );
            whereClause.append( imageMask->getImageNumber() );
            whereClause.append( "'" );
        }
    }

    if ( whereClause.length() > 0 )
    {
        sql.append( whereClause );
    }

    return sql;
}

std::string CacheImageDAL::buildSqlQueryImages( ImageMask *imageMask )
{
    std::string sql  ,imgNum;

    sql.insert( 0 , "select ImgNum , AbsPath , Image.StuInsUID , SerInsUID , SopInsUID , ImgNam from image , study where Image.StuInsUID = '" );
    sql.append( imageMask->getStudyUID() );
    sql.append( "' and SerInsUID = '" );
    sql.append( imageMask->getSeriesUID() );
    sql.append( "' and Study.StuInsUID = Image.StuInsUID " );

    imgNum = imageMask->getImageNumber();

    if ( imgNum.length() > 0 )
    {
        sql.append( " and ImgNum = " );
        sql.append( imgNum );
        sql.append( " " );
    }

    sql.append( " order by ImgNum" );

    return sql;
}

CacheImageDAL::~CacheImageDAL()
{
}

}
