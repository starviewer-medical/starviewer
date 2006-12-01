/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include <string>
#include <sqlite3.h>
#include <QString>

#include "cachestudydal.h"
#include "status.h"
#include "databaseconnection.h"
#include "study.h"
#include "studylist.h"
#include "studymask.h"
#include "cachepool.h"
#include "logging.h"

namespace udg {

CacheStudyDAL::CacheStudyDAL()
{
}

Status CacheStudyDAL::insertStudy( Study *study )
{
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();
    std::string insertPatient , insertStudy , sql , patientName , logMessage;
    int stateDatabase;
    Status state;
    char *sqlSentence , errorNumber[5];
    
    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }
    
    // Hi ha noms del pacients que depenent de la màquina tenen el nom format per cognoms^Nom, en aquest cas substituim ^ per espai
    patientName = study->getPatientName();
    
    while ( patientName.find( '^' ) != std::string::npos )
    {
        patientName.replace( patientName.find( '^' ) , 1 , " " , 1 );
    }
   
    sql.insert(0, "Insert into Patient ( PatId , PatNam , PatBirDat , PatSex ) values ( %Q , %Q , %Q , %Q )");
    
    sqlSentence = sqlite3_mprintf( sql.c_str() ,
                                        study->getPatientId().c_str() ,
                                        patientName.c_str() , 
                                        study->getPatientBirthDate().c_str() ,
                                        study->getPatientSex().c_str() );
    
    databaseConnection->getLock(); //s'insereix el pacient 
    stateDatabase = sqlite3_exec( databaseConnection->getConnection() , sqlSentence  , 0 , 0 , 0 ) ;
    databaseConnection->releaseLock();

    state = databaseConnection->databaseStatus( stateDatabase );
    
    //si l'stateDatabase de l'operació és fals, però l'error és el 2019, significa que el pacient, ja existia a la bdd, per tant 
    //continuem inserint l'estudi, si es provoca qualsevol altre error parem
    if ( !state.good() && state.code() != 2019 )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        return state;
    }
    
    sql.clear();
    sql.insert( 0 , "Insert into Study " ); //crem el el sql per inserir l'estudi ,al final fem un select per assignar a l'estudi l'id del PACS al que pertany
    sql.append( "( PatId , StuInsUID , StuID , StuDat , StuTim , RefPhyNam , AccNum , StuDes , Modali , " );
    sql.append( " OpeNam , Locati , AccDat , AccTim , AbsPath , Status , PacsID , PatAge ) " );
    sql.append( " Values ( %Q , %Q , %Q , %Q , %Q , %Q , %Q , %Q , %Q , %Q , %Q , %i , %i , %Q , %Q , " );
    sql.append( " ( select PacsID from PacsList where AETitle = %Q ) , %Q) " );//busquem l'id del PACS
    
    delete sqlSentence;
    sqlSentence = sqlite3_mprintf( sql.c_str()
                                ,study->getPatientId().c_str()
                                ,study->getStudyUID().c_str()
                                ,study->getStudyId().c_str()
                                ,study->getStudyDate().c_str()
                                ,study->getStudyTime().c_str()
                                ,""                        //Referring Physician Name
                                ,study->getAccessionNumber().c_str()
                                ,study->getStudyDescription().c_str()
                                ,study->getStudyModality().c_str()   //Modality
                                ,""                        //Operator Name
                                ,""                        //Location
                                ,getDate()                 //Access Date
                                ,getTime()                 //Access Time
                                ,study->getAbsPath().c_str()
                                ,"PENDING"                 //stateDatabase pendent perquè la descarrega de l'estudi encara no està completa               
                                ,study->getPacsAETitle().c_str()
                                ,study->getPatientAge().c_str()
                                );
    
    databaseConnection->getLock();
    stateDatabase = sqlite3_exec( databaseConnection->getConnection() , sqlSentence  , 0 , 0 , 0 ) ;
    databaseConnection->releaseLock();
                                
    state = databaseConnection->databaseStatus( stateDatabase );
    
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

