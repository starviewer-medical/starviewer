/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "applicationtranslationsloader.h"

#include <QSettings>
#include <QApplication>
#include <QTranslator>

#include "starviewerapplication.h"

namespace udg {

ApplicationTranslationsLoader::ApplicationTranslationsLoader(QApplication *application)
{
    m_application = application;
}

ApplicationTranslationsLoader::~ApplicationTranslationsLoader()
{
}

QLocale ApplicationTranslationsLoader::getDefaultLocale()
{
    QSettings settings;
    settings.beginGroup("Starviewer-Language");
    QString defaultLocale = settings.value( "languageLocale", QLocale::system().name() ).toString();
    settings.endGroup();

	// configurem les locales de l'aplicació
	// TODO ara està simplificat només als idiomes que nosaltres
	// suportem. Mirar si es pot millorar i fer més genèric
	QLocale::Language language;
	QLocale::Country country;

	if( defaultLocale.startsWith("en_") )
		language = QLocale::English;
	else if( defaultLocale.startsWith("es_") )
		language = QLocale::Spanish;
	else if( defaultLocale.startsWith("ca_") )
		language = QLocale::Catalan;
	else
		language = QLocale::C; // TODO no hauria de ser anglès per defecte?

	if( defaultLocale.endsWith("_GB") )
		country = QLocale::UnitedKingdom;
	else if( defaultLocale.endsWith("_ES") )
		country = QLocale::Spain;
	else
		country = QLocale::AnyCountry; // // TODO no hauria de ser EEUU/UK per defecte?

	return QLocale(language, country);
}

bool ApplicationTranslationsLoader::loadTranslation(QString translationFilePath)
{
    QTranslator *translator = new QTranslator(m_application);
    if (translator->load( translationFilePath ))
    {
        m_application->installTranslator( translator );
        return true;
    }
    else
    {
        return false;
    }
}

} // end namespace
