/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include <sqlite3.h>

#include "dicommask.h"
#include "status.h"
#include "cacheseriesdal.h"
#include "dicomseries.h"
#include "serieslist.h"
#include "logging.h"
#include "databaseconnection.h"

namespace udg {

CacheSeriesDAL::CacheSeriesDAL()
{
}

CacheSeriesDAL::~CacheSeriesDAL()
{
}

Status CacheSeriesDAL::insertSeries( DICOMSeries *serie )
{
    int stateDatabase;
    Status state;
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();
    QString sqlSentence;

    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }

    sqlSentence = QString("Insert into Series "
                " ( SerInsUID , SerNum , StuInsUID , SerMod , ProNam , SerDes , SerPath , BodParExa , SerDat , SerTim ) "
                " values ( '%1' , '%2' , '%3' , '%4' , '%5' , '%6' , '%7' , '%8' , '%9' , '%10' ) " )
            .arg( serie->getSeriesUID() )
            .arg( serie->getSeriesNumber() )
            .arg( serie->getStudyUID() )
            .arg( serie->getSeriesModality() )
            .arg( serie->getProtocolName() )
            .arg( serie->getSeriesDescription() )
            .arg( serie->getSeriesPath() )
            .arg( serie->getBodyPartExaminated() )
            .arg( serie->getSeriesDate() )
            .arg( serie->getSeriesTime() );

    databaseConnection->getLock();
    stateDatabase = sqlite3_exec( databaseConnection->getConnection() , qPrintable(sqlSentence), 0 , 0 , 0 ) ;
    databaseConnection->releaseLock();

    state = databaseConnection->databaseStatus( stateDatabase );
    if ( !state.good() )
    {
        if ( state.code() != 2019 )
        {
            ERROR_LOG( QString("Error a la cache número %1").arg(state.code()) );
            ERROR_LOG( sqlSentence );
        }
        else
        {
            INFO_LOG( QString("La sèrie de la l'estudi %1 amb el SeriesUID %2 ja existeix a la base de dades").arg( serie->getStudyUID() ).arg( serie->getSeriesUID() ) );
        }
    }

    return state;
}

Status CacheSeriesDAL::querySeries( DicomMask seriesMask , SeriesList &ls )
{
    DcmDataset* mask = NULL;
    int columns , rows , i = 0 , stateDatabase;
    DICOMSeries series;
    char **reply = NULL , **error = NULL;
    QString logMessage;
    Status state;
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();

    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }

    mask = seriesMask.getDicomMask();

    databaseConnection->getLock();
    stateDatabase = sqlite3_get_table( databaseConnection->getConnection() , qPrintable(buildSqlQuerySeries( &seriesMask )), &reply , &rows, &columns , error ); //connexio a la bdd,sentencia sql ,reply, numero de files,numero de columnss.
    databaseConnection->releaseLock();

    state = databaseConnection->databaseStatus( stateDatabase );
    if ( !state.good() )
    {
        ERROR_LOG( QString("Error a la cache número %1").arg(state.code()) );
        ERROR_LOG( buildSqlQuerySeries( &seriesMask )  );
        return state;
    }

    i = 1;//ignorem les capçaleres
    while (i <= rows )
    {
        series.setSeriesUID( reply [ 0 + i * columns ] );
        series.setSeriesNumber( reply [ 1 + i * columns ] );
        series.setStudyUID( reply [ 2 + i * columns ] );
        series.setSeriesModality( reply [ 3 + i * columns ] );
        series.setSeriesDescription( reply [ 4 + i * columns ] );
        series.setProtocolName( reply [ 5 + i * columns ] );
        series.setSeriesPath( reply [ 6 + i * columns ] );
        series.setBodyPartExaminated( reply [ 7 + i * columns ] );
        series.setSeriesDate( reply[ 8 + i * columns ] );
        series.setSeriesTime( reply[ 9 + i * columns ] );

        ls.insert( series );
        i++;
    }
    return state;
}

Status CacheSeriesDAL::deleteSeries( QString studyUID )
{
    QString sql;
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();
    int stateDatabase;
    QString sqlSentence;
    Status state;

    databaseConnection->getLock();//nomes un proces a la vegada pot entrar a la cache

    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }

    sqlSentence = QString("delete from series where StuInsUID = '%1'").arg( studyUID );

    databaseConnection->getLock();
    stateDatabase = sqlite3_exec( databaseConnection->getConnection() , qPrintable( sqlSentence ), 0 , 0 , 0 ) ;
    databaseConnection->releaseLock();

    state = databaseConnection->databaseStatus( stateDatabase );

    if ( !state.good() )
    {
        ERROR_LOG( QString("Error a la cache número %1").arg(state.code()) );
        ERROR_LOG( sqlSentence );
    }
    return state;
}

QString CacheSeriesDAL::buildSqlQuerySeries( DicomMask *seriesMask )
{
    QString sql;

    sql = QString("select SerInsUID , SerNum , StuInsUID , SerMod , SerDes , ProNam, SerPath , BodParExa, SerDat , SerTim "
            " from series where StuInsUID = '%1' order by SerNum" ).arg( seriesMask->getStudyUID() );

    return sql;
}

}
