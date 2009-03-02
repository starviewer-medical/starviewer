/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
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
    bool loadTranslation(QString translationFilePath);

private:
    QApplication *m_application;
};

} // end namespace

#endif