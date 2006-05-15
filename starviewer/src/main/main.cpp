/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qapplicationmainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
//#include <qtextcodec.h>
#include "logging.h"
  
int main(int argc, char *argv[])
{ 
    QApplication app(argc, argv);
    
    app.setOrganizationName("GGG");
    app.setOrganizationDomain("ima.udg.es");
    app.setApplicationName("Starviewer");
     
    QString logFile = qApp->applicationDirPath() + "/log4cxx.properties";
    LOGGER_INIT( logFile.toStdString() )
    // translation

    QSettings settings("GGG", "StarViewer-Core");
    settings.beginGroup("StarViewer-Language");
    QString m_defaultLocale = settings.value( "languageLocale", "interface_" + QLocale::system().name() ).toString();
    settings.endGroup();
    
    QString qmPath = qApp->applicationDirPath() + "/../src/interface";
    QTranslator m_applicationTranslator;
    m_applicationTranslator.load( m_defaultLocale , qmPath );
    app.installTranslator( &m_applicationTranslator );
    
    QSplashScreen *splash = new QSplashScreen( QPixmap(":/images/splash.png") );
    splash->show();
    
    udg::QApplicationMainWindow *mainWin = new udg::QApplicationMainWindow;
    INFO_LOG("Creada finestra principal")
    mainWin->show();
    
    QObject::connect( &app, SIGNAL( lastWindowClosed() ),
                      &app, SLOT( quit() )); 
    splash->finish( mainWin );
    delete splash;
    
    return app.exec();
}
