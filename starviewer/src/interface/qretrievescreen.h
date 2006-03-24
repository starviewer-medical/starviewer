/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGQRETRIEVESCREEN_H
#define UDGQRETRIEVESCREEN_H

#include "ui_qretrievescreenbase.h"
#include <QString>
#include "study.h"
#include "image.h"
#include <starviewerprocessimage.h>
#include <semaphore.h>

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
    void setRetrievedFinished(QString studyUID);
    void setErrorRetrieving(QString studyUID);
    ~QRetrieveScreen();
    
    
public slots :
    
    void imageRetrieved(Image *,int downloadedImages);
    void setSeriesRetrieved(QString studyUID);
    void clearList();

signals:
    void studyRetrieved(QString);
    
private:

    QRetrieveScreen( QWidget *parent = 0 );
    
    void createConnections();
    void deleteStudy(QString studyUID);
};

};

#endif

