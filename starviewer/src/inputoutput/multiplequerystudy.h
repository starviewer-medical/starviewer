/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGMULTQUERYSTUDY_H
#define UDGMULTQUERYSTUDY_H

#include "status.h"
#include "pacsparameters.h"
#include "qquerystudythread.h"
#include "pacslist.h"
#include "studylistsingleton.h"
#include "pacsparameters.h"
#include "studymask.h"
#include <QObject>

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class MultipleQueryStudy : public QObject
{
Q_OBJECT
public:
    MultipleQueryStudy(QObject *parent = 0);
    
    ~MultipleQueryStudy();
   
    void setMask(StudyMask);
    void setPacsList(PacsList);
    
    Status StartQueries();
    
    StudyListSingleton* getStudyList();

signals :
    void finish();
    void errorConnectingPacs( int );

public slots :

    void threadFinished();
    void slotErrorConnectingPacs( int );

private :

    StudyMask m_searchMask;
    
    StudyListSingleton* m_studyListSingleton;
    PacsList m_pacsList;
    int m_maxThreads;
    
    void QueryStudies();

};

}

#endif
