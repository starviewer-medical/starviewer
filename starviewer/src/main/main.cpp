/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qapplicationmainwindow.h"

#include "logging.h"
#include "statswatcher.h"
#include "extensions.h"
#include "extensionmediatorfactory.h"
// definicions globals d'aplicació
#include "starviewerapplication.h"
// decodificacio jpeg
#include "dcmtk/dcmjpeg/djdecode.h"
#include "applicationtranslationsloader.h"

#include "coresettings.h"
#include "inputoutputsettings.h"
#include "interfacesettings.h"
#include "shortcuts.h"
#include "starviewerapplicationcommandline.h"

#ifndef NO_CRASH_REPORTER
#include "crashhandler.h"
#endif

#include <QApplication>
#include <QSplashScreen>
#include <QLocale>
#include <QTextCodec>
#include <QDir>
#include <qtsingleapplication.h>

typedef udg::SingletonPointer<udg::StarviewerApplicationCommandLine> StarviewerSingleApplicationCommandLineSingleton;

void configureLogging()
{
    // primer comprovem que existeixi el direcotori ~/.starviewer/log/ on guradarem els logs
    QDir logDir = udg::UserLogsPath;
    if( !logDir.exists() )
    {
        // creem el directori
        logDir.mkpath( udg::UserLogsPath );
    }
    // TODO donem per fet que l'arxiu es diu així i es troba a la localització que indiquem. S'hauria de fer una mica més flexible o genèric;
    // està així perquè de moment volem anar per feina i no entretenir-nos però s'ha de fer bé.
    QString configurationFile = "/etc/starviewer/log.conf";
    if( ! QFile::exists(configurationFile) )
    {
        configurationFile = qApp->applicationDirPath() + "/log.conf";
    }

    LOGGER_INIT( configurationFile.toStdString() );
    DEBUG_LOG("Arxiu de configuració del log: " + configurationFile );
}

void initializeTranslations(QApplication &app)
{
    udg::ApplicationTranslationsLoader translationsLoader(&app);
	// li indiquem la locale corresponent
    QLocale defaultLocale = translationsLoader.getDefaultLocale();
	QLocale::setDefault( defaultLocale );

    translationsLoader.loadTranslation(":/core/core_" + defaultLocale.name());
    translationsLoader.loadTranslation(":/interface/interface_" + defaultLocale.name());
    translationsLoader.loadTranslation(":/inputoutput/inputoutput_" + defaultLocale.name());

    initExtensionsResources();
    INFO_LOG("Locales = " + defaultLocale.name());

    QList<QString> extensionsMediatorNames = udg::ExtensionMediatorFactory::instance()->getFactoryNamesList();
    foreach(QString mediatorName, extensionsMediatorNames)
    {
        udg::ExtensionMediator* mediator = udg::ExtensionMediatorFactory::instance()->create(mediatorName);

        if (mediator)
        {
            QString translationFilePath = ":/extensions/" + mediator->getExtensionID().getID() + "/translations_" + defaultLocale.name();
            if (!translationsLoader.loadTranslation(translationFilePath))
            {
                ERROR_LOG("No s'ha pogut carregar el translator " + translationFilePath);
            }
            delete mediator;
        }
        else
        {
            ERROR_LOG( "Error carregant el mediator de " + mediatorName );
        }
    }
}

/// Afegeix els directoris on s'han de buscar els plugins de Qt. Útil a windows.
void initQtPluginsDirectory()
{
#ifdef Q_OS_WIN32
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/plugins");
#endif
}

void printScreenInvalidCommandLineArguments(QString errorInvalidCommanLineArguments)
{
    qPrintable(QObject::tr("Starviewer - incorrect parameters"));
    qPrintable(errorInvalidCommanLineArguments);
}

void sendToFirstStarviewerInstanceCommandLineOptions(QtSingleApplication &app)
{
    QString errorInvalidCommanLineArguments;

    if (StarviewerSingleApplicationCommandLineSingleton::instance()->parse(app.arguments(), errorInvalidCommanLineArguments))
    {
        if (!app.sendMessage(app.arguments().join(";"), 10000))
        {
            ERROR_LOG("No s'ha pogut enviar a la instancia principal la llista d'arguments, sembla que l'instancia principal no respon.");
        }
        INFO_LOG("S'ha enviat correctament a la instancia principal els arguments de la línia de comandes.");
    }
    else 
    {
        printScreenInvalidCommandLineArguments(errorInvalidCommanLineArguments);
    }
}

