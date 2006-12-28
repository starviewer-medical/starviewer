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
#include "logging.h"

namespace udg {

QQueryStudyThread::QQueryStudyThread(QObject *parent)
        : QThread(parent)
{

}

void QQueryStudyThread::queryStudy( PacsParameters param , StudyMask mask )
{
    m_param = param;
    m_mask = mask;

    start();
}

void QQueryStudyThread::run()
{
    Status state;
    QString missatgeLog;
    //creem la connexió
    PacsServer server(m_param);

    INFO_LOG( infoLogInitialitzedThread().toAscii().constData() );

    state = server.connect( PacsServer::query,PacsServer::studyLevel );
    
    if ( !state.good() )
    {
        missatgeLog = "Error al connectar al PACS ";
        missatgeLog.append( m_param.getAEPacs().c_str() );
        missatgeLog.append( ". PACS ERROR : " );
        missatgeLog.append( state.text().c_str() );
        
        ERROR_LOG( missatgeLog.toAscii().constData() );
        emit( errorConnectingPacs( m_param.getPacsID() ) );
        exit(1);
    }
    else
    {
        //creem l'objecte fer la query
        QueryStudy qs( server.getConnection() , m_mask ); 
        
        //busquem els estudis
        state = qs.find();
        
        if (! state.good() ) 
        {   
            missatgeLog = "Error cercant al PACS ";
            missatgeLog.append( m_param.getAEPacs().c_str() );
            missatgeLog.append( ". PACS ERROR : " );
            missatgeLog.append( state.text().c_str() );
            ERROR_LOG( missatgeLog.toAscii().constData() );
            emit( errorQueringStudiesPacs( m_param.getPacsID() ) );
        }
        
        missatgeLog = "Thread del PACS ";
        missatgeLog.append( m_param.getAEPacs().c_str() );
        missatgeLog.append( " finalitzant" );       
        INFO_LOG ( missatgeLog.toAscii().constData() );
        
        //desconnectem
        server.disconnect();
        exit( 0 );
    }
}

QString QQueryStudyThread::infoLogInitialitzedThread()
{
    QString missatgeLog, pacsLog;
 
    missatgeLog = "thread iniciat per cercar al PACS ";
    
    pacsLog.insert( 0 , m_param.getAELocal().c_str() );
    pacsLog.append( ";" );
    pacsLog.append( m_param.getAEPacs().c_str() );
    pacsLog.append( ";" );
    pacsLog.append( m_param.getPacsAdr().c_str() );
    pacsLog.append( ";" );
    pacsLog.append( m_param.getPacsPort().c_str() );
    pacsLog.append( ";" );
    
    missatgeLog += pacsLog;
    
    return missatgeLog;
}

QQueryStudyThread::~QQueryStudyThread()
{
}

}