Status CacheStudyDAL::queryStudy( StudyMask studyMask , StudyList &ls )
{
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();
    int columns , rows , i = 0 , stateDatabase;
    Study stu;
    char **resposta = NULL , **error = NULL , errorNumber[5];
    Status state;
    std::string logMessage;
    
    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }
    
    databaseConnection->getLock();
    stateDatabase = sqlite3_get_table( databaseConnection->getConnection() , buildSqlQueryStudy( & studyMask ).c_str() , &resposta , &rows , &columns , error ); //connexio a la bdd,sentencia sql ,resposta, numero de files,numero de cols.
    databaseConnection->releaseLock();
    state = databaseConnection->databaseStatus( stateDatabase );
    
    if ( !state.good() )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        return state;
    }
    
    i = 1;//ignorem les capçaleres
    while ( i <= rows )
    {   
        stu.setPatientId( resposta [ 0 + i * columns ] );
        stu.setPatientName( resposta [ 1 + i * columns ] );
        stu.setPatientAge( resposta [ 2+ i * columns ] );
        stu.setStudyId( resposta [ 3+ i * columns ] );
        stu.setStudyDate( resposta [ 4+ i * columns ] );
        stu.setStudyTime( resposta [ 5+ i * columns ] );
        stu.setStudyDescription( resposta [ 6+ i * columns ] );
        stu.setStudyUID( resposta [ 7+ i * columns ] );
        stu.setPacsAETitle( resposta [ 8 + i * columns ] );
        stu.setAbsPath( resposta [ 9 + i * columns ] );
        stu.setStudyModality( resposta [ 10 + i * columns ] );
        ls.insert( stu );
        i++;
    }
    
    return state;
}

Status CacheStudyDAL::queryOldStudies( std::string OldStudiesDate , StudyList &ls )
{
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();
    int columns , rows , i = 0 , stateDatabase;
    Study stu;
    std::string sql , logMessage;
    char errorNumber[5];
    
    sql.insert( 0 , "select PatId, StuID, StuDat, StuTim, StuDes, StuInsUID, AbsPath, Modali " );
    sql.append( " from Study" );
    sql.append( " where AccDat < " );
    sql.append( OldStudiesDate );
    sql.append( " and Status = 'RETRIEVED' " );
    sql.append( " order by StuDat,StuTim " );

    char **resposta = NULL , **error = NULL;
    Status state;
    
    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }
    
    databaseConnection->getLock();
    stateDatabase = sqlite3_get_table( databaseConnection->getConnection() , sql.c_str() , &resposta , &rows , &columns , error ); //connexio a la bdd,sentencia sql ,resposta, numero de files,numero de cols.
    databaseConnection->releaseLock();
    state = databaseConnection->databaseStatus( stateDatabase );
    
    if ( !state.good() )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        return state;
    }
        
    i = 1;//ignorem les capçaleres
    while ( i <= rows )
    {   
        stu.setPatientId( resposta [ 0 + i * columns ] );
        stu.setStudyId( resposta [ 1 + i * columns ] );
        stu.setStudyDate( resposta [ 2 + i * columns ] );
        stu.setStudyTime( resposta [ 3 + i * columns ] );
        stu.setStudyDescription( resposta [ 4 + i * columns ] );
        stu.setStudyUID( resposta [ 5 + i * columns ] );
        stu.setAbsPath( resposta [ 6 + i * columns ] );
        stu.setStudyModality( resposta [ 7 + i * columns ] );
        ls.insert( stu );
        i++;
    }
    
    return state;
}

Status CacheStudyDAL::queryStudy( std::string studyUID , Study &study )
{
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();
    int columns , rows , i = 0 , stateDatabase;
    char **resposta = NULL , **error = NULL , errorNumber[5];
    Status state;
    std::string sql , logMessage;
    
    sql.insert( 0 , "select Study.PatId, PatNam, PatAge, StuID, StuDat, StuTim, StuDes, StuInsUID, AETitle, AbsPath, Modali " );
    sql.append( " from Patient,Study,PacsList " );
    sql.append( " where Study.PatID=Patient.PatId " );
    sql.append( " and Status in ( 'RETRIEVED' , 'RETRIEVING' ) " );
    sql.append( " and PacsList.PacsID=Study.PacsID" ); //busquem el nom del pacs
    sql.append( " and StuInsUID = '" );
    sql.append( studyUID );
    sql.append( "'" );
    
    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }
    
    databaseConnection->getLock();
    stateDatabase = sqlite3_get_table( databaseConnection->getConnection() , sql.c_str() , &resposta , &rows , &columns , error ); //connexio a la bdd,sentencia sql ,resposta, numero de files,numero de cols.
    databaseConnection->releaseLock();
    
    state = databaseConnection->databaseStatus( stateDatabase );
    
    if ( !state.good() )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        return state;
    }
        
    i = 1;//ignorem les capçaleres
    if ( rows > 0 ) 
    {
        study.setPatientId( resposta [ 0 + i * columns ] );
        study.setPatientName( resposta [ 1 + i * columns ] );
        study.setPatientAge( resposta [ 2+ i * columns ] );
        study.setStudyId( resposta [ 3+ i * columns ] );
        study.setStudyDate( resposta [ 4+ i * columns ] );
        study.setStudyTime( resposta [ 5+ i * columns ] );
        study.setStudyDescription( resposta [ 6+ i * columns ] );
        study.setStudyUID( resposta [ 7+ i * columns ] );
        study.setPacsAETitle( resposta [ 8 + i * columns ] );
        study.setAbsPath( resposta [ 9 + i * columns ] );
        study.setStudyModality( resposta [ 10 + i * columns ] );
    }
    else
    { 
        stateDatabase = 99; //no trobat
        state = databaseConnection->databaseStatus( stateDatabase );
        if ( !state.good() )
        {
            sprintf( errorNumber , "%i" , state.code() );
            logMessage = "Error a la cache número ";
            logMessage.append( errorNumber );
            ERROR_LOG( logMessage.c_str() );
        }
    }
    
    return state;
}

