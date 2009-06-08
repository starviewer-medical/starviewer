#include "settings.h"

#include <QSettings>

namespace udg{

Settings::Settings()
{
}

Settings::~Settings()
{
}

QVariant Settings::read( const QString &key, const QVariant &defaultValue ) const
{
    QSettings settings;

    return settings.value(key, defaultValue);
}

void Settings::write( const QString &key, const QVariant &value )
{
    QSettings settings;

    settings.setValue(key, value);
}

bool Settings::contains( const QString &key ) const
{
    QSettings settings;

    return settings.contains(key);
}

void Settings::remove( const QString &key )
{
    QSettings settings;

    settings.remove(key);
}

}  // end namespace udg
