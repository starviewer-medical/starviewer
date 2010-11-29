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

    A més a més, a aquestes paraules clau se'ls pot afegir un sufix que els hi apliqui una màscara.

    == Màscara de truncatge ==
    
    Aquesta màscara el que fa és truncar a 'n' caràcters la clau %KEY%. Podem especificar adicionalment
    un caràcter de padding amb el que s'omplin el nombre de caràcters buits si la longitud de %KEY% és 
    inferior al nombre de caràcters truncats.
    
    La sintaxi és la següent
    
    %KEY%[n:c]  
    
    on:
      * %KEY%: paraula clau del catàleg (HOSTANAME,IP, etc)
      * n: nombre de caràcters a truncar de %KEY%. Ha de ser un nombre natural > 0
      * c: caràcter de padding amb el que omplirem els espais buits. 
           El caràcter de padding pot ser qualsevol caràcter, excepte l'espai en blanc

    
    Exemples:
    
    Per obtenir els 5 últims dígits de l'adreça IP i omplir els espais buits amb el caràcter 'x', escriuríem
    %IP.3%[2:x]%IP.4%[3:x]
    Si tinguèssim la adreça IP 10.80.9.2 el resultat seria "x9xx2"
    
    Per obtenir els 4 últims dígits, sense padding escriuríem
    %IP.3%[1:]%IP.4%[3:]
    i en aquest cas, per la mateixa adreça IP que l'anterior, el resultat seria "92"

    
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

    /// Ens diu si la cadena de texte passada és una adreça IPv4 vàlida.
    /// Només accepta com a resultats vàlids cadenes de texte que tinguin únicament l'adreça IP
    /// Si la cadena conté una adreça IP a més d'altres caràcters, aquesta serà donada com a invàlida.
    /// Es considera una adreça IP vàlida una cadena amb la forma xxx.xxx.xxx.xxx on xxx és un número 
    /// dins del rang 0..255. La cadena 192.168.2.1 seria considerada com a vàlida, 
    /// però la cadena 192.168.002.001 no ho seria pas.
    /// TODO Aquest mètode hauria de formar part d'una classe amb utilitats de sistema o xarxa
    /// accessible des de qualsevol altre lloc de l'aplicació per fer aquest tipu de comprovacions.
    bool isIPv4Address(const QString &ipAddress);

    /// Ens retorna una llista amb les adreces IPv4 de la màquina local.
    /// Normalment el primer element de la llista hauria de ser la IP corresponent 
    /// a la connexió de xarxa principal.
    /// TODO Aquest mètode hauria de formar part d'una classe amb utilitats de sistema o xarxa
    /// accessible des de qualsevol altre lloc de l'aplicació per fer obtenir aquest tipu d'informació.
    QStringList getLocalHostIPv4Addresses();

private:
    /// Mapa que conté la relació de paraules clau amb el seu valor
    QMap<QString, QString> m_parseableStringsTable;

};

} // end namespace udg 

#endif
