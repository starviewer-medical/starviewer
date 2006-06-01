/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include <QLineEdit>
#include <semaphore.h>

#include "status.h"
#include "multiplequerystudy.h"
#include "starviewersettings.h"
#include "pacsparameters.h"
#include "qquerystudythread.h"

namespace udg {

// Per raons d'optimització nomes es podran tenir un límit threads alhora executant la query, per aconseguir això utilitzem un semàfor
sem_t *activeThreads;

MultipleQueryStudy::MultipleQueryStudy( QObject *parent )
 : QObject( parent )
{
    StarviewerSettings settings;
    activeThreads = ( sem_t* )malloc( sizeof( sem_t ) );
    sem_init( activeThreads , 0 , settings.getMaxConnections().toInt( NULL , 10 ) );
    m_studyListSingleton = StudyListSingleton::getStudyListSingleton();
}

void MultipleQueryStudy::setMask( StudyMask mask )
{
    m_searchMask = mask;
}

void MultipleQueryStudy::setPacsList( PacsList list )
{
     m_pacsList = list;
}

void MultipleQueryStudy::threadFinished()
{
    sem_post( activeThreads );
}

void MultipleQueryStudy::slotErrorConnectingPacs( int pacsID )
{
    emit ( errorConnectingPacs ( pacsID ) );
}

Status MultipleQueryStudy::StartQueries()
{
    QQueryStudyThread m_thread[ 20 ];
    int i = 0,j = 0;
    bool error = false;
    Status state;
    PacsParameters pacsParameters;
    QLineEdit prova;
    QString missatgeLog;
    
    m_studyListSingleton->clear();
    
    m_pacsList.firstPacs();
           
    while ( !m_pacsList.end() ) //Anem creant threads per cercar
    {
        //aquest signal ha de ser QDirectConnection, pq sera el propi thread qui executara l'slot d'alliberar un recurs del semafor, si fos queued, hauria de ser el pare qui respongues al signal, pero com estaria fent el sem_wait no respondria mai! i tindríem deadlock
        connect( &m_thread[i] , SIGNAL( finished() ) , this , SLOT( threadFinished() ) , Qt::DirectConnection );
        connect( &m_thread[i] , SIGNAL( errorConnectingPacs( int ) ) , this , SLOT ( slotErrorConnectingPacs( int  ) ) );
        sem_wait(activeThreads);//Demanem recurs, hi ha un maxim de threads limitat
        pacsParameters = m_pacsList.getPacs();

        m_thread[i].queryStudy( m_pacsList.getPacs() , m_searchMask );

        m_pacsList.nextPacs();
        i++;
    }
    
    m_pacsList.firstPacs();
    for (j = 0;j < m_pacsList.size();j++)
    {//Esperem que tots els threads estiguin
        m_thread[j].wait();
        //m_thread[j]->delete();
        m_pacsList.nextPacs();
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
