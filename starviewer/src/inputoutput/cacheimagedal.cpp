/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "cacheimagedal.h"

#include <QString>
#include <sqlite3.h>

#include "dicomimage.h"
#include "status.h"
#include "databaseconnection.h"
#include "dicommask.h"
#include "logging.h"

namespace udg {

CacheImageDAL::CacheImageDAL()
{
}

Status CacheImageDAL::insertImage( DICOMImage *image )
{
    //no guardem el path de la imatge perque la el podem saber amb Study.AbsPath/SeriesUID/SopInsUID
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();
    int stateDatabase;
    Status state;
    QString sqlSentence;

    if( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }

    databaseConnection->getLock();

    sqlSentence = QString("Insert into Image (SopInsUID, StuInsUID, SerInsUID, ImgNum, ImgTim,ImgDat, ImgSiz, ImgNam) values ('%1','%2','%3',%4,'%5','%6',%7,'%8')" )
        .arg( image->getSOPInstanceUID() )
        .arg( image->getStudyUID() )
        .arg( image->getSeriesUID() )
        .arg( image->getImageNumber() )
        .arg( '0' ) //Image time
        .arg( '0' ) //Image Date
        .arg( image->getImageSize() )
        .arg( image->getImageName() );

    stateDatabase = sqlite3_exec( databaseConnection->getConnection(), qPrintable( sqlSentence ), 0, 0, 0);

    state = databaseConnection->databaseStatus( stateDatabase );
    if ( !state.good() )
    {
        stateDatabase = sqlite3_exec( databaseConnection->getConnection() , "ROLLBACK TRANSACTION " , 0 , 0 , 0 );
        databaseConnection->releaseLock();

        if ( state.code() != 2019 )
        {
            ERROR_LOG( QString("Error a la cache número %1").arg( state.code() ) );
            ERROR_LOG( sqlSentence );
        }
        else
        {
            INFO_LOG( QString("La image de la l'estudi %1 amb el SeriesUID %2 amb el SopInstanceUID %3 ja existeix a la base de dades")
                .arg( image->getStudyUID() )
                .arg( image->getSeriesUID() )
                .arg( image->getSOPInstanceUID() )
                );
        }
        return state;
    }

    //Actualitzem l'espai ocupat de la cache , per la nova imatge descarregada
    sqlSentence = QString("Update Pool Set Space = Space + %1 where Param = 'USED'" ).arg(image->getImageSize() );

    stateDatabase = sqlite3_exec( databaseConnection->getConnection(), qPrintable( sqlSentence ), 0, 0, 0);

    state = databaseConnection->databaseStatus( stateDatabase );
    if ( !state.good() )
    {
        stateDatabase = sqlite3_exec( databaseConnection->getConnection() , "ROLLBACK TRANSACTION ", 0 , 0 , 0 );
        databaseConnection->releaseLock();
        ERROR_LOG( QString("Error a la cache número %1").arg( state.code() ) );
        ERROR_LOG( sqlSentence );
        return state;
    }

    databaseConnection->releaseLock();

    return state;
}

Status CacheImageDAL::queryImages( DicomMask imageMask , QList<DICOMImage> &outResultsImageList )
{
    int columns , rows , i = 0 , stateDatabase;
    DICOMImage image;
    char **reply = NULL , **error = NULL;
    QString absPath;
    Status state;
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();

    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }

    databaseConnection->getLock();
    stateDatabase = sqlite3_get_table( databaseConnection->getConnection() ,
                                      qPrintable( buildSqlQueryImages( &imageMask ) ) ,
                                    &reply , &rows , &columns , error );
                                    //connexio a la bdd,sentencia sql ,reply, numero de files,numero de cols.
    databaseConnection->releaseLock();

    state = databaseConnection->databaseStatus( stateDatabase );
    if ( !state.good() )
    {
        ERROR_LOG( QString("Error a la cache número %1").arg( state.code() ) );
        ERROR_LOG( buildSqlQueryImages( &imageMask ) );
        return state;
    }

    i = 1;//ignorem les capçaleres
    while (i <= rows )
    {
        image.setImageNumber(atoi( reply [ 0 + i * columns ] ) );

        //creem el path absolut
        absPath = QString("%1%2/%3")
            .arg( reply[1 + i * columns ] )
            .arg( reply [ 3 + i * columns ]  ) //incloem el directori de la serie
            .arg( reply [ 5 + i * columns ] ); //incloem el nom de la imatge

        image.setImagePath( absPath );

        image.setStudyUID( reply [ 2 + i * columns ] );
        image.setSeriesUID( reply [ 3 + i * columns ] );
        image.setSOPInstanceUID( reply [ 4 + i * columns ] );
        image.setImageName( reply [ 5 + i * columns ] );

        outResultsImageList.append( image );
        i++;
    }

