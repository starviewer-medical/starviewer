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

#ifndef UDGAPPLICATIONTRANSLATIONSLOADER_H
#define UDGAPPLICATIONTRANSLATIONSLOADER_H

#include <QLocale>

class QApplication;

namespace udg {

/**
    Classe encarregada de carregar les diferents traduccions d'una aplicació.
    De la part d'internacionalització de l'aplicació s'encarrega de carregar les traduccions. També té paràmetres
    per saber quina és la traducció correcta que s'ha d'aplicar.
  */
class ApplicationTranslationsLoader {
public:
    /// Constructor de la classe. Cal passar-li l'aplicació a la que volem que es carreguin les traduccions.
    ApplicationTranslationsLoader(QApplication *application);
    ~ApplicationTranslationsLoader();

    /// Retorna el Local de l'aplicació. Aquest pot haver estat escollit per l'usuari o, si no, és el del sistema.
    QLocale getDefaultLocale();

    /// Carrega una traducció de Qt (*.qm) que es trobi a translationFilePath. Aquest path pot ser d'un resource.
    /// Retorna true si s'ha carregat correctament, altrament, false.
    bool loadTranslation(QString translationFilePath, const QString &directory = QString());

private:
    QApplication *m_application;
};

} // End namespace

#endif
