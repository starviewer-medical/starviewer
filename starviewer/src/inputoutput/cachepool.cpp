/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "cachepool.h"
#include "status.h"
#include <qdir.h>
#include <QString>
#include <iostream.h>
#include <string>
#ifdef _WIN32
	#include <windows.h>
#else // linux stuff
	#include <sys/vfs.h>
	#include <sys/stat.h>
#endif // _WIN32
#include "starviewersettings.h"

namespace udg {

/** Constructor de la classe
  */
CachePool::CachePool()
{
   m_DBConnect = DatabaseConnection::getDatabaseConnection();
}

CachePool::~CachePool()
{
}

/**  Construeix l'estat en que ha finaltizat l'operació sol·licitada
  *            @param  [in] Estat de sqlite
  *            @return retorna l'estat de l'operació
  */
Status CachePool::constructState(int numState)
{
//A www.sqlite.org/c_interface.html hi ha al codificacio dels estats que retorna el sqlite
    Status state;

    switch(numState)
    {//aqui tractem els errors que ens poden afectar de manera més directe, i els quals l'usuari pot intentar solucionbar                         
        case SQLITE_OK :        state.setStatus("Normal",true,0);
                                break;
        case SQLITE_ERROR :     state.setStatus("Database missing",false,2001);
                                break;
        case SQLITE_CORRUPT :   state.setStatus("Database corrupted",false,2011);
                                break;
        case SQLITE_CONSTRAINT: state.setStatus("Constraint Violation",false,2019);
                                break;
      //aquests errors en principi no es poden donar, pq l'aplicació no altera cap element de l'estructura, si es produeix algun
      //Error d'aquests en principi serà perquè la bdd està corrupte o problemes interns del SQLITE, fent Numerror-2000 de l'estat
      //a la pàgina de www.sqlite.org podrem saber de quin error es tracta.
        default :               state.setStatus("SQLITE internal error",false,2000+numState); 
                                break;
    }
   return state;
}

/** retorna l'espai lliure del disc dur
  *        @return espai lliure al disc dur
  */
double CachePool::getFreeTotalSpace()
{
    double fFree;
#ifdef _WIN32
//codi per Windows
	QString sCurDir = QDir::current().absPath();
	
	ULARGE_INTEGER free,total;
	bool bRes = ::GetDiskFreeSpaceExA( 0 , &free , &total , NULL );
	if ( !bRes ) return false;

	QDir::setCurrent( sCurDir );

	fFree = static_cast<double>( static_cast<__int64>(free.QuadPart)  );// / fKB;
// 	fTotal = static_cast<double>( static_cast<__int64>(total.QuadPart));// / fKB;

#else 
//codi per linux 
        StarviewerSettings settings;
        QString sDirPath = QDir::current().absolutePath();
	
        struct stat stst;
	struct statfs stfs;
        double Mb = 1024*1024;

        // \TODO Saber i aclarir què fan les dues línies següents
	if ( ::stat(sDirPath.toLocal8Bit(),&stst) == -1 ) return false;
	if ( ::statfs(sDirPath.toLocal8Bit(),&stfs) == -1 ) return false;

	fFree = stfs.f_bavail * ( stst.st_blksize );
// 	fTotal = stfs.f_blocks * ( stst.st_blksize );
                fFree = fFree / Mb;

#endif // _WIN32
	
        return fFree;
}

/** Esborra un estudi de l'spool de l'aplicació
  *         @param path absolut de l'estudi
  */
void CachePool::removeStudy(std::string absPathStudy)
{
    QStringList seriesDirList;
    QDir studyDir,seriesDir;
    QString absSeriesPath;
    
    studyDir.setPath( absPathStudy.c_str() );
    seriesDirList =  studyDir.entryList();
        
    //busquem els directori de totes les series que conté l'estudi
    for ( QStringList::Iterator it = seriesDirList.begin(); it != seriesDirList.end(); ++it ) 
    {
        if (*it != "." && *it != "..")
        {
            absSeriesPath.truncate(0);
            absSeriesPath.append( absPathStudy.c_str() );          
            absSeriesPath.append(*it);
            absSeriesPath.append("/");
            removeSeries( absSeriesPath.toStdString() );
            seriesDir.rmdir( absSeriesPath ); //esborra el directori de la sèrie
        }
    }
    studyDir.rmdir( absPathStudy.c_str() ); //esborra el directori de l'estudi
    
}

/** Esborra una serie de l'spool
  *        @param path absolut de la sèrie
  */
void CachePool::removeSeries(std::string absPathSeries)
{
    QStringList imageFilesList;
    QDir seriesDir,imageFile;
    QString absPathImage;
    
    seriesDir.setPath( absPathSeries.c_str() );
    imageFilesList =  seriesDir.entryList();
        
    for ( QStringList::Iterator it = imageFilesList.begin(); it != imageFilesList.end(); ++it ) 
    {
        if (*it != "." && *it != "..")
        {
            absPathImage.truncate(0);
            absPathImage.append( absPathSeries.c_str() );
            absPathImage.append("/");
            absPathImage.append(*it);
            imageFile.remove( absPathImage );
        }           
    }
    
}

/** Esborra una imatge del disc dur
  *         @param Path de la imatge a esborrar
  *         @return estat del mètode
  */
bool CachePool::removeImage(std::string absPath)
{
    QDir file;
    
    return file.remove( absPath.c_str() );
        
}

/** Esborra un directori
  *         @param Path del directori
  *         @return estat del mètode
  */
bool CachePool::removeDir(std::string absDirPath)
{
    QDir dir;
    
    return dir.rmdir( absDirPath.c_str() );
}

//AQUESTA FUNCIO NO S'UTILITZA, JA QUE SEMPRE QUE ACTUALITZEM L'ESPAI ES QUANT INSERIM O ESBORREM UN ESTUDI I AQUESTES ACCIONS
//S'HAN DE FER AMB UN TRANSACCIO SINO ENS PODRIEM TROBAR QUE INSERISSIM UNA IMATGE L'APLICACIO ES TANQUES I NO S'HAGUES ACTUALITAT L'ESPAI OCUPAT
//SI TOT S'ENGLOBA DINS UNA TRANSACCIO NO HI HAURA AQUEST PROBLEMA
 
/** Actualitza l'espai utilitzat de caché, amb la quantitat de bytes passats per paràmetre
  *         @param mida a actualitzar
  *         @return retorna estat del mètode
  */
Status CachePool::updatePoolSpace(int size)
{

    int i;
    Status state;
    std::string sql;
    
    if (!m_DBConnect->connected())
    {//el 50 es l'error de no connectat a la base de dades
        return constructState(50);
    }
    
    sql.insert(0,"Update Pool Set Space = Space + %i ");
    sql.append("where Param = 'USED'");
    
    m_DBConnect->getLock();
    i = sqlite_exec_printf(m_DBConnect->getConnection(),sql.c_str(),0,0,0
                                ,size);
    m_DBConnect->releaseLock();
                                
    state = constructState(i);
    return state;
}

/** actualitza l'espai màxim disponible per a la caché
  *        @param [in] Espai en Mb que pot ocupar la caché
  *        @return estat el mètode
  */
Status CachePool::updatePoolTotalSize(int space)
{
    int i;
    Status state;
    std::string sql;
    char size[7];
    
    //sqlite no permet en un update entra valors mes gran que un int, a través de la interfície c++ com guardem la mida en bytes fem
    //un string i hi afegim 6 zeros per passar Mb a bytes
    
    if (!m_DBConnect->connected())
    {//el 50 es l'error de no connectat a la base de dades
        return constructState(50);
    }
    
    sprintf(size,"%i",space); //convertim l'espai en Mb a string
    sql.insert(0,"Update Pool Set Space = ");//convertim l'espai en bytes
    sql.append(size);
    sql.append("000000");
    sql.append(" where Param = 'POOLSIZE'");
    
    m_DBConnect->getLock();
    i = sqlite_exec_printf(m_DBConnect->getConnection(),sql.c_str(),0,0,0,space);
    m_DBConnect->releaseLock();
                                
    state = constructState(i);

    return state;
}

/** actualitza l'espai utiltizat de la cache a 0 bytes
  *        @return estat el mètode
  */
Status CachePool::resetPoolSpace()
{
    int i;
    Status state;
    std::string sql;
    
    
    if (!m_DBConnect->connected())
    {//el 50 es l'error de no connectat a la base de dades
        return constructState(50);
    }
    
    sql.insert(0,"Update Pool Set Space = 0 ");
    sql.append("where Param = 'USED'");
    
    m_DBConnect->getLock();
    i = sqlite_exec_printf(m_DBConnect->getConnection(),sql.c_str(),0,0,0);
    m_DBConnect->releaseLock();
                                
    state = constructState(i);

    return state;
}

/** Retorna l'espai que estem utilitzan de l'spool en Mb
  *         @param space [in/out] Espai ocupat del Pool (la caché) actualment en Mb
  *         @return estat el mètode
  */
Status CachePool::getPoolUsedSpace(int &space)
{
    Status state;
    std::string sql;
    char **resposta = NULL,**error = NULL;
    int col,rows,i;
    
    if (!m_DBConnect->connected())
    {//el 50 es l'error de no connectat a la base de dades
        return constructState(50);
    }    
    
    sql.insert(0,"select round(Space/1000000) from Pool ");
    sql.append("where Param = 'USED'");
    
    m_DBConnect->getLock();
    i = sqlite_get_table(m_DBConnect->getConnection(),sql.c_str(),&resposta,&rows,&col,error);
    m_DBConnect->releaseLock();
                                
    state = constructState(i);

    if (!state.good()) return state;
    
    i = 1;//ignorem les capçaleres
   
    space = atoi(resposta[i]);
    
    return state;
}

/** Retorna l'espai màxim que pot ocupar el Pool(Tamany de la caché)
  *         @param space [in/out] Espai assignat en Mb que pot ocupar el Pool (la caché)
  *         @return estat el mètode
  */
Status CachePool::getPoolTotalSize(int &space)
{
    Status state;
    std::string sql;
    char **resposta = NULL,**error = NULL;
    int col,rows,i;
    
    if (!m_DBConnect->connected())
    {//el 50 es l'error de no connectat a la base de dades
        return constructState(50);
    }
    
    sql.insert(0,"select round(Space/1000000) from Pool "); // dividim per 1.000.000 per obtenir les dades en Mb
    sql.append("where Param = 'POOLSIZE'");
    
    m_DBConnect->getLock();
    i = sqlite_get_table(m_DBConnect->getConnection(),sql.c_str(),&resposta,&rows,&col,error);
    m_DBConnect->releaseLock();
                                
    state = constructState(i);

    if (!state.good()) return state;
    
    i=1;//ignorem les capçaleres
   
    space = atoi(resposta[i]);
    
    return state;
}

};
