/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "starviewersettings.h"
#include <stream.h>
#include "constkey.h"
#include <QDir>
#include <QApplication>

namespace udg {

/** Constructor de la casse
*/
StarviewerSettings::StarviewerSettings()
{

    m_starviewerSettings.beginGroup("/Starviewer/");

}

/** estableix el path de la base de dades
  *        @param path de la base de dades local
  */
void StarviewerSettings::setDatabasePath(QString path)
{
    m_starviewerSettings.setValue(databaseRootKey,path);
}

/** estableix el path on es guarden les imatges de la cache
  *        @param path de la cache
  */
void StarviewerSettings::setCacheImagePath(QString path)
{
    m_starviewerSettings.setValue(cacheImagePathKey,path);
}

/** estableix la mida de la pool
  *        @param mida de la pool
  */
void StarviewerSettings::setPoolSize(QString size)
{
    m_starviewerSettings.setValue(poolSizeKey,size);
}

/** retorna el path de la base de dades
  *        @return retorna el path de la base de dades
  */
QString StarviewerSettings::getDatabasePath()
{
    QDir currentDir;
    QString defaultDir;
    
    //construim directori per defecte
    defaultDir = QApplication::applicationDirPath(); //directori actual
    defaultDir.append("/pacscache/database/dicom.sdb");
    
    return m_starviewerSettings.value(databaseRootKey,defaultDir).toString();
}

/** retorna la mida de la pool
  *        @return retorna la mida de la pool
  */
QString StarviewerSettings::getPoolSize()
{
    return m_starviewerSettings.value(poolSizeKey,"30").toString();
}

/** retorna el Path on es guarden les imatges
  *         @return retorn el path de la cache
  */
QString StarviewerSettings::getCacheImagePath()
{
    QString defaultDir;
    
    //construim directori per defecte
    defaultDir = QApplication::applicationDirPath(); //directori actual
    defaultDir.append("/pacscache/dicom/");
    
    return m_starviewerSettings.value(cacheImagePathKey,defaultDir).toString();
}


/************************ CONFIGURACIO PACS************************************************/

/** guarda el AETitle de la màquina
  *         @param AETitle de la màquina
  */
void StarviewerSettings::setAETitleMachine(QString AETitle)
{
    m_starviewerSettings.setValue(AETitleMachineKey,AETitle);
}

/** estableix el Time out
  *         @param Time out
  */
void StarviewerSettings::setTimeout(QString time)
{
    m_starviewerSettings.setValue(timeoutPacsKey,time);
}

/** guarda el port Local pel qual rebrem les imatges descarregades
  *        @param port local per la descarrega d'imatges
  */
void StarviewerSettings::setLocalPort(QString port)
{
    m_starviewerSettings.setValue(localPortKey,port);
}

/** estableix l'idioma de l'aplicació
  *        @param idioma de l'aplicació
  */
void StarviewerSettings::setLanguage(QString lang)
{
    m_starviewerSettings.setValue(selectLanguageKey,lang);
}

/**  Nombre màxim de connexions simultànies al PACS
  *        @param nombre màxim de connexions
  */
void StarviewerSettings::setMaxConnections(QString maxConn)
{
    m_starviewerSettings.setValue(maxConnectionsKey,maxConn);
}

/** Activa o desactiva la opció de previsualització d'imatges
  *        @param boolea indicant si s'ha de previsualitzar les imatges
  */
void StarviewerSettings::setPrevImages(bool prev)
{
    m_starviewerSettings.setValue(prevImagesKey,prev);
}

/** Activa o desactiva la opció de comptar imatges en el PACS
  *        @param boolea indicant si s'han de comptar les imatges del PACS
  */
void StarviewerSettings::setCountImages(bool count)
{
    m_starviewerSettings.setValue(countImagesKey,count);
}

/** retorna el AEtitle de la màquina
  *        @return AETitle de la màquina
  */
QString StarviewerSettings::getAETitleMachine()
{
    return m_starviewerSettings.value( AETitleMachineKey,"PACS").toString();
}
/** retorna el time out de l'aplicacio
  *        @retrun timeout
  */
QString StarviewerSettings::getTimeout()
{
    return m_starviewerSettings.value(timeoutPacsKey,"20000").toString();
}

/** retorna el port pel qual esperem rebre les imatges descarregades
  *        @return port Local
  */
QString StarviewerSettings::getLocalPort()
{
    return m_starviewerSettings.value( localPortKey,"104").toString();
}

/** retorna l'idioma seleccionat per l'usuari
  *        @return idioma seleccionat
  */
QString StarviewerSettings::getLanguage()
{
    return m_starviewerSettings.value(selectLanguageKey,"104").toString();
}

/** retorna el nombre màxim de connexions simultànies que es poden tenir atacant a un PACS
  *        @return nombre màxim de connexions
  */
QString StarviewerSettings::getMaxConnections()
{
    return m_starviewerSettings.value( maxConnectionsKey,"3").toString();
}

/** retorna l'indicador de si s'han de previsualitzar les imatges del PACS
  *        @return indica si s'han de previsualitzar les imatges
  */
bool StarviewerSettings::getPrevImages()
{
    return m_starviewerSettings.value(prevImagesKey,false).toBool();
}

/** retorna l'indicador de si s'han de comptar les imatges
  *        @return indica si s'han de compter les imatges
  */
bool StarviewerSettings::getCountImages()
{
    return m_starviewerSettings.value(countImagesKey,false).toBool();
}

/************************ INTERFICIE ************************************************/
/** guarda la mida de la columna que se li passa per paràmetre del QStudyListView, encarregat de mostrar les dades del Pacs
  *        @param número de columna
  *        @param amplada de la columna
  */
void StarviewerSettings::setStudyPacsListColumnWidth(int col,int width)
{
    QString key,strCol;
    
    strCol.setNum(col,10);
    key.insert(0,pacsColumnWidthKey);
    key.append(strCol);
    
    m_starviewerSettings.setValue(key,width);
}

/** guarda la mida de la columna que se li passa per paràmetre del QStudyListView, encarregat de mostrar les dades de la cache
  *        @param número de columna
  *        @param amplada de la columna
  */
void StarviewerSettings::setStudyCacheListColumnWidth(int col,int width)
{
    QString key,strCol;
    
    strCol.setNum(col,10);
    key.insert(0,cacheColumnWidthKey);
    key.append(strCol);
    
    m_starviewerSettings.setValue(key,width);
}

/** retorna l'amplada del número de columna de la llista d'estudis del PACS, passat per paràmetre
  *        @return amplada de la columna
  */
int StarviewerSettings::getStudyPacsListColumnWidth(int col)
{   
    QString key,strCol;
    
    strCol.setNum(col,10);
    key.insert(0,pacsColumnWidthKey);
    key.append(strCol);
    
    return m_starviewerSettings.value(key,100).toInt();
}

/** retorna l'amplada del número de columna de la llista d'estudis de la cache, passat per paràmetre
  *        @return amplada de la columna
  */
int StarviewerSettings::getStudyCacheListColumnWidth(int col)
{   
    QString key,strCol;
    
    strCol.setNum(col,10);
    key.insert(0,cacheColumnWidthKey);
    key.append(strCol);
    
    return m_starviewerSettings.value(key,100).toInt();
}

/** destructor de la classe
  */
StarviewerSettings::~StarviewerSettings()
{

}

};
