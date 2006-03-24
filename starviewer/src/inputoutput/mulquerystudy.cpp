/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "multquerystudy.h"
#include <QLineEdit>
#include <semaphore.h>
#include "starviewersettings.h"
#include <qmessagebox.h>

namespace udg {

// Per raons d'optimització nomes es podran tenir un límit threads alhora executant la query, per aconseguir això utilitzem un semàfor
sem_t *activeThreads;

/** Constructor de la Classe
  */
MultQueryStudy::MultQueryStudy(QObject *parent)
 : QObject(parent)
{
    StarviewerSettings settings;
    activeThreads = (sem_t*)malloc(sizeof(sem_t));
    sem_init(activeThreads,0,settings.getMaxConnections().toInt(NULL,10));
    m_studyListSingleton=StudyListSingleton::getStudyListSingleton();
}

/** Ens permet indicar quina màscara utilitzarem per fer la query als PACS
  *                @ param StudyMask [in]  Màscara del estudis a cercar
  */
void MultQueryStudy::setMask(StudyMask mask)
{
    m_searchMask = mask;
}

/** Estableix la llista de PACS als quals es farà la cerca
  *         @param PacsList amb els pacs als quals es cercarà
  */
void MultQueryStudy::setPacsList(PacsList list)
{
     m_pacsList = list;
}

void MultQueryStudy::threadFinished()
{
    sem_post(activeThreads);
}

/** Una vegada haguem especificat la màscara, i tots els PACS als que volem realitzar la query, aquesta acció iniciara el procés de cerca a tots els PACS
  */
Status MultQueryStudy::StartQueries()
{
    QQueryStudyThread m_thread[20];
    int i = 0,j = 0;
    bool error = false;
    Status state;
    PacsParameters p;
    QLineEdit prova;
    
    m_studyListSingleton->clear();
    
    m_pacsList.firstPacs();
           
    while (!m_pacsList.end()) //Anem creant threads per cercar
    {
        //aquest signal ha de ser QDirectConnection, pq sera el propi thread qui executara l'slot d'alliberar un recurs del semafor, si fos queued, hauria de ser el pare qui respongues al signal, pero com estaria fent el sem_wait no respondria mai! i tindríem deadlock
        connect(&m_thread[i],SIGNAL(finished()),this,SLOT(threadFinished()),Qt::DirectConnection);
        sem_wait(activeThreads);//Demanem recurs, hi ha un maxim de threads limitat
        p = m_pacsList.getPacs();
        cout<<p.getAELocal()<<endl;
        cout<<p.getAEPacs()<<endl;
        cout<<p.getPacsAdr()<<endl;
        cout<<p.getPacsPort()<<endl;
        m_thread[i].queryStudy(m_pacsList.getPacs(),m_searchMask);

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
    if (!error)
    {
        state.setStatus(CORRECT);
    }
    
    return state;
}

/** retorna un apuntador a la llist amb els estudis
  *                @return  Llista amb els estudis trobats que complien amb la màscara.
  */
StudyListSingleton * MultQueryStudy::getStudyList()
{
    m_studyListSingleton->firstStudy();
    return m_studyListSingleton;
}

MultQueryStudy::~MultQueryStudy()
{
}


}
