/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include <string>
#include <sqlite3.h>

#include "seriesmask.h"
#include "status.h"
#include "cacheseriesdal.h"
#include "series.h"
#include "serieslist.h"

#include "databaseconnection.h"

namespace udg {

CacheSeriesDAL::CacheSeriesDAL()
{
}

Status CacheSeriesDAL::insertSeries( Series *serie )
{
    int stateDatabase;
    Status state;
    std::string sql;
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();
    char *sqlSentence;
    
    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }
    
    sql.insert( 0 , "Insert into Series " );
    sql.append( " ( SerInsUID , SerNum , StuInsUID , SerMod , ProNam , SerDes , SerPath , BodParExa , SerDat , SerTim ) " );
    sql.append( " values ( %Q , %Q , %Q , %Q , %Q , %Q , %Q , %Q , %Q , %Q ) " );
    
    sqlSentence = sqlite3_mprintf( sql.c_str() 
                                ,serie->getSeriesUID().c_str()
                                ,serie->getSeriesNumber().c_str()
                                ,serie->getStudyUID().c_str()
                                ,serie->getSeriesModality().c_str()
                                ,serie->getProtocolName().c_str()
                                ,serie->getSeriesDescription().c_str()
                                ,serie->getSeriesPath().c_str()
                                ,serie->getBodyPartExaminated().c_str()
                                ,serie->getSeriesDate().c_str()
                                ,serie->getSeriesTime().c_str() );
                                
    databaseConnection->getLock();                                    
    stateDatabase = sqlite3_exec( databaseConnection->getConnection() , sqlSentence  , 0 , 0 , 0 ) ;          
    databaseConnection->releaseLock();
    
    state = databaseConnection->databaseStatus( stateDatabase );
    return state;
}

Status CacheSeriesDAL::querySeries( SeriesMask seriesMask , SeriesList &ls )
{
    DcmDataset* mask = NULL;
    int columns , rows , i = 0 , stateDatabase;
    Series series;
    char **resposta = NULL , **error = NULL;
    Status state;
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();
        
    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }    
        
    mask = seriesMask.getSeriesMask();
                     
    databaseConnection->getLock();
    stateDatabase = sqlite3_get_table( databaseConnection->getConnection() , buildSqlQuerySeries( &seriesMask ).c_str() , &resposta , &rows, &columns , error ); //connexio a la bdd,sentencia sql,resposta, numero de files,numero de columnss.
    databaseConnection->releaseLock();
    
    state = databaseConnection->databaseStatus( stateDatabase );
    if ( !state.good() ) return state;
    
    i = 1;//ignorem les capçaleres
    while (i <= rows )
    {   
        series.setSeriesUID( resposta [ 0 + i * columns ] );
        series.setSeriesNumber( resposta [ 1 + i * columns ] );
        series.setStudyUID( resposta [ 2 + i * columns ] );
        series.setSeriesModality( resposta [ 3 + i * columns ] );
        series.setSeriesDescription( resposta [ 4 + i * columns ] );
        series.setProtocolName( resposta [ 5 + i * columns ] );
        series.setSeriesPath( resposta [ 6 + i * columns ] );
        series.setBodyPartExaminated( resposta [ 7 + i * columns ] );
        series.setSeriesDate( resposta[ 8 + i * columns ] );
        series.setSeriesTime( resposta[ 9 + i * columns ] );
        
        ls.insert( series );
        i++;
    }
    return state;
}

Status CacheSeriesDAL::deleteSeries( std::string studyUID )
{
    std::string sql;
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();
    int stateDatabase;
    char *sqlSentence;
    
    databaseConnection->getLock();//nomes un proces a la vegada pot entrar a la cache
    
    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }
    
    sql.insert( 0 , "delete from series where StuInsUID = %Q" );
    
    
    sqlSentence = sqlite3_mprintf( sql.c_str() 
                                    ,studyUID.c_str() );
                                
    databaseConnection->getLock();                                    
    stateDatabase = sqlite3_exec( databaseConnection->getConnection() , sqlSentence  , 0 , 0 , 0 ) ;          
    databaseConnection->releaseLock();
 
    return databaseConnection->databaseStatus( stateDatabase );
}

std::string CacheSeriesDAL::buildSqlQuerySeries( SeriesMask *seriesMask )
{
    std::string sql;
    
    sql.insert( 0 , "select SerInsUID , SerNum , StuInsUID , SerMod , SerDes , ProNam, SerPath , BodParExa " );
    sql.append( ", SerDat , SerTim " );
    sql.append( " from series where StuInsUID = '" );
    sql.append( seriesMask->getStudyUID() );
    sql.append( "' order by SerDat , SerTim , SerNum" );
    
    return sql;
}

CacheSeriesDAL::~CacheSeriesDAL()
{
}

}
