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

#ifndef UDGCOMMANDLINEOPTION_H
#define UDGCOMMANDLINEOPTION_H

#include <QString>

namespace udg {

/**
    Aquesta classe representa una opció de línia de comandes. Permet definir-ne un nom, si necessita argument i una descripció d'aquesta
 */
class CommandLineOption {
public:
    CommandLineOption();
    CommandLineOption(const QString &name, bool argumentIsRequired, const QString &description);
    ~CommandLineOption();
    
    /// Prefix que defineix que el paràmetre és una opció
    static const QString OptionSelectorPrefix;

    /// Retorna el nom
    QString getName() const;
    
    /// Retorna la descripció
    QString getDescription() const;

    /// Indica si requereix argument
    bool requiresArgument() const;

    /// Ens retorna la opció formatada en un string. Es mostrarà com s'hauria d'invocar la opció, amb la possibilitat d'afegir-li la descripció
    /// Per una opció sense argument seria així: -optionName    Descripció de la opció
    /// Per una opció amb argument: -optionName value   Descripció de la opció
    QString toString(bool includeDescription = false) const;

private:
    /// Nom de la opció
    QString m_name;

    /// Descripció de la opció
    QString m_description;

    /// Indica si l'opció requereix o no argument
    bool m_argumentIsRequired;
};

} // End namespace udg

#endif
