#ifndef UDGSETTINGSREGISTRY_H
#define UDGSETTINGSREGISTRY_H

#include "singleton.h"
#include "settings.h"
#include <QMap>
#include <QVariant>
#include <QPair>

namespace udg {

class SettingsRegistry : public Singleton<SettingsRegistry> {
public:
    /// Afegeix un setting al registre. Li donem la clau i valor que té per defecte
    void addSetting( const QString &key, const QVariant &defaultValue, Settings::Properties properties = Settings::None );

    /// Retorna el valor que tingui per defecte el setting amb clau "key"
    QVariant getDefaultValue( const QString &key );

    /// Obté el nivell d'accés que té una determinada clau. 
    /// Si no troba la definició per aquest clau, el valor 
    /// retornat per defecte és d'Usuari
    Settings::AccessLevel getAccessLevel( const QString &key ) const;

    /// Ens retorna les propietats de la clau 'key'
    Settings::Properties getProperties( const QString &key );

protected:
    /// Cal declarar-ho friend perquè sinó hauríem de fer públics 
    /// el constructor i destructor i trencaríem així la filosofia d'un Singleton
    friend class Singleton<SettingsRegistry>;
    SettingsRegistry();
    ~SettingsRegistry();

private:
    /// Carrega la taula de nivells d'accés. Aquesta taula només s'hauria de carregar un sol cop.
    void loadAccesLevelTable();

private:
    /// Mapa que associa la clau del setting amb el seu valor per defecte i les seves propietats.
    // TODO s'ha optat per tenir-ho tot en un sol map, però es podria considerar la opció de mantenir dos maps
    // ja que habitualment no tindrem cap propietat associada, de moment només en els casos de settings parsejables
    QMap<QString, QPair<QVariant,Settings::Properties> > m_keyDefaultValueAndPropertiesMap;

    /// Mapa en el que guardem el nivell d'accés associat a cada setting
    QMap<QString, Settings::AccessLevel> m_accessLevelTable;

};

} // end namespace udg 

#endif
