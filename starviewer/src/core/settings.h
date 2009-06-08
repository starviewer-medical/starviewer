/**
    Interfície per manipular settings
*/

#ifndef UDGSETTINGS_H
#define UDGSETTINGS_H

#include <QVariant>
// Forward declarations
class QString;

namespace udg {

class Settings
{
public:
    Settings();
    ~Settings();

    /// Retorna el valor per la clau demanada. Si el setting no existeix, retorna defaultValue.
    /// Si no s'especifica cap valor per defecte, es retorna una default QVariant.
    QVariant read( const QString &key, const QVariant &defaultValue = QVariant() ) const;

    /// Assigna el valor al setting amb la clau indicada. Si la clau ja existeix, els valor anterior queda sobre-escrit.
    void write( const QString &key, const QVariant &value );

    /// Retorna cert si existeix un setting anomenat key; returna fals altrament.
    bool contains( const QString &key ) const;

    /// Elimina els settings de key i totes les sub-settings que hi hagi per sota de la clau
    void remove( const QString &key );
};

} // end namespace udg

#endif

