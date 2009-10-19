#ifndef UDGSETTINGSPARSER_H
#define UDGSETTINGSPARSER_H

#include "singleton.h"

#include <QMap>

namespace udg {

/**
    Classe encarregada de parsejar strings de settings que poden
    contenir un seguit de paraules clau. Només tindrem una sola instància
    ja que es tracta d'una classe Singleton.

    Les paraules clau disponibles es carreguen inicialment al crear l'objecte
    conjuntament amb els seus corresponents valors.
    
    El catàleg de paraules clau és el següent:

    %HOSTNAME%: Nom de la màquina local (aka localHostName)
    %IP%: Adreça ip de la màquina local en format aaa.bbb.ccc.dddd 
    %IP.1%: Primer prefix de l'adreça ip de la màquina local
    %IP.2%: Segon prefix de l'adreça ip de la màquina local
    %IP.3%: Tercer prefix de l'adreça ip de la màquina local
    %IP.4%: Quart prefix de l'adreça ip de la màquina local
    %USERNAME%: Nom d'usuari actual
    %HOMEPATH%: Ruta al directori "home" d'usuari

    El seu tractament serà "case sensitive".
*/
class SettingsParser : public Singleton<SettingsParser> {
public:
    /// Donada una string la parseja i ens retorna el resultat
    QString parse( const QString &stringToParse );

protected:
    /// Cal declarar-ho friend perquè sinó hauríem de fer públics 
    /// el constructor i destructor i trencaríem així la filosofia d'un Singleton
    friend class Singleton<SettingsParser>;
    SettingsParser();
    ~SettingsParser();

private:
    /// Inicialitza la taula amb les paraules clau i els seus corresponents valors
    void initializeParseableStringsTable();

private:
    /// Mapa que conté la relació de paraules clau amb el seu valor
    QMap<QString, QString> m_parseableStringsTable;

};

} // end namespace udg 

#endif
