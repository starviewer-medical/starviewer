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
#include "extensionmediatorfactory.h"

void configureLogging()
{
    // primer comprovem que existeixi el direcotori ~/.starviewer/log/ on guradarem els logs
    QDir logDir = QDir::homePath() + "/.starviewer/log/";
    if( !logDir.exists() )
    {
        // creem el directori
        logDir.mkpath( QDir::homePath() + "/.starviewer/log/" );
    }
    // \TODO donem per fet que l'arxiu es diu així i es troba a la localització que indiquem. S'hauria de fer una mica més flexible o genèric;
    // està així perquè de moment volem anar per feina i no entretenir-nos però s'ha de fer bé.
    QString configurationFile = "/etc/starviewer/log.conf";
    if( ! QFile::exists(configurationFile) )
    {
        configurationFile = QDir::currentPath() + "/log.conf";
    }
    if( ! QFile::exists(configurationFile) )
    {
        configurationFile = QDir::currentPath() + "/bin/log.conf";
    }
    LOGGER_INIT( configurationFile.toStdString() );
    DEBUG_LOG("Arxiu de configuració del log: " + configurationFile );
}

void loadTranslator(QApplication &app, QString pathTranslator)
{
    QTranslator *translator = new QTranslator(&app);
    if (translator->load( pathTranslator ))
    {
        app.installTranslator( translator );
    }
    else
    {
        ERROR_LOG("No s'ha pogut carregar el translator " + pathTranslator);
    }
}

void initializeTranslations(QApplication &app)
{
    QSettings settings;
    settings.beginGroup("Starviewer-Language");
    QString m_defaultLocale = settings.value( "languageLocale", QLocale::system().name() ).toString();
    settings.endGroup();

    loadTranslator(app, ":/core/core_" + m_defaultLocale);
    loadTranslator(app, ":/interface/interface_" + m_defaultLocale);
    loadTranslator(app, ":/inputoutput/inputoutput_" + m_defaultLocale);

    initExtensionsResources();
    INFO_LOG("Locales = " + m_defaultLocale);

    QList<QString> extensionsMediatorNames = udg::ExtensionMediatorFactory::instance()->getFactoryNamesList();
    foreach(QString mediatorName, extensionsMediatorNames)
    {
        udg::ExtensionMediator* mediator = udg::ExtensionMediatorFactory::instance()->create(mediatorName);

        if (mediator)
        {
            loadTranslator(app, ":/extensions/" + mediator->getExtensionID().getID() + "/translations_" + m_defaultLocale);
            delete mediator;
        }
        else
        {
            ERROR_LOG( "Error carregant el mediator de " + mediatorName );
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName("GGG");
    app.setOrganizationDomain("trueta.udg.cat");
    app.setApplicationName("Starviewer");

    // ajustem el codec per els strings pelats ( no QString,sinó "bla bla bla" ).
    // Amb aquesta crida escollirà el codec més apropiat segons el sistema. En aquest cas ens agafarà utf-8 (Mandriva 2007)
    QTextCodec::setCodecForCStrings( QTextCodec::codecForLocale() );

    configureLogging();
    initializeTranslations(app);

    QSplashScreen *splash = new QSplashScreen( QPixmap(":/images/splash.png") );
    splash->show();

    udg::QApplicationMainWindow *mainWin = new udg::QApplicationMainWindow;
    INFO_LOG("Creada finestra principal");
    mainWin->show();

    QObject::connect( &app, SIGNAL( lastWindowClosed() ),
                      &app, SLOT( quit() ));
    splash->finish( mainWin );
    delete splash;

    return app.exec();
}
