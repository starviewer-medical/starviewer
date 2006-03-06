#include "mulquerystudy.h"
#include <pthread.h>
#include "const.h"   
#include <stdlib.h> 
#include <ofcond.h>


namespace udg{

//estructura necessària per passar els paràmetres a funció estàtica que executen threads
struct QueryParameters
     {
        StudyMask searchMask;
        PacsParameters pacs;
      };    
// Per raons d'optimització nomes es podran tenir un límit threads alhora executant la query, per aconseguir això utilitzem un semàfor
sem_t *activeThreads;

/** Constructor de la Classe
  *        @param Numero màxim de threads que es poden executar alhora
  */
MulQueryStudy::MulQueryStudy(int threads)
{
    m_maxThreads = threads;

    activeThreads = (sem_t*)malloc(sizeof(sem_t));
    sem_init(activeThreads,0,m_maxThreads);
    m_studyListSingleton=StudyListSingleton::getStudyListSingleton();
}

/** Ens permet indicar quina màscara utilitzarem per fer la query als PACS
  *                @ param StudyMask [in]  Màscara del estudis a cercar
  */
void MulQueryStudy::setMask(StudyMask mask)
{
    m_searchMask = mask;
}

/** Estableix la llista de PACS als quals es farà la cerca
  *         @param PacsList amb els pacs als quals es cercarà
  */
void MulQueryStudy::setPacsList(PacsList list)
{
     m_PacsList = list;
}

/** Accióprivada, utilitzada pels threads, que s'encarrega de connectar amb el PACS, i donar-li l'ordre de buscar els estudis
  *                    @param arg [in] Objecte composat d'un ParameterPacs i StudyMask
  */
void *Query(void *arg)
{

    QueryParameters param;
    Status state;
    int code = 1;
    
    //fem un castinc per obtenir els parametres
    param = *((QueryParameters *)arg);
    
    //creem la connexió
    PacsServer server(param.pacs);

    state = server.Connect(PacsServer::query,PacsServer::seriesLevel);
    
    if (!state.good())
    {
        sem_post(activeThreads);
        pthread_exit((void*)(code));
    }
    
    code = 2;
    //creem l'objecte fer la query
    QueryStudy qs(server.getConnection(),param.searchMask); 
    
    //busquem els estudis
    state = qs.find();
     
    if (!state.good()) 
    {   
        server.Disconnect();
        sem_post(activeThreads);
        pthread_exit((void*)(code));
    }
    
    //desconnectem
    server.Disconnect();
    code = 0;
    sem_post(activeThreads);//Allibarem recurs. Un altre thread pot entrar.
    pthread_exit((void*)(&code));
    
}

/** Una vegada haguem especificat la màscara, i tots els PACS als que volem realitzar la query, aquesta acció iniciara el procés de cerca a tots els PACS
  */
Status MulQueryStudy::StartQueries()
{
    int i = 0,j = 0;
    QueryParameters param;
    int code;
    bool error = false;
    Status state;
    PacsParameters p;
    
    
    m_studyListSingleton->clear();
    
    m_PacsList.firstPacs();
    
    while (!m_PacsList.end()) //Anem creant threads per cercar
    {
        sem_wait(activeThreads);//Demanem recurs, hi ha un maxim de threads limitat
        param.pacs = m_PacsList.getPacs();
        param.searchMask = m_searchMask;
        p = m_PacsList.getPacs();
        cout<<p.getAELocal()<<endl;
        cout<<p.getAEPacs()<<endl;
        cout<<p.getPacsAdr()<<endl;
        cout<<p.getPacsPort()<<endl;
        pthread_create(&m_threads[i],NULL,Query,(void*)&param); //creem thread
        m_PacsList.nextPacs();
        i++;
    }
    
    m_PacsList.firstPacs();
    for (j = 0;j < m_PacsList.size();j++)
    {//Esperem que tots els threads estiguin
        pthread_join(m_threads[j],(void **)&code);
        if (code == 2)
        {
            //L'error 1300 és el que indica que hi hagut errors al fer el find al PACS, retornem en el text de l'error el nom del pacs
            const OFConditionConst error_DQuery(0, 1300, OF_error,m_PacsList.getPacs().getAEPacs().c_str());
            const OFConditionConst error_Query (error_DQuery);
            state.setStatus(error_Query);
            error = false;
        }
        else if (code == 1)
        {
            const OFConditionConst error_DConnect(0, 1300, OF_error,m_PacsList.getPacs().getAEPacs().c_str());
            const OFConditionConst error_Connect(error_DConnect);
            state.setStatus(error_Connect);
            error = true;        
        }
        m_PacsList.nextPacs();
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
StudyListSingleton * MulQueryStudy::getStudyList()
{
    m_studyListSingleton->firstStudy();
    return m_studyListSingleton;
}

}