Status CacheStudyDAL::delStudy( std::string studyUID )
{
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();
    Status state;
    int stateDatabase;
    char **resposta = NULL , **error = NULL;
    int columns , rows , studySize , i;
    std::string sql , absPathStudy , logMessage;
    CachePool cachePool;
    char *sqlSentence , errorNumber[5];
    
    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }

    /* La part d'esborrar un estudi com que s'ha d'accedir a diverses taules, ho farem en un transaccio per si falla alguna sentencia sql fer un rollback, i així deixa la taula en estat estable, no deixem anar el candau fins al final */ 
    databaseConnection->getLock();
    stateDatabase = sqlite3_exec( databaseConnection->getConnection() , "BEGIN TRANSACTION ", 0 , 0 , 0 );
     //comencem la transacció

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

    //sql per saber el directori on es guarda l'estudi
    sql.insert( 0 , "select AbsPath from study where StuInsUID = '" );
    sql.append( studyUID );
    sql.append( "'" );
      
    stateDatabase = sqlite3_get_table(databaseConnection->getConnection(),sql.c_str() , &resposta , &rows , &columns , error ); //connexio a la bdd,sentencia sql ,resposta, numero de files,numero de columnss.
     
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
    else if (  rows == 0 )
    {
        stateDatabase = sqlite3_exec( databaseConnection->getConnection() , "ROLLBACK TRANSACTION " , 0 , 0 , 0 );
        databaseConnection->releaseLock();
        
        state = databaseConnection->databaseStatus( 99 );//error 99 registre no trobat           
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        return state;
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

     stateDatabase = sqlite3_get_table( databaseConnection->getConnection() , sql.c_str() , &resposta , &rows , &columns , error ); //connexio a la bdd,sentencia sql ,resposta, numero de files,numero de columnss.
     
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
    else if (  rows == 0 )
    {    
        stateDatabase = sqlite3_exec( databaseConnection->getConnection() , "ROLLBACK TRANSACTION " , 0 , 0 , 0 );
        databaseConnection->releaseLock();
        state = databaseConnection->databaseStatus( 99 );//error 99 registre no trobat   
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        return state;
    }
    
    //ignorem el resposta [0], perque hi ha la capçalera
    if ( atoi( resposta [1] ) == 1 )
    {//si aquell pacient nomes te un estudi l'esborrem de la taula Patient

        sql.clear();  
        sql.insert( 0 , "delete from Patient where PatID in (select PatID from study where StuInsUID = %Q)" );
    
        sqlSentence = sqlite3_mprintf( sql.c_str() , studyUID.c_str() );
    
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
            return state;
        }    
    }
    
    //esborrem de la taula estudi    
    sql.clear();
    sql.insert(0, "delete from study where StuInsUID= %Q");
  
    sqlSentence = sqlite3_mprintf( sql.c_str() , studyUID.c_str() );
    
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
        return state;
    }
    
    sql.clear();
    sql.insert(0, "delete from series where StuInsUID= %Q");
  
    sqlSentence = sqlite3_mprintf( sql.c_str() , studyUID.c_str() );
    
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
        return state;
    }
    
    //calculem el que ocupava l'estudi per actualitzar l'espai actualitzat
    sql.clear();
    sql.insert( 0 , "select sum(ImgSiz) from image where StuInsUID= '" );
    sql.append( studyUID );
    sql.append( "'" );
    stateDatabase = sqlite3_get_table( databaseConnection->getConnection() , sql.c_str() , &resposta , &rows , &columns , error );
    
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

    if ( resposta[1] != NULL )
    {
        studySize = atoi( resposta [1] );
    }
    else studySize = 0;

    //esborrem de la taula image
    sql.clear();
    sql.insert(0, "delete from image where StuInsUID= %Q");
  
    sqlSentence = sqlite3_mprintf( sql.c_str() , studyUID.c_str() );
    
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
        return state;
    }

    sql.clear();
    sql.insert( 0 , "Update Pool Set Space = Space - %i " );
    sql.append( "where Param = 'USED'" );
  
    sqlSentence = sqlite3_mprintf( sql.c_str() , studySize  );
    
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
        return state;
    }
        
    stateDatabase = sqlite3_exec( databaseConnection->getConnection() , "COMMIT TRANSACTION " , 0 , 0 , 0 );
     //fem commit
    state = databaseConnection->databaseStatus( stateDatabase );
    if ( !state.good() )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        return state;
    }
    
    databaseConnection->releaseLock();        
    
    //una vegada hem esborrat les dades de la bd, podem esborrar les imatges, això s'ha de fer al final, perqué si hi ha un error i esborrem les
    //imatges al principi, no les podrem recuperar i la informació a la base de dades hi continuarà estant
    cachePool.removeStudy( absPathStudy );
    
    return state;  
}

