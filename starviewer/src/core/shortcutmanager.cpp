#include "shortcutmanager.h"
#include "settings.h"

#include <QStringList>

namespace udg {

ShortcutManager::ShortcutManager()
{
}

QList<QKeySequence> ShortcutManager::getShortcuts( const QString &key )
{
    Settings settings;
    QList<QKeySequence> shortcuts;
    foreach( QString string, settings.getValue( key ).toStringList() )
    {
        shortcuts << QKeySequence(string);
    }

    return shortcuts;

}

}
