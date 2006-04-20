/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGSTARVIEWERSETTINGS_H
#define UDGSTARVIEWERSETTINGS_H

#include <qsettings.h>
#include <QString>
namespace udg {

/** Aquesta classe permet accedir i guardar els parametres de configuracio de l'starviewer
@author marc
*/

const QString databaseRootKey("pacs/cache/sdatabasePath"); //indica on es troba la bd
const QString poolSizeKey("pacs/cache/poolSize");
const QString cacheImagePathKey("pacs/cache/imagePath");
const QString AETitleMachineKey("pacs/pacsparam/AETitle");
const QString localPortKey("pacs/pacsparam/localPort");
const QString timeoutPacsKey("pacs/pacsparam/timeout");
const QString maxConnectionsKey("pacs/pacsparam/MaxConnects");
const QString selectLanguageKey("pacs/language");
const QString pacsColumnWidthKey("pacs/interfase/studyPacsList/columnWidth");//en aquesta clau a darrera s'hi concatena el número de columna, per diferenciar cada columna
const QString cacheColumnWidthKey("pacs/interface/studyCacheList/columnWidth");//en aquesta clau a darrera s'hi concatena el número de columna, per diferenciar cada columna
const QString MaximumDaysNotViewedStudy("pacs/cache/MaximumDaysNotViewedStudy");

class StarviewerSettings{
public:
    StarviewerSettings();

    ~StarviewerSettings();

    //Cache
      void setDatabasePath(QString);
      void setPoolSize(QString );
      void setCacheImagePath(QString);
      void setMaximumDaysNotViewedStudy( QString );
      
      QString getDatabasePath();
      QString getPoolSize();
      QString getCacheImagePath();
      QString getMaximumDaysNotViewedStudy();
    
    //Pacs
      void setAETitleMachine(QString);
      void setTimeout(QString);
      void setLocalPort(QString);
      void setLanguage(QString);
      void setMaxConnections(QString);
      
      QString getAETitleMachine();
      QString getTimeout();
      QString getLocalPort();
      QString getLanguage();
      QString getMaxConnections();
      
    //interficie
      void setStudyPacsListColumnWidth(int col,int width);
      void setStudyCacheListColumnWidth(int col,int width);      
      
      int getStudyPacsListColumnWidth(int column);
      int getStudyCacheListColumnWidth(int column);
      
private :

    QSettings m_starviewerSettings;
    
};

};

#endif
