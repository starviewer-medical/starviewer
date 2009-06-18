/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "multiplequerystudy.h"

#include <QSemaphore>
#include <QString>
#include <QMessageBox>
#include <QApplication>

#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "status.h"
#include "pacsparameters.h"
#include "qquerystudythread.h"

#include "errordcmtk.h"

namespace udg {

MultipleQueryStudy::MultipleQueryStudy( QObject *parent )
 : QObject( parent )
{
    m_semaphoreActiveThreads = new QSemaphore( PacsParameters::getMaximumConnections() );
}

void MultipleQueryStudy::setMask( DicomMask mask )
{
    m_searchMask = mask;
}

void MultipleQueryStudy::setPacsList( QList<PacsParameters> pacsListToQuery )
{
     m_pacsListToQuery = pacsListToQuery;
}

void MultipleQueryStudy::threadFinished()
{
    m_semaphoreActiveThreads->release();
}

Status MultipleQueryStudy::StartQueries()
{
    QList<QQueryStudyThread *> llistaThreads;
    bool error = false;
    Status state;
    QString missatgeLog;

    initializeResultsList();

    foreach(PacsParameters pacsToQuery, m_pacsListToQuery)
    {
        QQueryStudyThread *thread = new QQueryStudyThread;

        //aquest signal ha de ser QDirectConnection, pq sera el propi thread qui executara l'slot d'alliberar un recurs del semafor, si fos queued, hauria de ser el pare qui respongues al signal, pero com estaria fent el sem_wait no respondria mai! i tindríem deadlock
        connect(thread, SIGNAL( finished() ), this, SLOT( threadFinished() ), Qt::DirectConnection);

        connect(thread, SIGNAL( errorConnectingPacs(QString) ), this, SIGNAL( errorConnectingPacs(QString) ));
        connect(thread, SIGNAL( errorQueringStudiesPacs(QString) ), this, SIGNAL( errorQueringStudiesPacs(QString) ));
        m_semaphoreActiveThreads->acquire();//Demanem recurs, hi ha un maxim de threads limitat

        thread->queryStudy( pacsToQuery , m_searchMask );

        llistaThreads.append( thread );
    }

    foreach ( QQueryStudyThread *thread , llistaThreads )
    {
        thread->wait();
        //fusionem les resultats dels diferents threads
        m_patientStudyList += thread->getPatientStudyList();
        m_seriesList += thread->getSeriesList();
        m_imageList += thread->getImageList();

        /*TODO Tenir en compte que podem tenir un studyUID repetit en dos PACS, ara mateix no ho tenim contemplat fusionem les 
               dos QHash directament */

        m_hashPacsIDOfStudyInstanceUID = m_hashPacsIDOfStudyInstanceUID.unite(thread->getHashTablePacsIDOfStudyInstanceUID());

        delete thread;
    }

    //si no hi ha error retornem l'status ok
    if ( !error )
    {
        state.setStatus( DcmtkNoError );
    }

    return state;
}

void MultipleQueryStudy::initializeResultsList()
{
    m_patientStudyList.clear();
    m_seriesList.clear();
    m_imageList.clear();
    m_hashPacsIDOfStudyInstanceUID.clear();
}

QList<Patient*> MultipleQueryStudy::getPatientStudyList()
{
    return m_patientStudyList;
}

QList<Series*> MultipleQueryStudy::getSeriesList()
{
    return m_seriesList;
}

QList<Image*> MultipleQueryStudy::getImageList()
{
    return m_imageList;
}

MultipleQueryStudy::~MultipleQueryStudy()
{
}

QHash<QString,QString> MultipleQueryStudy::getHashTablePacsIDOfStudyInstanceUID()
{
    return m_hashPacsIDOfStudyInstanceUID;
}


}
