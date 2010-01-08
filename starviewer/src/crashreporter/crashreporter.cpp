/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "../main/applicationtranslationsloader.h"
#include "../core/starviewerapplication.h"
#include "../core/coresettings.h"
#include "qcrashreporter.h"
#include "logging.h"

/* Programa principal per executar l'aplicació de crash report */

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

    translationsLoader.loadTranslation(":/crashreporter_" + defaultLocale.name());
}

int main( int argc, char* argv[] )
{
    QApplication app( argc, argv );

    if ( app.arguments().size() != 3 )
        return false;
    
    configureLogging();

    app.setOrganizationName( udg::OrganizationNameString );
    app.setOrganizationDomain( udg::OrganizationDomainString );
    app.setApplicationName( udg::ApplicationNameString );

    udg::CoreSettings coreSettings;
    coreSettings.init();

    initializeTranslations(app);
    
    udg::QCrashReporter reporter( app.arguments() );
    reporter.show();

    return app.exec();
}
