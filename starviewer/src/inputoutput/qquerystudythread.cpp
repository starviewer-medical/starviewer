/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qquerystudythread.h"
#include "pacsserver.h"
#include "querypacs.h"
#include "status.h"
#include "logging.h"

namespace udg {

QQueryStudyThread::QQueryStudyThread(QObject *parent)
        : QThread(parent)
{

}

void QQueryStudyThread::queryStudy( PacsParameters param , DicomMask mask )
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
    PacsServer serverSCP(m_param);

    INFO_LOG( infoLogInitialitzedThread().toAscii().constData() );

    state = serverSCP.connect( PacsServer::query,PacsServer::studyLevel );

    if ( !state.good() )
    {
        ERROR_LOG( qPrintable( QString("Error al connectar al PACS %1. PACS ERROR: %2").arg( m_param.getAEPacs() ).arg( state.text().c_str() ) ) );
        emit( errorConnectingPacs( m_param.getPacsID() ) );
        exit(1);
    }
    else
    {
        //creem l'objecte fer la query
        QueryPacs queryPacsStudy;

        queryPacsStudy.setConnection( serverSCP.getConnection() );
        //busquem els estudis
        state = queryPacsStudy.query( m_mask );

        if (! state.good() )
        {
            ERROR_LOG( qPrintable( QString("Error al connectar al PACS %1. PACS ERROR: %2").arg( m_param.getAEPacs() ).arg( state.text().c_str() ) ) );
            emit( errorQueringStudiesPacs( m_param.getPacsID() ) );
        }

        INFO_LOG ( qPrintable( QString("Thread del PACS %1 finalitzant").arg( m_param.getAEPacs() ) ) );

        //desconnectem
        serverSCP.disconnect();
        exit( 0 );
    }
}

QString QQueryStudyThread::infoLogInitialitzedThread()
{
    QString missatgeLog, pacsLog;

    missatgeLog = "thread iniciat per cercar al PACS ";

    pacsLog.insert( 0 , m_param.getAELocal() );
    pacsLog.append( ";" );
    pacsLog.append( m_param.getAEPacs() );
    pacsLog.append( ";" );
    pacsLog.append( m_param.getPacsAdr() );
    pacsLog.append( ";" );
    pacsLog.append( m_param.getPacsPort() );
    pacsLog.append( ";" );

    missatgeLog += pacsLog;

    return missatgeLog;
}

QQueryStudyThread::~QQueryStudyThread()
{
}

}
