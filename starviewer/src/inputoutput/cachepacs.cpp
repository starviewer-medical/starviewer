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


/**********************************************************************************************************                                      ZONA INSERTS                                                          *******************************************************************************************************/
  
Status CachePacs::insertStudy( Study *stu )
{
    std::string insertPatient , insertStudy , sql , patientName;
    int i;
    Status state;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState(50);
    }
    
    // Hi ha noms del pacients que depenent de la màquina tenen el nom format per cognoms^Nom, en aquest cas substituim ^ per espai
    patientName = stu->getPatientName();
    
    while ( patientName.find('^') != std::string::npos )
    {
        patientName.replace( patientName.find( '^' ) , 1 , " " , 1 );
    }
    
    m_DBConnect->getLock(); //s'insereix el pacient 
    i=sqlite_exec_printf( m_DBConnect->getConnection() , "Insert into Patient (PatId,PatNam,PatBirDat,PatSex) values (%Q,%Q,%Q,%Q)" , 0 , 0 , 0
                                ,stu->getPatientId().c_str()
                                ,patientName.c_str()
                                ,stu->getPatientBirthDate().c_str()
                                ,stu->getPatientSex().c_str()
                                );
    m_DBConnect->releaseLock();

    state = constructState( i );
    
    //si l'estat de l'operació és fals, però l'error és el 2019, significa que el pacient, ja existia a la bdd, per tant 
    //continuem inserint l'estudi, si es provoca qualsevol altre error parem
    if ( !state.good() && state.code() != 2019 ) return state; 
    
    sql.insert( 0 , "Insert into Study " ); //crem el el sql per inserir l'estudi ,al final fem un select per assignar a l'estudi l'id del PACS al que pertany
    sql.append( "(PatId, StuInsUID, StuID, StuDat, StuTim, RefPhyNam, AccNum, StuDes, Modali, " );
    sql.append( "OpeNam, Locati, AccDat, AccTim, AbsPath, Status, PacsID, PatAge) " );
    sql.append( "Values (%Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %i, %i, %Q, %Q, " );
    sql.append( "(select PacsID from PacsList where AETitle = %Q), %Q)" );//busquem l'id del PACS
    
    m_DBConnect->getLock();
    i=sqlite_exec_printf( m_DBConnect->getConnection(),sql.c_str() , 0 , 0 , 0
                                ,stu->getPatientId().c_str()
                                ,stu->getStudyUID().c_str()
                                ,stu->getStudyId().c_str()
                                ,stu->getStudyDate().c_str()
                                ,stu->getStudyTime().c_str()
                                ,""                        //Referring Physician Name
                                ,stu->getAccessionNumber().c_str()
                                ,stu->getStudyDescription().c_str()
                                ,stu->getStudyModality().c_str()   //Modality
                                ,""                        //Operator Name
                                ,""                        //Location
                                ,getDate()                 //Access Date
                                ,getTime()                 //Access Time
                                ,stu->getAbsPath().c_str()
                                ,"PENDING"                 //estat pendent perquè la descarrega de l'estudi encara no està completa               
                                ,stu->getPacsAETitle().c_str()
                                ,stu->getPatientAge().c_str()
                                );
    m_DBConnect->releaseLock();
                                
                                
    state = constructState( i );
                                
    return state;
}

