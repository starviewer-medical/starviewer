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
#include <QDir>
#include <QTextCodec>
#include "logging.h"
#include "extensions.h"

void configureLogging()
{
    // \TODO donem per fet que l'arxiu es diu així i es troba a la localització que indiquem. S'hauria de fer una mica més flexible o genèric; està així perquè de moment volem anar per feina i no entretenir-nos però s'ha de fer bé.
    QString configurationFile = "/etc/starviewer/log.conf";
    QDir logFile( QDir::homePath() + "/.starviewer/log" );

    if (!logFile.exists())
    {
        logFile.mkpath( QDir::homePath() + "/.starviewer/log" );
    }
    LOGGER_INIT( configurationFile.toStdString() );
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName("GGG");
    app.setOrganizationDomain("trueta.udg.cat");
    app.setApplicationName("Starviewer");

    // ajustem el codec per els strings pelats ( no QString,sinó "bla bla bla" ). Amb aquesta crida escollirà el codec més apropiat segons el sistema. En aquest cas ens agafarà utf-8 (Mandriva 2007)
    QTextCodec::setCodecForCStrings( QTextCodec::codecForLocale() );

    configureLogging();
    // translation
    QSettings settings;
    settings.beginGroup("Starviewer-Language");
    QString m_defaultLocale = settings.value( "languageLocale", "interface_" + QLocale::system().name() ).toString();
    settings.endGroup();

    QTranslator m_applicationTranslator;
    m_applicationTranslator.load( QString(":/translations/") + m_defaultLocale );
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