Status CacheStudyDAL::delNotRetrievedStudies()
{
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();
    Status state;
    int stateDatabase;
    char **resposta = NULL , **error = NULL , errorNumber[5];
    int columns , rows , i;
    std::string sql , studyUID , logMessage;

    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }
    
    //cerquem els estudis pendents de finalitzar la descarrega
    sql.insert( 0 , "select StuInsUID from Study where Status in ( 'PENDING' , 'RETRIEVING' )" );
   
    databaseConnection->getLock();
    stateDatabase = sqlite3_get_table( databaseConnection->getConnection() , sql.c_str() , &resposta , &rows , &columns , error ); //connexio a la bdd,sentencia sql ,resposta, numero de files,numero de cols.
    databaseConnection->releaseLock();
    
    state = databaseConnection->databaseStatus( stateDatabase );
    if ( !state.good() )
    {
        sprintf( errorNumber , "%i" , state.code() );
        logMessage = "Error a la cache número ";
        logMessage.append( errorNumber );
        ERROR_LOG( logMessage.c_str() );
        return state;
    }
   
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

Status CacheStudyDAL::setStudyRetrieved( std::string studyUID )
{
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();
    int stateDatabase;
    Status state;
    std::string sql , logMessage;
    char *sqlSentence , errorNumber[5];
    
    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }

    sql.insert( 0 , "update study set Status = %Q " );
    sql.append( "where StuInsUID= %Q" );
  
    sqlSentence = sqlite3_mprintf( sql.c_str() , "RETRIEVED", studyUID.c_str()  );
    
    databaseConnection->getLock();
    stateDatabase = sqlite3_exec( databaseConnection->getConnection(), sqlSentence , 0, 0, 0);
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

Status CacheStudyDAL::setStudyRetrieving( std::string studyUID )
{
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();
    int stateDatabase;
    Status state;
    std::string sql , logMessage;
    char* sqlSentence , errorNumber[5];
    
    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }
    
    sql.insert( 0 , "update study set Status = %Q " );
    sql.append( "where StuInsUID= %Q" );
  
    sqlSentence = sqlite3_mprintf( sql.c_str() , "RETRIEVING", studyUID.c_str()  );
    
    databaseConnection->getLock();
    stateDatabase = sqlite3_exec( databaseConnection->getConnection(), sqlSentence , 0, 0, 0);
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

Status CacheStudyDAL::setStudyModality( std::string studyUID , std::string modality )
{
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();
    int stateDatabase;
    Status state;
    std::string sql , logMessage;
    char* sqlSentence , errorNumber[5];
    
    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }
    
    sql.insert( 0 , "update study set Modali = %Q " );
    sql.append( "where StuInsUID= %Q" );
  
    sqlSentence = sqlite3_mprintf( sql.c_str() , modality.c_str(), studyUID.c_str()  );
    
    databaseConnection->getLock();
    stateDatabase = sqlite3_exec( databaseConnection->getConnection(), sqlSentence , 0, 0, 0);
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