Status CachePacs::insertSeries( Series *serie )
{
    int i;
    Status state;
    std::string sql;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState( 50 );
    }
    
    sql.insert( 0 , "Insert into Series " );
    sql.append( " ( SerInsUID , SerNum , StuInsUID , SerMod , ProNam , SerDes , SerPath , BodParExa , SerDat , SerTim ) " );
    sql.append( " values ( %Q , %Q , %Q , %Q , %Q , %Q , %Q , %Q , %Q , %Q ) " );
    
    m_DBConnect->getLock();
    i = sqlite_exec_printf( m_DBConnect->getConnection(),sql.c_str() , 0 , 0 , 0
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
    m_DBConnect->releaseLock();
    
    state = constructState( i );
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

Status CachePacs::queryStudy( StudyMask studyMask , StudyList &ls )
{
    int col , rows , i = 0 , estat;
    Study stu;

    char **resposta = NULL , **error = NULL;
    Status state;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState( 50 );
    }
    
    m_DBConnect->getLock();
    estat = sqlite_get_table( m_DBConnect->getConnection() , buildSqlQueryStudy( & studyMask ).c_str() , &resposta , &rows , &col , error ); //connexio a la bdd,sentencia sql,resposta, numero de files,numero de cols.
    m_DBConnect->releaseLock();
    state = constructState( estat );
    
    if ( !state.good() ) return state;
    
    i = 1;//ignorem les capçaleres
    while ( i <= rows )
    {   
        stu.setPatientId( resposta [ 0 + i * col ] );
        stu.setPatientName( resposta [ 1 + i * col ] );
        stu.setPatientAge( resposta [ 2+ i * col ] );
        stu.setStudyId( resposta [ 3+ i * col ] );
        stu.setStudyDate( resposta [ 4+ i * col ] );
        stu.setStudyTime( resposta [ 5+ i * col ] );
        stu.setStudyDescription( resposta [ 6+ i * col ] );
        stu.setStudyUID( resposta [ 7+ i * col ] );
        stu.setPacsAETitle( resposta [ 8 + i * col ] );
        stu.setAbsPath( resposta [ 9 + i * col ] );
        stu.setStudyModality( resposta [ 10 + i * col ] );
        ls.insert( stu );
        i++;
    }
    
    return state;
}

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

Status CachePacs::queryStudy( std::string studyUID , Study &study )
{
    int col , rows , i = 0 , estat;

    char **resposta = NULL , **error = NULL;
    Status state;
    std::string sql;
    
    sql.insert( 0 , "select Study.PatId, PatNam, PatAge, StuID, StuDat, StuTim, StuDes, StuInsUID, AETitle, AbsPath, Modali " );
    sql.append( " from Patient,Study,PacsList " );
    sql.append( " where Study.PatID=Patient.PatId " );
    sql.append( " and Status in ('RETRIEVED','RETRIEVING') " );
    sql.append( " and PacsList.PacsID=Study.PacsID" ); //busquem el nom del pacs
    sql.append( " and StuInsUID = '" );
    sql.append( studyUID );
    sql.append( "'" );
    
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
    if ( rows > 0 ) 
    {
        study.setPatientId( resposta [ 0 + i * col ] );
        study.setPatientName( resposta [ 1 + i * col ] );
        study.setPatientAge( resposta [ 2+ i * col ] );
        study.setStudyId( resposta [ 3+ i * col ] );
        study.setStudyDate( resposta [ 4+ i * col ] );
        study.setStudyTime( resposta [ 5+ i * col ] );
        study.setStudyDescription( resposta [ 6+ i * col ] );
        study.setStudyUID( resposta [ 7+ i * col ] );
        study.setPacsAETitle( resposta [ 8 + i * col ] );
        study.setAbsPath( resposta [ 9 + i * col ] );
        study.setStudyModality( resposta [ 10 + i * col ] );
    }
    else
    { 
        estat = 99; //no trobat
        state = constructState( estat );
    }
    
    return state;
}

