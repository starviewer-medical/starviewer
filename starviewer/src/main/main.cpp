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
//#include <qtextcodec.h>

int main(int argc, char *argv[])
{ 
    QApplication app(argc, argv);
        
    app.setOrganizationName("GGG");
    app.setOrganizationDomain("ima.udg.es");
    app.setApplicationName("Starviewer"); 
    // translation
    
    QString qmPath = qApp->applicationDirPath() + "/../src/interface";
    QTranslator m_applicationTranslator;
    m_applicationTranslator.load( "interface_ca_ES"/* + m_locales[ languageID ]*/, qmPath );
    app.installTranslator( &m_applicationTranslator );
    
    QSplashScreen *splash = new QSplashScreen( QPixmap(":/images/splash.png") );
    splash->show();
    
    udg::QApplicationMainWindow *mainWin = new udg::QApplicationMainWindow;

    mainWin->show();
    
    QObject::connect( &app, SIGNAL( lastWindowClosed() ),
                      &app, SLOT( quit() )); 
    splash->finish( mainWin );
    delete splash;
    
    return app.exec();
}
