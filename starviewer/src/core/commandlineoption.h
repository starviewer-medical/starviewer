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
