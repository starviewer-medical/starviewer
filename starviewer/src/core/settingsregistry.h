#ifndef UDGSETTINGSREGISTRY_H
#define UDGSETTINGSREGISTRY_H

#include "singleton.h"
#include <QMap>
#include <QVariant>

namespace udg {

class SettingsRegistry : public Singleton<SettingsRegistry> {
public:
    /// Afegeix un setting al registre. Li donem la clau i valor que té per defecte
    void addSetting( const QString &key, const QVariant &defaultValue );

    /// Retorna el valor que tingui per defecte el setting amb clau "key"
    QVariant getDefaultValue( const QString &key );

protected:
    /// Cal declarar-ho friend perquè sinó hauríem de fer públics 
    /// el constructor i destructor i trencaríem així la filosofia d'un Singleton
    friend class Singleton<SettingsRegistry>;
    SettingsRegistry();
    ~SettingsRegistry();

private:
    /// Mapa que associa la clau del setting amb el seu valor per defecte si en té.
    QMap<QString, QVariant> m_keyDefaultValueMap;

};

} // end namespace udg 

#endif
