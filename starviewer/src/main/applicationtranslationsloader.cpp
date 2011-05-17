#include "applicationtranslationsloader.h"

#include <QApplication>
#include <QTranslator>

#include "starviewerapplication.h"
#include "coresettings.h"

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
    Settings settings;
    QString defaultLocale = settings.getValue(CoreSettings::LanguageLocale).toString();

    // Configurem les locales de l'aplicació
    // TODO ara està simplificat només als idiomes que nosaltres
    // suportem. Mirar si es pot millorar i fer més genèric
    QLocale::Language language;
    QLocale::Country country;

    if (defaultLocale.startsWith("en_"))
    {
        language = QLocale::English;
    }
    else if (defaultLocale.startsWith("es_"))
    {
        language = QLocale::Spanish;
    }
    else if (defaultLocale.startsWith("ca_"))
    {
        language = QLocale::Catalan;
    }
    else
    {
        // TODO no hauria de ser anglès per defecte?
        language = QLocale::C;
    }

    if (defaultLocale.endsWith("_GB"))
    {
        country = QLocale::UnitedKingdom;
    }
    else if (defaultLocale.endsWith("_ES"))
    {
        country = QLocale::Spain;
    }
    else
    {
        // TODO no hauria de ser EEUU/UK per defecte?
        country = QLocale::AnyCountry;
    }

    return QLocale(language, country);
}

bool ApplicationTranslationsLoader::loadTranslation(QString translationFilePath)
{
    QTranslator *translator = new QTranslator(m_application);
    if (translator->load(translationFilePath))
    {
        m_application->installTranslator(translator);
        return true;
    }
    else
    {
        return false;
    }
}

} // End namespace