int main(int argc, char *argv[])
{
    /*Utilitzem QtSingleApplication en lloc de QtApplication, ja que ens permet tenir executant sempre una sola instància d'Starviewer, si l'usuari executa
      una nova instància d'Starviewer aquesta ho detecta i envia la línia de comandes amb que l'usuari ha executat la nova instància principal.
     */
    QtSingleApplication app(argc, argv);

    app.setOrganizationName( udg::OrganizationNameString );
    app.setOrganizationDomain( udg::OrganizationDomainString );
    app.setApplicationName( udg::ApplicationNameString );

	// Inicialitzem el crash handler en el cas que ho suportem.
#ifndef NO_CRASH_REPORTER
    CrashHandler *crashHandler = new CrashHandler();
#endif
    
	//TODO tot aquest proces inicial de "setups" hauria d'anar encapsulat en
    // una classe dedicada a tal efecte

    // ajustem el codec per els strings pelats ( no QString,sinó "bla bla bla" ).
    // Amb aquesta crida escollirà el codec més apropiat segons el sistema. En aquest cas ens agafarà utf-8 (Mandriva 2007)
    QTextCodec::setCodecForCStrings( QTextCodec::codecForLocale() );

    configureLogging();
    udg::StatsWatcher::log("Inicialització de l'aplicació");

    // Inicialitzem els settings
    udg::CoreSettings coreSettings;
    udg::InputOutputSettings inputoutputSettings;
    udg::InterfaceSettings interfaceSettings;
    udg::Shortcuts shortcuts;
    
    coreSettings.init();
    inputoutputSettings.init();
    interfaceSettings.init();
    shortcuts.init();
    
    initQtPluginsDirectory();
    initializeTranslations(app);

    // TODO aixo es necessari per, entre d'altres coses, poder crear thumbnails,
    // dicomdirs, etc de dicoms comprimits i tractar-los correctament amb dcmtk
    // aixo esta temporalment aqui, a la llarga anira a una classe de setup
    // registrem els codecs decompressors JPEG
    DJDecoderRegistration::registerCodecs();

    if (app.isRunning())
    {
        //Hi ha una altra instància del Starviewer executant-se
        INFO_LOG("Hi ha una altra instancia de l'starviewer executant-se. S'enviaran els arguments de la linia de comandes a la instancia principal.");

        sendToFirstStarviewerInstanceCommandLineOptions(app);
        return 0;
    }
    else
    {
        //Instància principal, no n'hi ha cap més executant-se
        QString errorInvalidCommanLineArguments;
        
        udg::QApplicationMainWindow *mainWin = new udg::QApplicationMainWindow;
        //Fem el connect per rebre els arguments de les altres instàncies
        QObject::connect(&app, SIGNAL(messageReceived(QString)), StarviewerSingleApplicationCommandLineSingleton::instance(), SLOT(parseAndRun(QString)));

        if (app.arguments().count() > 1)
        {
            if (!StarviewerSingleApplicationCommandLineSingleton::instance()->parseAndRun(app.arguments(), errorInvalidCommanLineArguments))
            {
                printScreenInvalidCommandLineArguments(errorInvalidCommanLineArguments);
                return 0;
            }
        }

        INFO_LOG("Creada finestra principal");
        QPixmap splashPixmap;
        #ifdef STARVIEWER_LITE
        splashPixmap.load(":/images/splashLite.png");
        #else
        splashPixmap.load(":/images/splash.png");
        #endif
        QSplashScreen *splash = new QSplashScreen( splashPixmap );
        splash->show();

        mainWin->show();

        QObject::connect( &app, SIGNAL( lastWindowClosed() ),
                          &app, SLOT( quit() ));
        splash->finish( mainWin );
        delete splash;
            
        int returnValue = app.exec();

        udg::StatsWatcher::log("Es tanca l'aplicació");

        return returnValue;
    }
}
