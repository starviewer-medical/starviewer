/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "cachepacs.h"
#include "series.h"
#include "studylist.h"
#include "serieslist.h"
#include "status.h"
#include "image.h"
#include <time.h>
#include "cachepool.h"

namespace udg {

CachePacs::CachePacs()
{
   m_DBConnect = DatabaseConnection::getDatabaseConnection();
}

/**  Construeix l'estat en que ha finaltizat l'operació sol·licitada
  *            @param  [in] Estat de sqlite
  *            @return retorna l'estat de l'operació
  */
Status CachePacs::constructState(int numState)
{
//A www.sqlite.org/c_interface.html hi ha al codificacio dels estats que retorna el sqlite
    Status state;

    switch(numState)
    {//aqui tractem els errors que ens poden afectar de manera més directe, i els quals l'usuari pot intentar solucionbar                         
        case SQLITE_OK :        state.setStatus("Normal",true,0);
                                break;
        case SQLITE_ERROR :     state.setStatus("Database missing ",false,2001);
                                break;
        case SQLITE_CORRUPT :   state.setStatus("Database corrupted",false,2011);
                                break;
        case SQLITE_CONSTRAINT: state.setStatus("Constraint Violation",false,2019);
                                break;
        case 50 :               state.setStatus("Not connected to database",false,2050);
                                break;
      //aquests errors en principi no es poden donar, pq l'aplicació no altera cap element de l'estructura, si es produeix algun
      //Error d'aquests en principi serà perquè la bdd està corrupte o problemes interns del SQLITE, fent Numerror-2000 de l'estat
      //a la pàgina de www.sqlite.org podrem saber de quin error es tracta.
        default :               state.setStatus("SQLITE internal error",false,2000+numState); 
                                break;
    }
   return state;
}


/************************************************************************************************************************
  *                                       ZONA INSERTS                                                                  *
  ***********************************************************************************************************************/
  
/** Afegeix un nou estudi i pacient a la bd local, quant comencem a descarregar un nou estudi.
  *   La informació que insereix és :
  *        Si el pacient no existeix - PatientId
  *                                  - PatientName
  *                                  - PatientBirthDate
  *                                  - PatientSex  
  *
  *       Si l'estudi no existeix    - PatientID
  *                                  - StudyUID
  *                                  - StudyDate
  *                                  - StudyTime
  *                                  - StudyID
  *                                  - AccessionNumber
  *                                  - StudyDescription
  *                                  - Status
  *  La resta d'informació no estarà disponible fins que les imatges estiguin descarregades, 
  *                    
  *         @param Study[in]  Informació de l'estudi 
  *         @return retorna l'estat de l'inserció                                    
  */
Status CachePacs::insertStudy(Study *stu)
{
    
    std::string insertPatient,insertStudy,sql,patientName;
    int i;
    Status state;
    std::string::size_type pos;
    
    if (!m_DBConnect->connected())
    {//el 50 es l'error de no connectat a la base de dades
        return constructState(50);
    }
    
    // Hi ha noms del pacients que depenent de la màquina tenen el nom format per cognoms^Nom, en aquest cas substituim ^ per espai
    patientName = stu->getPatientName();
    pos = patientName.find('^',0);
    
    if (pos != std::string::npos)
    {
        patientName.replace(pos,1," ");
    } 
    
    m_DBConnect->getLock(); //s'insereix el pacient 
    i=sqlite_exec_printf(m_DBConnect->getConnection(),"Insert into Patient (PatId,PatNam,PatBirDat,PatSex) values (%Q,%Q,%Q,%Q)",0,0,0
                                ,stu->getPatientId().c_str()
                                ,patientName.c_str()
                                ,stu->getPatientBirthDate().c_str()
                                ,stu->getPatientSex().c_str()
                                );
    m_DBConnect->releaseLock();

    state = constructState(i);
    
    //si l'estat de l'operació és fals, però l'error és el 2019, significa que el pacient, ja existia a la bdd, per tant 
    //continuem inserint l'estudi, si es provoca qualsevol altre error parem
    if (!state.good() && state.code() != 2019) return state; 
    
    sql.insert(0,"Insert into Study "); //crem el el sql per inserir l'estudi ,al final fem un select per assignar a l'estudi l'id del PACS al que pertany
    sql.append("(PatId, StuInsUID, StuID, StuDat, StuTim, RefPhyNam, AccNum, StuDes, Modali, ");
    sql.append("OpeNam, Locati, AccDat, AccTim, AbsPath, Status, PacsID, PatAge) ");
    sql.append("Values (%Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %i, %i, %Q, %Q, ");
    sql.append("(select PacsID from PacsList where AETitle = %Q), %Q)");//busquem l'id del PACS
    
    m_DBConnect->getLock();
    i=sqlite_exec_printf(m_DBConnect->getConnection(),sql.c_str(),0,0,0
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
                                
                                
    state = constructState(i);
                                
    return state;
    
}

/** Insereix una sèrie a la caché
  *        @param series [in] Dades de la sèrie
  *        @return retorna l'estat de la inserció
  */
Status CachePacs::insertSeries(Series *serie)
{
    int i;
    Status state;
    std::string sql;
    
    if (!m_DBConnect->connected())
    {//el 50 es l'error de no connectat a la base de dades
        return constructState(50);
    }
    
    sql.insert(0,"Insert into Series (SerInsUID,SerNum,StuInsUID,SerMod,ProNam,SerDes,SerPath,BodParExa)");
    sql.append("values (%Q,%Q,%Q,%Q,%Q,%Q,%Q,%Q)");
    
    m_DBConnect->getLock();
    i = sqlite_exec_printf(m_DBConnect->getConnection(),sql.c_str(),0,0,0
                                ,serie->getSeriesUID().c_str()
                                ,serie->getSeriesNumber().c_str()
                                ,serie->getStudyUID().c_str()
                                ,serie->getSeriesModality().c_str()
                                ,serie->getProtocolName().c_str()
                                ,serie->getSeriesDescription().c_str()
                                ,serie->getSeriesPath().c_str()
                                ,serie->getBodyPartExaminated().c_str());
    m_DBConnect->releaseLock();
    
    state = constructState(i);
    return state;
}

/** Insereix la informació d'una imatge a la caché. I actualitza l'espai ocupat de la pool, com s'ha de fer un insert i un update aquests dos operacions
  * es fan dins el marc d'una transaccio, per mantenir coherent l'espai de la pool ocupat
  *        @param [in] dades de la imatge 
  *         @return retorna estat del mètode
  */
Status CachePacs::insertImage(Image *image)
{
    //no guardem el path de la imatge perque la el podem saber amb Study.AbsPath/SeriesUID/SopInsUID

    int i;
    Status state;
    std::string sql;
    
    if (!m_DBConnect->connected())
    {//el 50 es l'error de no connectat a la base de dades
        return constructState(50);
    }
    
    sql.insert(0,"Insert into Image (SopInsUID, StuInsUID, SerInsUID, ImgNum, ImgTim,ImgDat, ImgSiz, ImgNam) ");
    sql.append("values (%Q,%Q,%Q,%i,%Q,%Q,%i,%Q)");
    
    m_DBConnect->getLock();
    i = sqlite_exec_printf(m_DBConnect->getConnection(),"BEGIN TRANSACTION ",0,0,0);//comencem la transacció

    state = constructState(i);
    
    if (!state.good())
    {
        i = sqlite_exec_printf(m_DBConnect->getConnection(),"ROLLBACK TRANSACTION ",0,0,0);
        m_DBConnect->releaseLock();
        return state;
    }
    i = sqlite_exec_printf(m_DBConnect->getConnection(),sql.c_str(),0,0,0
                                ,image->getSoPUID().c_str()
                                ,image->getStudyUID().c_str()
                                ,image->getSeriesUID().c_str()
                                ,image->getImageNumber()
                                ,"0" //Image time
                                ,"0" //Image Date
                                ,image->getImageSize()
                                ,image->getImageName().c_str());   //IMage size
                                
    state = constructState(i);
    if (!state.good())
    {
        i = sqlite_exec_printf(m_DBConnect->getConnection(),"ROLLBACK TRANSACTION ",0,0,0);
        m_DBConnect->releaseLock();
        return state;
    }
                                    
    sql.clear();  
    sql.insert(0,"Update Pool Set Space = Space + %i ");
    sql.append("where Param = 'USED'");
    
    i = sqlite_exec_printf(m_DBConnect->getConnection(),sql.c_str(),0,0,0
                                ,image->getImageSize());
    
    state = constructState(i);
    if (!state.good())
    {
        i = sqlite_exec_printf(m_DBConnect->getConnection(),"ROLLBACK TRANSACTION ",0,0,0);
        m_DBConnect->releaseLock();
        return state;
    }
    
    i = sqlite_exec_printf(m_DBConnect->getConnection(),"COMMIT TRANSACTION ",0,0,0);
    
    m_DBConnect->releaseLock();
                                
    state = constructState(i);
    
    return state;
}

/*********************************************************************************************************************************************
 *                                                       ZONA DE LES QUERIES                                                                 *
 *********************************************************************************************************************************************/

/** Cerca els estudis que compleixen la màscara a la caché
  *            @param    Màscara de la cerca
  *            @param    StudyList amb els resultats
  *            @return retorna estat del mètode
  */
Status CachePacs::queryStudy(StudyMask studyMask,StudyList &ls)
{

    DcmDataset* mask=NULL;
    int col,rows,i=0,estat;
    Study stu;

    char **resposta=NULL,**error=NULL;
    Status state;
    
    if (!m_DBConnect->connected())
    {//el 50 es l'error de no connectat a la base de dades
        return constructState(50);
    }
    
    mask = studyMask.getMask();
    
    m_DBConnect->getLock();
    estat = sqlite_get_table(m_DBConnect->getConnection(),buildSqlQueryStudy(mask).c_str(),&resposta,&rows,&col,error); //connexio a la bdd,sentencia sql,resposta, numero de files,numero de cols.
    m_DBConnect->releaseLock();
    state = constructState(estat);
    
    if (!state.good()) return state;
    
    i = 1;//ignorem les capçaleres
    while (i <= rows)
    {   
        stu.setPatientId(resposta[0 + i*col]);
        stu.setPatientName(resposta[1 + i*col]);
        stu.setPatientAge(resposta[2+ i*col]);
        stu.setStudyId(resposta[3+ i*col]);
        stu.setStudyDate(resposta[4+ i*col]);
        stu.setStudyTime(resposta[5+ i*col]);
        stu.setStudyDescription(resposta[6+ i*col]);
        stu.setStudyUID(resposta[7+ i*col]);
        stu.setPacsAETitle(resposta[8 + i*col]);
        stu.setAbsPath(resposta[9 + i*col]);
        stu.setStudyModality(resposta[10 + i*col]);
        ls.insert(stu);
        i++;
    }
    
    return state;
    
}

/** Construeix la sentència sql per fer la query de l'estudi en funció dels parametres de cerca
  *         @param mascara de cerca
  *         @return retorna estat del mètode
  */
std::string CachePacs::buildSqlQueryStudy(DcmDataset* mask)
{
    std::string sql,patFirstNam,patID,stuDatMin,stuDatMax,stuID,accNum,patLastNam,stuInsUID,stuMod;
    
    sql.insert(0,"select Study.PatId, PatNam, PatAge, StuID, StuDat, StuTim, StuDes, StuInsUID, AETitle, AbsPath, Modali ");
    sql.append(" from Patient,Study,PacsList ");
    sql.append(" where Study.PatID=Patient.PatId ");
    sql.append(" and Status = 'RETRIEVED' ");
    sql.append(" and PacsList.PacsID=Study.PacsID"); //busquem el nom del pacs
    
    //llegim la informació de la màscara
    patFirstNam = getPatientFirstNameMask(mask);
    patLastNam = getPatientLastNameMask(mask);
    patID = getPatientIDMask(mask);
    stuDatMin = getStudyDateMaskMin(mask);
    stuDatMax = getStudyDateMaskMax(mask);
    stuID = getStudyIDMask(mask);
    accNum = getAccessionNumber(mask);
    stuMod = getStudyModalityMask(mask);

    stuInsUID = getStudyUID(mask);
    //Nom del pacient
    if (patFirstNam.length() > 0)
    {
        sql.append(" and PatNam like '% ");
        sql.append(patFirstNam);
        sql.append("%' ");
    }                
    
    //cognoms del pacient
    if (patLastNam.length() > 0)
    {
        sql.append(" and PatNam like '");
        sql.append(patLastNam);
        sql.append("%' ");
    }          
    
    //Id del pacient
    if (patID != "*" && patID.length() > 0)
    {
        sql.append(" and Study.PatID = '");
        sql.append(patID);
        sql.append("' ");
    }
    
    //data
    if (stuDatMin != "00000000" && stuDatMin.length() > 0)
    {
        sql.append(" and StuDat >= '");
        sql.append(stuDatMin);
        sql.append("' ");
    }
    
    if (stuDatMax != "99999999" && stuDatMax.length() > 0)
    {
        sql.append(" and Studat <= '");
        sql.append(stuDatMax);
        sql.append("' ");
    }
    
    //id estudi
    
    if (stuID != "*" && stuID.length() > 0)
    {
        sql.append(" and StuID = '");
        sql.append(stuID);
        sql.append("' ");
    }
    
    //Accession Number
    if (accNum != "*" && accNum.length() > 0)
    {
        sql.append(" and AccNum = '");
    
        sql.append(accNum);
        sql.append("' ");
    }
    
    if (stuInsUID != "*" && stuInsUID.length() > 0)
    {
        sql.append(" and StuInsUID = '");
        sql.append(stuInsUID);
        sql.append("' ");        
    }
    
    if (stuMod != "*" && stuMod.length() > 0)
    {
        sql.append(" and Modali in ");
        sql.append(stuMod);
    }
    
    return sql;
}

/** Cerca les sèries demanades a la màscara. Important! Aquesta acció només té en compte l'StudyUID de la màscara per fer la cerca, els altres camps de la màscara
  * els ignorarà!
  *         @param  mascarà de la serie amb l' sstudiUID a buscar
  *         @param  retorna la llista amb la sèries trobades
  *         @return retorna estat del mètode
  */
Status CachePacs::querySeries(SeriesMask seriesMask,SeriesList &ls)
{

    DcmDataset* mask = NULL;
    int col,rows,i = 0,estat;
     Series series;
    char **resposta = NULL,**error = NULL;
    Status state;
        
    if (!m_DBConnect->connected())
    {//el 50 es l'error de no connectat a la base de dades
        return constructState(50);
    }    
        
    mask = seriesMask.getSeriesMask();
                     
    m_DBConnect->getLock();
    estat = sqlite_get_table(m_DBConnect->getConnection(),buildSqlQuerySeries(mask).c_str(),&resposta,&rows,&col,error); //connexio a la bdd,sentencia sql,resposta, numero de files,numero de cols.
    m_DBConnect->releaseLock();
    
    state = constructState(estat);
    if (!state.good()) return state;
    
    i = 1;//ignorem les capçaleres
    while (i <= rows)
    {   
        series.setSeriesUID(resposta[0 + i*col]);
        series.setSeriesNumber(resposta[1 + i*col]);
        series.setStudyUID(resposta[2 + i*col]);
        series.setSeriesModality(resposta[3 + i*col]);
        series.setSeriesDescription(resposta[4 + i*col]);
        series.setProtocolName(resposta[5 + i*col]);
        series.setSeriesPath(resposta[6 + i*col]);
        series.setBodyPartExaminated(resposta[7 + i*col]);
        ls.insert(series);
        i++;
    }
    return state;
}

/** Construeix la sentència per buscar les sèries d'un estudi
  *            @param mask [in] màscara de cerca
  *            @return sentència sql
  */
std::string CachePacs::buildSqlQuerySeries(DcmDataset* mask)
{
    std::string sql;
    
    sql.insert(0,"select SerInsUID,SerNum,StuInsUID,SerMod,SerDes,ProNam,SerPath,BodParExa from series where StuInsUID = '");
    sql.append(getStudyUID(mask));
    sql.append("'");
    
    return sql;
}

/** Cerca les imatges demanades a la màscara. Important! Aquesta acció només té en compte l'StudyUID i el SeriesUID de la màscara per fer la cerca, els altres 
  * caps de la màscara els ignorarà!
  *         @param  mascara de les imatges a cercar
  *         @param llistat amb les imatges trobades
  *         @return retorna estat del mètode
  */
Status CachePacs::queryImages(ImageMask imageMask,ImageList &ls)
{

    DcmDataset* mask = NULL;
    int col,rows,i = 0,estat;
    Image image;
    char **resposta = NULL,**error = NULL;
    Status state;
    std::string absPath;
        
    if (!m_DBConnect->connected())
    {//el 50 es l'error de no connectat a la base de dades
        return constructState(50);
    }    
        
    mask = imageMask.getImageMask();
                     
    m_DBConnect->getLock();
    estat = sqlite_get_table(m_DBConnect->getConnection(),buildSqlQueryImages(mask).c_str(),&resposta,&rows,&col,error); //connexio a la bdd,sentencia sql,resposta, numero de files,numero de cols.
    m_DBConnect->releaseLock();
    
    state = constructState(estat);
    if (!state.good()) return state;
    
    i = 1;//ignorem les capçaleres
    while (i <= rows)
    {   
        image.setImageNumber(atoi(resposta[0 + i*col]));
        
        //creem el path absolut
        absPath.erase();
        absPath.insert(0,resposta[1 + i*col]);
        absPath.append(resposta[3 + i*col]); //incloem el directori de la serie
        absPath.append("/");
        absPath.append(resposta[5 + i*col]); //incloem el nom de la imatge
        image.setImagePath(absPath.c_str());
        
        image.setStudyUID(resposta[2 + i*col]);
        image.setSeriesUID(resposta[3 + i*col]);
        image.setSoPUID(resposta[4 + i*col]);        
        
        image.setImageName(resposta[5 + i *col]);
        ls.insert(image);
        i++;
    }
    
    return state;
}

/** Construeix la sentència per buscar les Imatges d'una sèrie
  *            @param mask [in] màscara de cerca
  *            @return sentència sql
  */
std::string CachePacs::buildSqlQueryImages(DcmDataset* mask)
{
    std::string sql,imgNum;
    
    sql.insert(0,"select ImgNum, AbsPath, Image.StuInsUID, SerInsUID, SopInsUID, ImgNam from image,study where Image.StuInsUID = '");
    sql.append(getStudyUID(mask));
    sql.append("' and SerInsUID = '");
    sql.append(getSeriesUID(mask));
    sql.append("' and Study.StuInsUID = Image.StuInsUID ");
    
    imgNum = getImageNumber(mask);
    
    if (imgNum.length() > 0)
    {
        sql.append(" and ImgNum = ");
        sql.append(imgNum);
        sql.append(" ");
    }
    
    sql.append(" order by ImgNum");
    
    return sql;
}

/** compta les imatges d'una sèrie 
  *            @param series [in] mascarà de la serie a comptar les images. Las màscara ha de contenir el UID de l'estudi i de la sèrie
  *            @param imageNumber [out] conte el nombre d'imatges
  *            @return retorna estat del mètode  
  */
Status CachePacs::countImageNumber(ImageMask imageMask,int &imageNumber)
{
    DcmDataset* mask = NULL;
    int col,rows,i = 0,estat;
    Series series;
    char **resposta = NULL,**error = NULL;
    Status state;
    std::string sql;
    
    if (!m_DBConnect->connected())
    {//el 50 es l'error de no connectat a la base de dades
        return constructState(50);
    }
    
    mask = imageMask.getImageMask();
                 
    sql = buildSqlCountImageNumber(mask);
    
    m_DBConnect->getLock();
    estat = sqlite_get_table(m_DBConnect->getConnection(),sql.c_str(),&resposta,&rows,&col,error);;
    m_DBConnect->releaseLock();
    
    state = constructState(estat);
    
    if (!state.good()) return state;
    
    i = 1;//ignorem les capçaleres
   
    imageNumber = atoi(resposta[i]);
   
   return state;
}

/** Construiex la sentència sql per comptar el nombre d'imatges de la sèrie d'un estudi
  *            @param mask [in]
  */
std::string CachePacs::buildSqlCountImageNumber(DcmDataset* mask)
{
    std::string sql;
    
    sql.insert(0,"select count(*) from image where StuInsUID = '");
    sql.append(getStudyUID(mask));
    sql.append("' and SerInsUID = '");
    sql.append(getSeriesUID(mask));
    sql.append("'");

    return sql;
}

/****************************************************************************************************************************************************
 *                                                    ZONA DELETE                                                                                   *
 ****************************************************************************************************************************************************
 */


/** Esborra un estudi de la cache, l'esborra la taula estudi,series, i image, i si el pacient d'aquell estudi, no té cap altre estudi a la cache local
  * tambe esborrem el pacient
  *            @param std::string [in] UID de l'estudi
  *            @return estat de l'operació
  */
Status CachePacs::delStudy(std::string studyUID)
{
    Status state;
    int estat;
    char **resposta = NULL,**error = NULL;
    int col,rows,studySize,i;
    std::string sql,absPathStudy;
    CachePool *cacheSpool = CachePool::getCachePool();

    if (!m_DBConnect->connected())
    {//el 50 es l'error de no connectat a la base de dades
        return constructState(50);
    }

    /* La part d'esborrar un estudi com que s'ha d'accedir a diverses taules, ho farem en un transaccio per si falla alguna 
      sentencia sql fer un rollback, i així deixa la taula en estat estable, no deixem anar el candau fins al final */ 
    m_DBConnect->getLock();
    estat = sqlite_exec_printf(m_DBConnect->getConnection(),"BEGIN TRANSACTION ",0,0,0);//comencem la transacció

    state=constructState(estat);
    if (!state.good())
    {
        estat = sqlite_exec_printf(m_DBConnect->getConnection(),"ROLLBACK TRANSACTION ",0,0,0);
        m_DBConnect->releaseLock();
        return state;
    }

    //sql per saber el directori on es guarda l'estudi
    sql.clear();
    sql.insert(0,"select AbsPath from study where PatID in (select PatID from study where StuInsUID = '");
    sql.append(studyUID);
    sql.append("')");
      

    estat = sqlite_get_table(m_DBConnect->getConnection(),sql.c_str(),&resposta,&rows,&col,error); //connexio a la bdd,sentencia sql,resposta, numero de files,numero de cols.
     
    state = constructState(estat);
    if (!state.good())
    {
        estat = sqlite_exec_printf(m_DBConnect->getConnection(),"ROLLBACK TRANSACTION ",0,0,0);
        m_DBConnect->releaseLock();
        return state;
    }       
        
    absPathStudy = resposta[1];
    
    //sql per saber quants estudis te el pacient
    sql.clear();
    sql.insert(0,"select count(*) from study where PatID in (select PatID from study where StuInsUID = '");
    sql.append(studyUID);
    sql.append("')");
      

    estat = sqlite_get_table(m_DBConnect->getConnection(),sql.c_str(),&resposta,&rows,&col,error); //connexio a la bdd,sentencia sql,resposta, numero de files,numero de cols.
     
    state = constructState(estat);
    if (!state.good())
    {
        estat = sqlite_exec_printf(m_DBConnect->getConnection(),"ROLLBACK TRANSACTION ",0,0,0);
        m_DBConnect->releaseLock();
        return state;
    }   
    
    //ignorem el resposta [0], perque hi ha la capçalera
    if (atoi(resposta[1]) == 1)
    {//si aquell pacient nomes te un estudi l'esborrem de la taula Patient
        estat = sqlite_exec_printf(m_DBConnect->getConnection(),"delete from Patient where PatID in (select PatID from study where StuInsUID = %Q)",0,0,0
                                ,studyUID.c_str());
                                
        state = constructState(estat);
        if (!state.good())
        {
            estat=sqlite_exec_printf(m_DBConnect->getConnection(),"ROLLBACK TRANSACTION ",0,0,0);
            m_DBConnect->releaseLock();
            return state;
        }    
    }
    
    //esborrem de la taula estudi    
    estat = sqlite_exec_printf(m_DBConnect->getConnection(),"delete from study where StuInsUID= %Q",0,0,0,studyUID.c_str());
    
    state = constructState(estat);
    if (!state.good())
    {
        estat = sqlite_exec_printf(m_DBConnect->getConnection(),"ROLLBACK TRANSACTION ",0,0,0);
        m_DBConnect->releaseLock();
        return state;
    }

    //esborrem de la taula series
    estat = sqlite_exec_printf(m_DBConnect->getConnection(),"delete from series where StuInsUID= %Q",0,0,0,studyUID.c_str());
    state = constructState(estat);
    if (!state.good())
    {
        estat = sqlite_exec_printf(m_DBConnect->getConnection(),"ROLLBACK TRANSACTION ",0,0,0);
        m_DBConnect->releaseLock();
        return state;
    }
    
//     //calculem el que ocupava l'estudi per actualitzar l'espai actualitzat
    sql.clear();
    sql.insert(0,"select sum(ImgSiz) from image where StuInsUID= '");
    sql.append(studyUID);
    sql.append("'");
    estat = sqlite_get_table(m_DBConnect->getConnection(),sql.c_str(),&resposta,&rows,&col,error);
    
    state = constructState(estat);
    if (!state.good())
    {
        estat = sqlite_exec_printf(m_DBConnect->getConnection(),"ROLLBACK TRANSACTION ",0,0,0);
        m_DBConnect->releaseLock();
        return state;
    }
    i = 1;//ignorem les capçaleres
    studySize = atoi(resposta[i]);

      
    //esborrem de la taula image
    estat = sqlite_exec_printf(m_DBConnect->getConnection(),"delete from image where StuInsUID= %Q",0,0,0,studyUID.c_str());
    state = constructState(estat);
    if (!state.good())
    {
        estat = sqlite_exec_printf(m_DBConnect->getConnection(),"ROLLBACK TRANSACTION ",0,0,0);
        m_DBConnect->releaseLock();
        return state;
    }

    sql.clear();    
    sql.insert(0,"Update Pool Set Space = Space + %i ");
    sql.append("where Param = 'USED'");
    
    estat = sqlite_exec_printf(m_DBConnect->getConnection(),sql.c_str(),0,0,0
                                ,studySize);
                                
    state = constructState(estat);
    if (!state.good())
    {
        estat = sqlite_exec_printf(m_DBConnect->getConnection(),"ROLLBACK TRANSACTION ",0,0,0);
        m_DBConnect->releaseLock();
        return state;
    }
        
    estat = sqlite_exec_printf(m_DBConnect->getConnection(),"COMMIT TRANSACTION",0,0,0); //fem commit
    state = constructState(estat);
    if (!state.good())
    {
        return state;
    }
    
    m_DBConnect->releaseLock();        
    
    //una vegada hem esborrat les dades de la bd, podem esborrar les imatges, això s'ha de fer al final, perqué si hi ha un error i esborrem les
    //imatges al principi, no les podrem recuperar i la informació a la base de dades hi continuarà estant
    cacheSpool->removeStudy(absPathStudy);
    
    return state;
   
}


/** Aquesta acció es per mantenir la coherencia de la base de dades, si ens trobem estudis al iniciar l'aplicació que tenen l'estat pendent
  * vol dir que l'aplicació en l'anterior execussió ha finalitzat anòmalament, per tant aquest estudis en estat pendents, les seves sèrie i 
  * imatges han de ser borrades perquè es puguin tornar a descarregar. Aquesta acció és simplement per seguretat!
  *            @return estat de l'operació
  */
Status CachePacs::delPendingStudies()
{
    Status state;
    int estat;
    char **resposta = NULL,**error = NULL;
    int col,rows,i;
    std::string sql,studyUID;

    if (!m_DBConnect->connected())
    {//el 50 es l'error de no connectat a la base de dades
        return constructState(50);
    }
    
    //cerquem els estudis pendents de finalitzar la descarrega
    sql.insert(0,"select StuInsUID from Study where Status ='PENDING'");
   
    m_DBConnect->getLock();
    estat = sqlite_get_table(m_DBConnect->getConnection(),sql.c_str(),&resposta,&rows,&col,error); //connexio a la bdd,sentencia sql,resposta, numero de files,numero de cols.
    m_DBConnect->releaseLock();
    
    state = constructState(estat);
    if (!state.good()) return state;
   
    //ignorem el resposta [0], perque hi ha la capçalera
    i = 1;
    
    while (i <= rows)
    {   
        studyUID.erase();
        studyUID.insert(0,resposta[i]);
        state = delStudy(studyUID);
        if (!state.good())
        {
            break;
        }
        i++;
    }
    
    return state;
   
}

/** Esborra totes les dades de la cache, exceptuant la taula PacsList, ja que no pertany a l'ambit d'aquesta classe
  *         @return retorna estat del mètode
  */
Status CachePacs::clearCache()
{
    int col,rows,i = 0,estat;
    Series series;
    char **resposta = NULL,**error = NULL;
    Status state;
    std::string sql;
    CachePool *pool = CachePool::getCachePool();
    
    
    if (!m_DBConnect->connected())
    {//el 50 es l'error de no connectat a la base de dades
        return constructState(50);
    }
    
    //els estudis pendents no els esborrem, perque l'usuari els esta descarregant en aquell moment
    //seleccionem el UID de tots els estudis a esborrar
    sql.insert(0,"select StuInsUID from study ");
    sql.append(" where Status <> 'PENDING'");                     
    m_DBConnect->getLock();
    estat = sqlite_get_table(m_DBConnect->getConnection(),sql.c_str(),&resposta,&rows,&col,error); //connexio a la bdd,sentencia sql,resposta, numero de files,numero de cols.
    m_DBConnect->releaseLock();
    
    state = constructState(estat);
    if (!state.good()) return state;
    
    i = 1;//ignorem les capçaleres
    while (i <= rows)
    {   
        delStudy(resposta[i]); //esborrar l'etudi
        i++;
    }
    
    pool->resetPoolSpace();
    
    return state;
}

/************************************************************************************************************************************************
 *                                                        ZONA UPDATES                                                                          *
 ************************************************************************************************************************************************
 */

/** Updata un estudi pendent de descarregar a l'estat de descarregat
  *        @param Uid de l'estudi a actualitzar
  *        @return retorna estat del mètode
  */
Status CachePacs::setStudyRetrieved(std::string studyUID)
{
    int i;
    Status state;
    
    if (!m_DBConnect->connected())
    {//el 50 es l'error de no connectat a la base de dades
        return constructState(50);
    }
    
    m_DBConnect->getLock();
    i = sqlite_exec_printf(m_DBConnect->getConnection(),"update study set Status = %Q where StuInsUID= %Q",0,0,0,"RETRIEVED",studyUID.c_str());
    m_DBConnect->releaseLock();
                                
    state=constructState(i);

    return state;
                                
}

/** Updata la modalitat d'un estudi
  *        @param Uid de l'estudi a actualitzar
  *        @param Modalitat de l'estudi
  *        @return retorna estat del mètode
  */
Status CachePacs::setStudyModality(std::string studyUID,std::string modality)
{
    int i;
    Status state;
    
    if (!m_DBConnect->connected())
    {//el 50 es l'error de no connectat a la base de dades
        return constructState(50);
    }
    
    m_DBConnect->getLock();
    i = sqlite_exec_printf(m_DBConnect->getConnection(),"update study set Modali = %Q where StuInsUID= %Q",0,0,0,modality.c_str(),
              studyUID.c_str());
    m_DBConnect->releaseLock();
                                
    state=constructState(i);

    return state;
                                
}

/** actualitza l'última vegada que un estudi ha estat visualitzat, d'aquesta manera quant haguem d'esborrar estudis
  * automàticament per falta d'espai, esborarrem els que fa més temps que no s'han visualitzat
  *        @param UID de l'estudi
  *        @param hora de visualització de l'estudi format 'hhmm'
  *        @param data visualització de l'estudi format 'yyyymmdd'
  *        @return estat el mètode
  */
Status CachePacs::updateStudyAccTime(std::string studyUID)
{
    int i;
    Status state;
    std::string sql;
    
    //sqlite no permet en un update entra valors mes gran que un int, a través de la interfície c++ com guardem la mida en bytes fem
    //un string i hi afegim 6 zeros per passar Mb a bytes

    sql.insert(0,"Update Study Set AccDat = %i, ");//convertim l'espai en bytes
    sql.append("AccTim = %i ");
    sql.append("where StuInsUID = %Q");

    if (!m_DBConnect->connected())
    {//el 50 es l'error de no connectat a la base de dades
        return constructState(50);
    }

    m_DBConnect->getLock();
    i = sqlite_exec_printf(m_DBConnect->getConnection(),sql.c_str(),0,0,0
                                ,getDate()
                                ,getTime()
                                ,studyUID.c_str());
    m_DBConnect->releaseLock();
                                
    state=constructState(i);

    return state;
}


/***************************************************************************************************************************************
 *                                                MANTENIMENT DE LA CACHE                                                              *
 ***************************************************************************************************************************************
 */

/** Compacta la base de dades de la cache, per estalviar espai
  *        @return estat del mètode  
  */
Status CachePacs::compactCachePacs()
{
    int i;
    Status state;
    std::string sql;
    
    if (!m_DBConnect->connected())
    {//el 50 es l'error de no connectat a la base de dades
        return constructState(50);
    }
    
    sql.insert(0,"vacuum");//amb l'acció vacuum es compacta la base de dades
    
    m_DBConnect->getLock();
    i = sqlite_exec_printf(m_DBConnect->getConnection(),sql.c_str(),0,0,0);
    m_DBConnect->releaseLock();
                                
    state = constructState(i);

    return state;
}

/*************************************************************************************************************************************
 *                                            ZONA DE LLEGIR LES DADES DE LA MASCARA                                                 *
 *************************************************************************************************************************************
 */
 
/** Extreu de la mascara el nom del pacient a buscar el format del Patient Name és "cognoms* Nom*" o "*" d'aquest format treiem el nom
  *            @param mask [in] màscara de la cerca
  *            @return   nom del pacient a buscar
  */  
std::string CachePacs::getPatientFirstNameMask(DcmDataset* mask)
{
    const char * name = NULL;
    std::string patientName;
    std::string::size_type ipos = 0;
    
    /*El nom esta format per cognoms*^nom, per agafar el nom hem d'agafar el que hi ha despres de *^ */
    
    DcmTagKey patientNameTagKey (DCM_PatientsName);
    OFCondition ec;
    ec = mask->findAndGetString( patientNameTagKey, name, OFFalse );
    
    if (name == NULL) return "";
    
    patientName.insert(0,name);
    
    if (patientName != "*")
    {
        ipos = patientName.find("* ");
        //si és un * sol vol dir no hi ha mascara de nom
        if (patientName.substr(ipos+2,1) != "*")
        {
            patientName = patientName.substr(ipos+2,patientName.length());
            ipos = patientName.find("*"); //treiem el * del final
            patientName.replace(ipos,1,"");
            return patientName;
        }
        else return "";
    }
    else return "";
}

/** Extreu de la mascara els cognoms del pacient a buscar. El format del Patient Name és "cognoms* Nom*" o "*" d'aquest format treiem el cognom
  *            @param mask [in] màscara de la cerca
  *            @return   cognoms del pacient a buscar
  */  
std::string CachePacs::getPatientLastNameMask(DcmDataset* mask)
{
    const char * name = NULL;
    std::string patientName;
    std::string::size_type ipos = 0;
    
        /*El nom esta format per cognoms*^nom, per agafar el nom hem d'agafar el que hi ha abans de *^ */
    DcmTagKey patientNameTagKey (DCM_PatientsName);
    OFCondition ec;
    ec = mask->findAndGetString( patientNameTagKey, name, OFFalse );
    
    if (name == NULL) return "";
    
    patientName.insert(0,name);
    
    if (patientName != "*")
    {
        ipos = patientName.find("* ");
        //Si esta a la posicio 0 vol dir que no hi ha mascara de cognoms
        if (ipos != 0)
        {
            patientName = patientName.substr(0,ipos);
            return patientName;
        }
        else return "";
    }
    else return "";
}

/** Extreu de la mascara el Id del pacient a buscar
  *            @param mask [in] màscara de la cerca
  *            @return   ID del pacient a buscar
  */
std::string CachePacs::getPatientIDMask(DcmDataset* mask)
{
    const char * ID = NULL;
    std::string patientID;
    
    DcmTagKey patientIDTagKey (DCM_PatientID);
    OFCondition ec;
    ec = mask->findAndGetString( patientIDTagKey, ID, OFFalse );
    
    
    if (ID != NULL) patientID.insert(0,ID);
    
    return patientID;
}


/** Extreu de la mascara el Id de l'estudi a buscar
  *            @param mask [in] ID de l'estudi de la cerca
  *            @return   ID de l'estudi a buscar
  */
std::string CachePacs::getStudyIDMask(DcmDataset* mask)
{
    const char * ID = NULL;
    std::string studyID;
    
    DcmTagKey studyIDTagKey (DCM_StudyID);
    OFCondition ec;
    ec = mask->findAndGetString( studyIDTagKey, ID, OFFalse );
    
    if (ID != NULL) studyID.insert(0,ID);
    
    return studyID;
}

/** Extreu de la mascara de la modalitat de l'estudi a buscar
  *            @param mask [in] ID de l'estudi de la cerca
  *            @return   ID de l'estudi a buscar
  */
std::string CachePacs::getStudyModalityMask(DcmDataset* mask)
{
    const char * mod = NULL;
    std::string studyModality;
    
    DcmTagKey studyModalityTagKey (DCM_ModalitiesInStudy);
    OFCondition ec;
    ec = mask->findAndGetString( studyModalityTagKey, mod, OFFalse );
    
    if (mod != NULL) studyModality.insert(0,mod);
    
    return studyModality;
}

/** Extreu de la mascara el accession number de l'estudi a buscar
  *            @param mask [in] accession number de l'estudi de la cerca
  *            @return   accession number de l'estudi a buscar
  */
std::string CachePacs::getAccessionNumber(DcmDataset* mask)
{
    const char * aNumber = NULL;
    std::string accessionNumber;
    
    DcmTagKey accessionNumberTagKey (DCM_AccessionNumber);
    OFCondition ec;
    ec = mask->findAndGetString( accessionNumberTagKey, aNumber, OFFalse );
    
    if (aNumber != NULL) accessionNumber.insert(0,aNumber);
        
    return accessionNumber;
}


/** Extreu de la data mínima a partir de la qual hem de començar a buscar els estudis
  *            @param mask [in] màscara de la cerca
  *            @return   Data a partir de la qual hem de començar a buscar els estudis
  */
std::string CachePacs::getStudyDateMaskMin(DcmDataset* mask)
{
    const char * date=NULL;
    std::string studyDate,minDate;
    
    DcmTagKey studyDateTagKey (DCM_StudyDate);
    OFCondition ec;
    ec = mask->findAndGetString( studyDateTagKey, date, OFFalse );
    
    if (date==NULL) return "";
    
    studyDate.insert(0,date);
    
    if (studyDate.length()==9 && studyDate.substr(8,1)=="-")
    {
        minDate.insert(0,studyDate.substr(0,8)); 
    }     
    else if (studyDate.length()==17)
    {
        minDate.insert(0,studyDate.substr(0,8));
    }
    else
    {
        minDate.insert(0,"00000000");
    }
    
    return minDate;
}

/** Extreu de la data màxima fins la qual busqyem els estudis
  *            @param mask [in] màscara de la cerca
  *            @return   Data a fins la qual busquem els estudis
  */
std::string CachePacs::getStudyDateMaskMax(DcmDataset* mask)
{
    const char * date = NULL;
    std::string studyDate,maxDate;
    
    DcmTagKey studyDateTagKey (DCM_StudyDate);
    OFCondition ec;
    ec = mask->findAndGetString( studyDateTagKey, date, OFFalse );;
    
    if (date == NULL) return "";
    
    studyDate.insert(0,date);
    
    if (studyDate.length()==9 && studyDate.substr(0,1)=="-") 
    {
        maxDate.insert(0,studyDate.substr(1,9)); 
    }
    else if (studyDate.length() == 17)
    {
        maxDate.insert(0,studyDate.substr(9,17));
    }
    else maxDate.insert(0,"99999999");
    
    return maxDate;
}

/** Extreu de la màsca l'estudi UID
  *            @param mask [in] màscara de la cerca
  *            @return   Estudi UID que cerquem
  */
std::string CachePacs::getStudyUID(DcmDataset* mask)
{
    const char * UID=NULL;
    std::string studyUID;
    
    DcmTagKey studyUIDTagKey (DCM_StudyInstanceUID);
    OFCondition ec;
    ec = mask->findAndGetString( studyUIDTagKey, UID, OFFalse );;
    
    if (UID != NULL) studyUID.insert(0,UID);
        
    return studyUID;
}

/** Extreu de la màscarael series UID
  *            @param mask [in] màscara de la cerca
  *            @return   series UID que cerquem
  */
std::string CachePacs::getSeriesUID(DcmDataset* mask)
{
    const char * UID = NULL;
    std::string seriesUID;
    
    DcmTagKey seriesUIDTagKey (DCM_SeriesInstanceUID);
    OFCondition ec;
    ec = mask->findAndGetString( seriesUIDTagKey, UID, OFFalse );;
    
    if (UID != NULL) seriesUID.insert(0,UID);
        
    return seriesUID;
}


/** Extreu de la màscarael series UID
  *            @param mask [in] màscara de la cerca
  *            @return   series UID que cerquem
  */
std::string CachePacs::getImageNumber(DcmDataset* mask)
{
    const char * num = NULL;
    std::string imgNum;
    
    DcmTagKey instanceNumberTagKey (DCM_InstanceNumber);
    OFCondition ec;
    ec = mask->findAndGetString(instanceNumberTagKey, num, OFFalse );;
    
    if (num == NULL)  return "";
   
    imgNum.insert(0,num);
    
    if (imgNum == "*") return "";
    else return imgNum;
        
    return imgNum;
}
/** retorna l'hora del sistema
  *     @return retorna l'hora del sistema en format HHMM
  */
int CachePacs::getTime()
{
  time_t hora;
  char cad[5];
  struct tm *tmPtr;

  hora = time(NULL);
  tmPtr = localtime(&hora);
  strftime( cad, 5, "%H%M", tmPtr );
  
  return atoi(cad);
}

/** retorna la data del sistema
  *    @return retorna la data del sistema en format yyyymmdd
  */
int CachePacs::getDate()
{
  time_t hora;
  char cad[9];
  struct tm *tmPtr;

  hora = time(NULL);
  tmPtr = localtime(&hora);
  strftime( cad, 9, "%Y%m%d", tmPtr );
  
  return atoi(cad);
}

/** Destructor de la classe
  */
CachePacs::~CachePacs()
{

}

};
