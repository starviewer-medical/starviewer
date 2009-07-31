#ifndef UDGSHORTCUTMANAGER_H
#define UDGSHORTCUTMANAGER_H

#include <QList>
#include <QKeySequence>
#include "shortcuts.h"

namespace udg {

class ShortcutManager
{
public:

    ShortcutManager();

    static QList<QKeySequence> getShortcuts( const QString &key );
};

}
#endif // UDGSHORTCUTMANAGER_H
