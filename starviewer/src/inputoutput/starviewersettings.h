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
class StarviewerSettings{
public:
    StarviewerSettings();

    ~StarviewerSettings();

    //Cache
      void setDatabasePath(QString);
      void setPoolSize(QString );
      void setCacheImagePath(QString);
      
      QString getDatabasePath();
      QString getPoolSize();
      QString getCacheImagePath();
    
    //Pacs
      void setAETitleMachine(QString);
      void setTimeout(QString);
      void setLocalPort(QString);
      void setLanguage(QString);
      void setMaxConnections(QString);
      void setPrevImages(bool);
      void setCountImages(bool);
      
      QString getAETitleMachine();
      QString getTimeout();
      QString getLocalPort();
      QString getLanguage();
      QString getMaxConnections();
      bool    getPrevImages();
      bool    getCountImages();
      
private :

    QSettings m_starviewerSettings;
    
};

};

#endif