std::string CachePacs::buildSqlQueryStudy(StudyMask* studyMask)
{
    std::string sql,patientName,patID,stuDatMin,stuDatMax,stuID,accNum,stuInsUID,stuMod,studyDate;
    
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
        sql.append( " and Study.PatID = '" );
        sql.append( patID );
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
        sql.append( " and StuID = '" );
        sql.append( stuID );
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

Status CachePacs::querySeries( SeriesMask seriesMask , SeriesList &ls )
{
    DcmDataset* mask = NULL;
    int col , rows , i = 0 , estat;
    Series series;
    char **resposta = NULL , **error = NULL;
    Status state;
        
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState( 50 );
    }    
        
    mask = seriesMask.getSeriesMask();
                     
    m_DBConnect->getLock();
    estat = sqlite_get_table( m_DBConnect->getConnection() , buildSqlQuerySeries( &seriesMask ).c_str() , &resposta , &rows, &col , error ); //connexio a la bdd,sentencia sql,resposta, numero de files,numero de cols.
    m_DBConnect->releaseLock();
    
    state = constructState( estat );
    if ( !state.good() ) return state;
    
    i = 1;//ignorem les capçaleres
    while (i <= rows )
    {   
        series.setSeriesUID( resposta [ 0 + i * col ] );
        series.setSeriesNumber( resposta [ 1 + i * col ] );
        series.setStudyUID( resposta [ 2 + i * col ] );
        series.setSeriesModality( resposta [ 3 + i * col ] );
        series.setSeriesDescription( resposta [ 4 + i * col ] );
        series.setProtocolName( resposta [ 5 + i * col ] );
        series.setSeriesPath( resposta [ 6 + i * col ] );
        series.setBodyPartExaminated( resposta [ 7 + i * col ] );
        series.setSeriesDate( resposta[8 + i*col] );
        series.setSeriesTime( resposta[9 + i*col] );
        ls.insert( series );
        i++;
    }
    return state;
}

std::string CachePacs::buildSqlQuerySeries( SeriesMask *seriesMask )
{
    std::string sql;
    
    sql.insert( 0 , "select SerInsUID , SerNum , StuInsUID , SerMod , SerDes , ProNam, SerPath , BodParExa " );
    sql.append( ", SerDat , SerTim " );
    sql.append( " from series where StuInsUID = '" );
    sql.append( seriesMask->getStudyUID() );
    sql.append( "' order by SerDat , SerTim , SerNum" );
    
    return sql;
}

Status CachePacs::queryImages( ImageMask imageMask , ImageList &ls )
{
    int col , rows , i = 0 , estat;
    Image image;
    char **resposta = NULL , **error = NULL;
    Status state;
    std::string absPath;
        
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState( 50 );
    }    
        
    m_DBConnect->getLock();
    estat = sqlite_get_table( m_DBConnect->getConnection() , buildSqlQueryImages( &imageMask ).c_str() , &resposta , &rows , &col , error ); //connexio a la bdd,sentencia sql,resposta, numero de files,numero de cols.
    m_DBConnect->releaseLock();
    
    state = constructState( estat );
    if ( !state.good() ) return state;
    
    i = 1;//ignorem les capçaleres
    while (i <= rows )
    {   
        image.setImageNumber(atoi( resposta [ 0 + i * col ] ) );
        
        //creem el path absolut
        absPath.erase();
        absPath.insert( 0 , resposta[1 + i * col ] );
        absPath.append( resposta [ 3 + i * col ] ); //incloem el directori de la serie
        absPath.append( "/" );
        absPath.append( resposta [ 5 + i * col ] ); //incloem el nom de la imatge
        image.setImagePath( absPath.c_str() );
        
        image.setStudyUID( resposta [ 2 + i * col ] );
        image.setSeriesUID( resposta [ 3 + i * col ] );
        image.setSoPUID( resposta [ 4 + i * col ] );        
        
        image.setImageName( resposta [ 5 + i * col ] );
        ls.insert( image );
        i++;
    }
    
    return state;
}

std::string CachePacs::buildSqlQueryImages( ImageMask *imageMask )
{
    std::string sql,imgNum;
    
    sql.insert( 0 , "select ImgNum, AbsPath, Image.StuInsUID, SerInsUID, SopInsUID, ImgNam from image,study where Image.StuInsUID = '" );
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

Status CachePacs::countImageNumber( ImageMask imageMask , int &imageNumber )
{
    int col , rows , i = 0 , estat;
    Series series;
    char **resposta = NULL , **error = NULL;
    Status state;
    std::string sql;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState( 50 );
    }
    
    m_DBConnect->getLock();
    estat = sqlite_get_table( m_DBConnect->getConnection() , buildSqlCountImageNumber( &imageMask ).c_str() , &resposta , &rows , &col , error );
    m_DBConnect->releaseLock();
    
    state = constructState( estat );
    
    if ( !state.good() ) return state;
    
    i = 1;//ignorem les capçaleres
   
    imageNumber = atoi( resposta [i] );
   
   return state;
}

