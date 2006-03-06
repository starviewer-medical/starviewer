/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGQRETRIEVESCREEN_H
#define UDGQRETRIEVESCREEN_H

#include <qretrievescreenbase.h>
#include <qlistview.h>
#include <qstring.h>
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
class QRetrieveScreen : public QRetrieveScreenBase
{
Q_OBJECT
public:

    void clearList();
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
    
public slots :
    
    void imageRetrieved(Image *,int downloadedImages);
    void setSeriesRetrieved(QString studyUID);

signals:
    void studyRetrieved(QString);
    
    
private:

    QRetrieveScreen(QWidget *parent = 0, const char *name =0);
    ~QRetrieveScreen();
    void deleteStudy(QString studyUID);
    int image;
    sem_t *semafor;
};

};

#endif

