/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "../main/applicationtranslationsloader.h"
#include "../core/starviewerapplication.h"
#include "qcrashreporter.h"

/* Programa principal per executar l'aplicació de crash report */

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
    
    app.setOrganizationName( udg::OrganizationNameString );
    app.setOrganizationDomain( udg::OrganizationDomainString );
    app.setApplicationName( udg::ApplicationNameString );

    initializeTranslations(app);
    
    udg::QCrashReporter reporter( app.arguments() );
    reporter.show();

    return app.exec();
}
