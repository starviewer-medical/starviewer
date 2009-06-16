#ifndef UDGSETTINGSREGISTRY_H
#define UDGSETTINGSREGISTRY_H

#include "singleton.h"

namespace udg {

class SettingsRegistry : public Singleton<SettingsRegistry>
{
public:
    /// Afegeix un setting al registre. Li donem la clau i valor que t� per defecte
    void addSetting( const QString &key, const QVariant &defaultValue );

protected:
    /// Cal declarar-ho friend perqu� sin� haur�em de fer p�blics 
    /// el constructor i destructor i trencar�em aix� la filosofia d'un Singleton
    friend class Singleton<SettingsRegistry>;
    SettingsRegistry();
    ~SettingsRegistry();

};

} // end namespace udg 

#endif