std::string CachePacs::buildSqlCountImageNumber( ImageMask *imageMask )
{
    std::string sql;
    
    sql.insert( 0 , "select count(*) from image where StuInsUID = '" );
    sql.append( imageMask->getStudyUID() );
    sql.append( "' and SerInsUID = '" );
    sql.append( imageMask->getSeriesUID() );
    sql.append( "'" );

    return sql;
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

/*********************************************************************************************************
                                                         ZONA UPDATES                                      
 *********************************************************************************************************/

Status CachePacs::setStudyRetrieved( std::string studyUID )
{
    int i;
    Status state;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState( 50 );
    }
    
    m_DBConnect->getLock();
    i = sqlite_exec_printf( m_DBConnect->getConnection() , "update study set Status = %Q where StuInsUID= %Q" , 0 , 0 , 0 , "RETRIEVED" , studyUID.c_str() );
    m_DBConnect->releaseLock();
                                
    state = constructState( i );

    return state;
}

Status CachePacs::setStudyRetrieving( std::string studyUID )
{
    int i;
    Status state;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState( 50 );
    }
    
    m_DBConnect->getLock();
    i = sqlite_exec_printf( m_DBConnect->getConnection() , "update study set Status = %Q where StuInsUID= %Q" , 0 , 0 , 0 , "RETRIEVING" , studyUID.c_str() );
    m_DBConnect->releaseLock();
                                
    state = constructState( i );

    return state;
}

Status CachePacs::setStudyModality( std::string studyUID , std::string modality )
{
    int i;
    Status state;
    
    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState( 50 );
    }
    
    m_DBConnect->getLock();
    i = sqlite_exec_printf( m_DBConnect->getConnection() , "update study set Modali = %Q where StuInsUID= %Q" , 0 , 0 , 0 , modality.c_str() ,
              studyUID.c_str() );
    m_DBConnect->releaseLock();
                                
    state = constructState( i );

    return state;
                                
}

Status CachePacs::updateStudyAccTime( std::string studyUID )
{
    int i;
    Status state;
    std::string sql;
    
    //sqlite no permet en un update entra valors mes gran que un int, a través de la interfície c++ com guardem la mida en bytes fem
    //un string i hi afegim 6 zeros per passar Mb a bytes

    sql.insert( 0 , "Update Study Set AccDat = %i, " );//convertim l'espai en bytes
    sql.append( "AccTim = %i " );
    sql.append( "where StuInsUID = %Q" );

    if ( !m_DBConnect->connected() )
    {//el 50 es l'error de no connectat a la base de dades
        return constructState( 50 );
    }

    m_DBConnect->getLock();
    i = sqlite_exec_printf( m_DBConnect->getConnection(),sql.c_str(), 0 , 0 , 0 
                                ,getDate()
                                ,getTime()
                                ,studyUID.c_str() );
    m_DBConnect->releaseLock();
                                
    state = constructState( i );

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

int CachePacs::getTime()
{
    time_t hora;
    char cad[5];
    struct tm *tmPtr;

    hora = time( NULL );
    tmPtr = localtime( &hora );
    strftime( cad , 5 , "%H%M" , tmPtr );
  
    return atoi( cad );
}

int CachePacs::getDate()
{
    time_t hora;
    char cad[9];
    struct tm *tmPtr;

    hora = time( NULL );
    tmPtr = localtime( &hora );
    strftime( cad , 9 , "%Y%m%d" , tmPtr );
  
    return atoi( cad );
}

std::string CachePacs::replaceAsterisk( std::string original )
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

CachePacs::~CachePacs()
{
}

};
