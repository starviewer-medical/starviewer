#ifndef UDGSETTINGSREGISTRY_H
#define UDGSETTINGSREGISTRY_H

#include "singleton.h"

namespace udg {

class SettingsRegistry : public Singleton<SettingsRegistry>
{
public:
    /// Afegeix un setting al registre. Li donem la clau i valor que té per defecte
    void addSetting( const QString &key, const QVariant &defaultValue );

protected:
    /// Cal declarar-ho friend perquè sinó hauríem de fer públics 
    /// el constructor i destructor i trencaríem així la filosofia d'un Singleton
    friend Singleton<SettingsRegistry>;
    SettingsRegistry();
    ~SettingsRegistry();

};

} // end namespace udg 

#endif
