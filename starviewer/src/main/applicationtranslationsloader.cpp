/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

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

bool ApplicationTranslationsLoader::loadTranslation(QString translationFilePath, const QString &directory)
{
    QTranslator *translator = new QTranslator(m_application);
    if (translator->load(translationFilePath, directory))
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
