/**
    Interfície per manipular settings
*/

#ifndef UDGSETTINGS_H
#define UDGSETTINGS_H

#include <QVariant>
// Forward declarations
class QString;
class QTreeWidget;

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

    /**
     * Mètodes per facilitar el guardar i/o restaurar la geometria de certs widgets
     */

    /// Guarda/Restaura els amples de columna del widget dins de la clau donada. 
    /// Sota la clau donada es guardaran els amples de cada columna amb nom columnWidthX on X serà el nombre de columna
    /// L'unica implementació de moment és per QTreeWidget (i classes que n'hereden). 
    /// Es sobrecarregarà el mètode per tants widgets com calgui.
    void saveColumnsWidths( const QString &key, QTreeWidget *treeWidget );
    void restoreColumnsWidths( const QString &key, QTreeWidget *treeWidget );
};

} // end namespace udg

#endif