Status CacheStudyDAL::updateStudyAccTime( std::string studyUID )
{
    DatabaseConnection* databaseConnection = DatabaseConnection::getDatabaseConnection();
    int stateDatabase;
    Status state;
    std::string sql , logMessage;
    char* sqlSentence , errorNumber[5];
    
    sql.insert( 0 , "Update Study Set AccDat = %i, " );//convertim l'espai en bytes
    sql.append( "AccTim = %i " );
    sql.append( "where StuInsUID = %Q" );

    if ( !databaseConnection->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return databaseConnection->databaseStatus( 50 );
    }
    
    sqlSentence = sqlite3_mprintf( sql.c_str() , getDate(), getTime(), studyUID.c_str() );
    
    databaseConnection->getLock();
    stateDatabase = sqlite3_exec( databaseConnection->getConnection(), sqlSentence , 0, 0, 0);
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

std::string CacheStudyDAL::buildSqlQueryStudy(StudyMask* studyMask)
{
    std::string sql ,patientName,patID,stuDatMin,stuDatMax,stuID,accNum,stuInsUID,stuMod,studyDate;
    
    sql.insert( 0 , "select Study.PatId, PatNam, PatAge, StuID, StuDat, StuTim, StuDes, StuInsUID, AETitle, AbsPath, Modali " );
    sql.append( " from Patient,Study,PacsList " );
    sql.append( " where Study.PatID=Patient.PatId " );
    sql.append( " and Status = 'RETRIEVED' " );
    sql.append( " and PacsList.PacsID=Study.PacsID" ); //busquem el nom del pacs
    
    //llegim la informació de la màscara
    patientName = replaceAsterisk( studyMask->getPatientName() );
    patID = studyMask->getPatientId();
    studyDate = studyMask->getStudyDate();
    stuID = studyMask->getStudyId();
    accNum = studyMask->getAccessionNumber();
    stuMod = studyMask->getStudyModality();
    stuInsUID = studyMask->getStudyUID();
    
    //cognoms del pacient
    if ( patientName.length() > 0 )
    {
        sql.append( " and PatNam like '" );
        sql.append( patientName );
        sql.append( "' " );
    }          
    
    //Id del pacient
    if ( patID != "*" && patID.length() > 0 )
    {
        sql.append( " and Study.PatID like '" );
        sql.append( replaceAsterisk( patID ) );
        sql.append( "' " );
    }
    
    //data
    if ( studyDate.length() == 8 )
    {
        sql.append( " and StuDat = '" );
        sql.append( studyDate );
        sql.append( "' " );        
    }
    else if ( studyDate.length() == 9 ) 
    {
        if ( studyDate.at( 0 ) == '-' )
        {
            sql.append( " and StuDat <= '" );
            sql.append( studyDate.substr( 1 , 8 ) );
            sql.append( "' " );
        }
        else if ( studyDate.at( 8 ) == '-' )
        {
            sql.append( " and StuDat >= '" );
            sql.append( studyDate.substr( 0 , 8 ) );
            sql.append( "' " ); 
        }
    }
    else if ( studyDate.length() == 17 )
    {
        sql.append( " and StuDat between '" );
        sql.append( studyDate.substr( 0 , 8 ) );
        sql.append( "' and '" );
        sql.append( studyDate.substr( 9 , 8 ) );
        sql.append( "'" );
    }
    
    //id estudi
    if ( stuID != "*" && stuID.length() > 0 )
    {
        sql.append( " and StuID like '" );
        sql.append( replaceAsterisk( stuID ) );
        sql.append( "' " );
    }
    
    //Accession Number
    if ( accNum != "*" && accNum.length() > 0 )
    {
        sql.append( " and AccNum = '" );
        sql.append( accNum );
        sql.append( "' " );
    }
    
    if ( stuInsUID != "*" && stuInsUID.length() > 0 )
    {
        sql.append( " and StuInsUID = '" );
        sql.append( stuInsUID );
        sql.append( "' " );        
    }
    
    if ( stuMod != "*" && stuMod.length() > 0 )
    {
        sql.append( " and Modali in " );
        sql.append( stuMod );
    }
    
    return sql;
}

std::string CacheStudyDAL::replaceAsterisk( std::string original )
{
    std::string ret;
    
    ret = original;
    
    //string::npos es retorna quan no s'ha trobat el "*"
    while ( ret.find("*") != std::string::npos )
    {
        ret.replace( ret.find( "*" ) , 1 , "%" , 1 );
    }
    
    return ret;
}

int CacheStudyDAL::getTime()
{
    time_t hora;
    char cad[5];
    struct tm *tmPtr;

    hora = time( NULL );
    tmPtr = localtime( &hora );
    strftime( cad , 5 , "%H%M" , tmPtr );
  
    return atoi( cad );
}

int CacheStudyDAL::getDate()
{
    time_t hora;
    char cad[9];
    struct tm *tmPtr;

    hora = time( NULL );
    tmPtr = localtime( &hora );
    strftime( cad , 9 , "%Y%m%d" , tmPtr );
  
    return atoi( cad );
}

CacheStudyDAL::~CacheStudyDAL()
{
}

}
