/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGAPPLICATIONCOMMANDLINEOPTIONS_H
#define UDGAPPLICATIONCOMMANDLINEOPTIONS_H

#include <QHash>
#include <QStringList>

class QString;

namespace udg {

/** Classe que ens parseja els arguments que ens entren per línia de comandes al executar l'aplicació. 
    Els arguments de comandes de línia es composen de dos parts, la primera part s'anomena l'opció que indica quina comanda 
    i la segona part s'anomena argument que indica amb quin valor s'ha d'executar la comanda, per exemple per -accessionnumber 1234 
    accessionnumber és l'opció i 1234 l'argument.
    
    Podem especificar opcions sense necessitar que continguin un argument tipus exeppre
        -version

    Podem especificar opcions amb un argument, si conté més d'un argument s'ignorarà i s'agafarà el primer 
        -accessionnumber 1234

    Aquesta classe és casesensitivie
    */

class ApplicationCommandLineOptions {

public:

    ///Permet afegir un argument que acceptarem com a vàlid per la comanda de línies. Retorna fals si ja existeix l'argument 
    bool addOption(QString optionName, QString description);

    /**Especifica la llista d'arguments a parsejar, si hi ha alguna opció de més que no s'ha definit o alguna opció té
       més d'un valor es retorna fals indicant que no s'ha parsejat correctament l'argument */
    bool parseArgumentList(QStringList argumentList);

    ///Retorna la llista d'arguments
    QStringList getArgumentList();

    ///Retorna cert si ens han especificat aquella opció al la línia d'arguments
    bool isSet(QString optionName);

    /*Retorna el valor que han especificat per aquella opció
     *   Si optionName no ha estat definit com a una opció vàlida retorna NULL
     *   Si han especificat l'opció però no li han donat cap argument retorna buit
     */
    QString getOptionArgument(QString optionName);

    ///Retorna l'error de parsejar una una llista d'arguments
    QString getParserErrorMessage();

    ///Retorna el número d'opcions que s'han trobat correctes al parserjar
    int getNumberOfParsedOptions();

    ///Retorna un string on mostra cada una de les opcions vàlides reconegudes pel parser i una descripció de la funcionalitat de cada opció
    QString getOptionsDescription();

private:

    ///Conté caràcter que defineix si el paràmetre passat per línia de comandes és un option o un value 
    static const QString optionSelectorCharacter;

    //Conté les opcions possibles que ens poden entrar des de la línia de comandes, i conté associada la descripció del que fa aquella opció
    QHash<QString, QString> m_commandLineOptions;
    //Conté les opcions parsejades entrades per comandes de línia amb el seu valor
    QHash<QString, QString> m_parsedOptions;
    //argumens de la línia de comandes a persajar
    QStringList m_argumentList;
    //Conté un text d'error si al parsejar la línia de comandes trobem opcions no vàlides
    QString m_parserErrorMessage;

    ///parseja els paràmetres en funció de les opcions que s'han especificat
    bool parse();

    ///Indica si l'string és una opció, és una opció si és una comanda amb format -versions
    bool isAnOption(QString command);

    ///Indica si l'string és un argument d'una opció
    bool isAnArgument(QString argument);
};

}

#endif