    return state;
}

Status CacheImageDAL::countImageNumber( DicomMask imageMask , int &imageNumber )
{
    int columns , rows , i = 0 , stateDatabase;
    char **reply = NULL , **error = NULL;
    Status state;
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();

    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus ( 50 );
    }

    databaseConnection->getLock();
    stateDatabase = sqlite3_get_table( databaseConnection->getConnection() , qPrintable( buildSqlCountImageNumber( &imageMask ) ) , &reply , &rows , &columns , error );
    databaseConnection->releaseLock();

    state = databaseConnection->databaseStatus ( stateDatabase );
    if ( !state.good() )
    {
        ERROR_LOG( QString("Error a la cache número %1").arg( state.code() ) );
        ERROR_LOG( buildSqlCountImageNumber( &imageMask ) );
        return state;
    }
    i = 1;//ignorem les capçaleres

    imageNumber = atoi( reply [i] );

   return state;
}

Status CacheImageDAL::imageSize (  DicomMask imageMask , quint64 &size )
{
    int columns , rows , stateDatabase;
    char **reply = NULL , **error = NULL;
    Status state;
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();

    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus ( 50 );
    }

    databaseConnection->getLock();
    stateDatabase = sqlite3_get_table( databaseConnection->getConnection() , qPrintable( buildSqlSizeImage( &imageMask ) ) , &reply , &rows , &columns , error );
    databaseConnection->releaseLock();

    state = databaseConnection->databaseStatus ( stateDatabase );

    if ( !state.good() )
    {
        ERROR_LOG( QString("Error a la cache número %1").arg( state.code() ) );
        ERROR_LOG( buildSqlSizeImage( &imageMask ) );
        return state;
    }

    if ( reply[1] != NULL )
    {
        size = ( QString( reply [1] ) ).toULongLong();
    }
    else size = 0;

   return state;
}

Status CacheImageDAL::existImage( DicomMask imageMask, bool & exist )
{
    int columns , rows , stateDatabase;
    char **reply = NULL , **error = NULL;
    QString sql;
    Status state;
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();

    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus ( 50 );
    }

    databaseConnection->getLock();
    stateDatabase = sqlite3_get_table( databaseConnection->getConnection() , qPrintable( buildSqlExistImage( &imageMask ) ) , &reply , &rows , &columns , error );
    databaseConnection->releaseLock();

    state = databaseConnection->databaseStatus ( stateDatabase );

    if ( !state.good() )
    {
        ERROR_LOG( QString("Error a la cache número %1").arg( state.code() ) );
        ERROR_LOG( buildSqlSizeImage( &imageMask ) );
        return state;
    }

    if ( reply[1] != NULL )
    {
        exist = true;
    }
    else exist = false;

   return state;
}

Status CacheImageDAL::deleteImages( QString studyUID )
{
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();
    int stateDatabase;
    QString sqlSentence;
    Status state;

    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }

    sqlSentence = QString("delete from image where StuInsUID = '%1'").arg( studyUID );

    databaseConnection->getLock();//nomes un proces a la vegada pot entrar a la cache

    stateDatabase = sqlite3_exec( databaseConnection->getConnection(), qPrintable( sqlSentence ), 0, 0, 0);

    databaseConnection->releaseLock();

    state =  databaseConnection->databaseStatus( stateDatabase );
    if ( !state.good() )
    {
        ERROR_LOG( QString("Error a la cache número %1").arg( state.code() ) );
        ERROR_LOG( sqlSentence );
    }

    return state;

}

