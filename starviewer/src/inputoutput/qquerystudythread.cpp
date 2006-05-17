/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qquerystudythread.h"
#include "pacsserver.h"
#include "querystudy.h"
#include "status.h"


namespace udg {

/** Constructor de la classe
  */
QQueryStudyThread::QQueryStudyThread(QObject *parent)
        : QThread(parent)
{

}

/** Accio que executa el thread
  *         @param parametres del pacs a consultar
  *         @param màscara de cerca
  */
void QQueryStudyThread::queryStudy(PacsParameters param,StudyMask mask)
{
    m_param = param;
    m_mask = mask;

    start();

}

/** el codi d'aquest mètode es el que s'executa en un nou thread
  */
void QQueryStudyThread::run()
{
    Status state;
    
    //creem la connexió
    PacsServer server(m_param);

    state = server.Connect(PacsServer::query,PacsServer::studyLevel);
    if (!state.good())
    {
        qDebug("peto\n");
        emit( errorConnectingPacs( m_param.getPacsID() ) );
        exit(1);
    }
    else
    {
        //creem l'objecte fer la query
        QueryStudy qs(server.getConnection(),m_mask); 
        
        //busquem els estudis
        state = qs.find();
        
        if (!state.good()) 
        {   
            server.Disconnect();
            qDebug("peto2\n");
        }
        
        //desconnectem
        server.Disconnect();
        exit(0);
    }
     
}

QQueryStudyThread::~QQueryStudyThread()
{
}


}
