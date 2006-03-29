/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGQRETRIEVESCREEN_H
#define UDGQRETRIEVESCREEN_H


#include <QString>
#include "study.h"
#include "image.h"
#include "starviewerprocessimage.h"
#include <semaphore.h>
#include "ui_qretrievescreenbase.h"

/** Singleton
  */
  
namespace udg {

/**
@author marc
*/
class Status;
class QRetrieveScreen : public QDialog , private Ui::QRetrieveScreenBase{
Q_OBJECT
public:

    
    static QRetrieveScreen* getQRetrieveScreen()
     {
         static QRetrieveScreen QRS;
         return &QRS; 
     }
     
    void setConnectSignal(StarviewerProcessImage *);
    void delConnectSignal(StarviewerProcessImage *);
    void insertNewRetrieve(Study *);

    
    ~QRetrieveScreen();
    
    
public slots :
    
    void imageRetrieved(QString studyUID,int downloadedImages);
    void setSeriesRetrieved(QString studyUID);
    void setRetrievedFinished(QString studyUID);
    void setErrorRetrieving(QString studyUID);
    void setRetrieving( QString );
    
    void clearList();

    
private:

    QRetrieveScreen( QWidget *parent = 0 );
    
    void createConnections();
    void deleteStudy(QString studyUID);
};

};

#endif