QString CacheImageDAL::buildSqlCountImageNumber( DicomMask *imageMask )
{
    QString sql , whereClause;

    sql = "select count(*) from image ";

    //si hi ha UID study
    if ( !imageMask->getStudyUID().isEmpty() )
    {
        whereClause = QString(" where StuInsUID = '%1'" ).arg( imageMask->getStudyUID() );
    }

    //si hi ha UID de la sèrie
    if ( !imageMask->getSeriesUID().isEmpty() )
    {
        if ( !whereClause.isEmpty() )
        {
            whereClause += QString(" and SerInsUID = '%1'" ).arg( imageMask->getSeriesUID() );
        }
        else
        {
            whereClause = QString( " where SerInsUID = '%1'" ).arg( imageMask->getSeriesUID() );
        }
    }

    //si hi ha número d'imatge
    if ( !imageMask->getImageNumber().isEmpty() )
    {
        if ( !whereClause.isEmpty() )
        {
            whereClause += QString( " and ImgNum = '%1'" ).arg( imageMask->getImageNumber() );
        }
        else
        {
            whereClause = QString(" where ImgNum = '%1'" ).arg( imageMask->getImageNumber() );
        }
    }

    if ( !whereClause.isEmpty() )
    {
        sql += whereClause;
    }

    return sql;
}

QString CacheImageDAL::buildSqlSizeImage( DicomMask *imageMask )
{
    QString sql , whereClause;

    sql = "select sum(ImgSiz) from image ";

    //si hi ha UID study
    if ( !imageMask->getStudyUID().isEmpty() )
    {
        whereClause = QString(" where StuInsUID = '%1'" ).arg( imageMask->getStudyUID() );
    }

    //si hi ha UID de la sèrie
    if ( !imageMask->getSeriesUID().isEmpty() )
    {
        if ( !whereClause.isEmpty() )
        {
            whereClause += QString( " and SerInsUID = '%1'" ).arg( imageMask->getSeriesUID() );
        }
        else
        {
            whereClause = QString( " where SerInsUID = '%1'" ).arg( imageMask->getSeriesUID() );
        }
    }

    //si hi ha número d'imatge
    if ( !imageMask->getImageNumber().isEmpty() )
    {
        if ( !whereClause.isEmpty() )
        {
            whereClause += QString( " and ImgNum = '%1'" ).arg( imageMask->getImageNumber() );
        }
        else
        {
            whereClause = QString( " where ImgNum = '%1'" ).arg( imageMask->getImageNumber() );
        }
    }

    if ( !whereClause.isEmpty() )
    {
        sql += whereClause;
    }

    return sql;
}

QString CacheImageDAL::buildSqlExistImage( DicomMask *imageMask )
{
    QString sql , whereClause;

    sql = "select sum(ImgSiz) from image ";

    //si hi ha UID study
    if ( !imageMask->getStudyUID().isEmpty() )
    {
        whereClause = QString(" where StuInsUID = '%1'" ).arg( imageMask->getStudyUID() );
    }

    //si hi ha UID de la sèrie
    if ( !imageMask->getSeriesUID().isEmpty() )
    {
        if ( !whereClause.isEmpty() )
        {
            whereClause += QString( " and SerInsUID = '%1'" ).arg( imageMask->getSeriesUID() );
        }
        else
        {
            whereClause = QString( " where SerInsUID = '%1'" ).arg( imageMask->getSeriesUID() );
        }
    }

    //si hi ha SopInsUID
    if ( !imageMask->getSOPInstanceUID().isEmpty() )
    {
        if ( !whereClause.isEmpty() )
        {
            whereClause += QString( " and SopInsUID = '%1'" ).arg( imageMask->getSOPInstanceUID() );
        }
        else
        {
            whereClause = QString( " where SopInsUID = '%1'" ).arg( imageMask->getSOPInstanceUID() );
        }
    }

    if ( !whereClause.isEmpty() )
    {
        sql += whereClause;
    }

    return sql;
}

QString CacheImageDAL::buildSqlQueryImages( DicomMask *imageMask )
{
    QString sql  ,imgNum;

    sql = QString( "select ImgNum , AbsPath , Image.StuInsUID , SerInsUID , SopInsUID , ImgNam from image , study where Image.StuInsUID = '%1'" ).arg( imageMask->getStudyUID() );

    if ( !imageMask->getSeriesUID().isEmpty() )
    {
        sql += QString( " and SerInsUID = '%1'" ).arg( imageMask->getSeriesUID() );
    }

    if ( !imageMask->getSOPInstanceUID().isEmpty() )
    {
        sql += QString( " and SopInsUID = '%1'" ).arg( imageMask->getSOPInstanceUID() );
    }

    sql += " and Study.StuInsUID = Image.StuInsUID ";

    imgNum = imageMask->getImageNumber();

    if ( !imgNum.isEmpty() )
    {
        sql += QString( " and ImgNum = %1 " ).arg( imgNum );
    }

    sql += " order by ImgNum";

    return sql;
}

CacheImageDAL::~CacheImageDAL()
{
}

}
