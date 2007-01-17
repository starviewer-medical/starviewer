/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                 *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qlogviewer.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QTextCodec>
#include <QFileDialog>
#include "logging.h"

namespace udg {

QLogViewer::QLogViewer(QWidget *parent)
 : QDialog(parent)
{
    setupUi( this );
//     // carreguem l'arxiu de log
//     updateData();
    createConnections();
}

QLogViewer::~QLogViewer()
{
}

void QLogViewer::updateData()
{
    // \TODO aquest directori s'hauria de guardar en alguna mena de qsettings o similar
    QFile logFile( QDir::homePath() + "/.starviewer/log/starviewer.log" );
    if ( !logFile.open( QFile::ReadOnly | QFile::Text) )
    {
        ERROR_LOG( "No s'ha pogut obrir l'arxiu de logs" );
        m_logBrowser->setPlainText( tr("ERROR: No Log file found!") );
    }
    else
    {
        INFO_LOG( "S'ha obert amb èxit l'arxiu de logs" );
        m_logBrowser->setReadOnly( true );
        m_logBrowser->setPlainText( logFile.readAll() );
    }
}

void QLogViewer::createConnections()
{
    connect( m_closeButton , SIGNAL( clicked() ) , this , SLOT( close() ) );
    connect( m_saveButton , SIGNAL( clicked() ) , this , SLOT( saveLogFileAs() ) );
}

void QLogViewer::saveLogFileAs()
{
    QString fileName = QFileDialog::getSaveFileName( this, tr("Save as..."),                                             QString(), tr("Log Files (*.log)") );

    if ( fileName.isEmpty() )
        return;

    QFile file( fileName );
    if ( !file.open( QFile::WriteOnly ) )
        return;

    QTextStream logStream( &file );
    logStream << m_logBrowser->document()->toPlainText();
}

}
