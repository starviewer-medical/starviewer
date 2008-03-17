/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "multiplequerystudy.h"

#include <QSemaphore>
#include <QString>

#include "status.h"
#include "starviewersettings.h"
#include "pacsparameters.h"
#include "qquerystudythread.h"
#include "studylistsingleton.h"

#include "const.h"

namespace udg {

MultipleQueryStudy::MultipleQueryStudy( QObject *parent )
 : QObject( parent )
{
    StarviewerSettings settings;

    m_semaphoreActiveThreads = new QSemaphore( settings.getMaxConnections().toInt( NULL , 10 ) );
    m_studyListSingleton = StudyListSingleton::getStudyListSingleton();
}

void MultipleQueryStudy::setMask( DicomMask mask )
{
    m_searchMask = mask;
}

void MultipleQueryStudy::setPacsList( PacsList list )
{
     m_pacsList = list;
}

void MultipleQueryStudy::threadFinished()
{
    m_semaphoreActiveThreads->release();
}

void MultipleQueryStudy::slotErrorConnectingPacs( int pacsID )
{
    emit ( errorConnectingPacs ( pacsID ) );
}

void MultipleQueryStudy::slotErrorQueringStudiesPacs( int pacsID )
{
    emit ( errorQueringStudiesPacs ( pacsID ) );
}


Status MultipleQueryStudy::StartQueries()
{
    QList<QQueryStudyThread *> llistaThreads;
    bool error = false;
    Status state;
    PacsParameters pacsParameters;
    QString missatgeLog;

    m_studyListSingleton->clear();

    m_pacsList.firstPacs();

    while ( !m_pacsList.end() ) //Anem creant threads per cercar
    {
        QQueryStudyThread *thread = new QQueryStudyThread;

        //aquest signal ha de ser QDirectConnection, pq sera el propi thread qui executara l'slot d'alliberar un recurs del semafor, si fos queued, hauria de ser el pare qui respongues al signal, pero com estaria fent el sem_wait no respondria mai! i tindríem deadlock
        
        connect( thread , SIGNAL( finished() ) , this , SLOT( threadFinished() ) , Qt::DirectConnection );
        connect( thread , SIGNAL( errorConnectingPacs( int ) ) , this , SLOT ( slotErrorConnectingPacs( int  ) ) );
        connect( thread , SIGNAL( errorQueringStudiesPacs( int ) ) , this , SLOT ( slotErrorQueringStudiesPacs( int  ) ) );
        m_semaphoreActiveThreads->acquire();//Demanem recurs, hi ha un maxim de threads limitat
        pacsParameters = m_pacsList.getPacs();

        thread->queryStudy( m_pacsList.getPacs() , m_searchMask );

        llistaThreads.append( thread );
        m_pacsList.nextPacs();
    }

    foreach ( QQueryStudyThread *thread , llistaThreads )
    {
        thread->wait();
        delete thread;
    }

    //si no hi ha error retornem l'status ok
    if ( !error )
    {
        state.setStatus( CORRECT );
    }

    return state;
}

StudyListSingleton * MultipleQueryStudy::getStudyList()
{
    m_studyListSingleton->firstStudy();
    return m_studyListSingleton;
}

MultipleQueryStudy::~MultipleQueryStudy()
{
}

}
