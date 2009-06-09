#include "settings.h"

#include <QSettings>
#include <QTreeWidget>
#include <QHeaderView> // pel restoreColumnsWidths

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

void Settings::saveColumnsWidths( const QString &key, QTreeWidget *treeWidget )
{
    Q_ASSERT( treeWidget );

    Settings settings;
    int columnCount = treeWidget->columnCount();
    QString columnKey;
    for( int column = 0; column < columnCount; column++ )
    {   
        columnKey = key + "/columnWidth" + QString::number(column);
        settings.write( columnKey, treeWidget->columnWidth(column) );
    }
}

void Settings::restoreColumnsWidths( const QString &key, QTreeWidget *treeWidget )
{
    Q_ASSERT( treeWidget );

    Settings settings;
    int columnCount = treeWidget->columnCount();
    QString columnKey;
    for( int column = 0; column < columnCount; column++ )
    {   
        columnKey = key + "/columnWidth" + QString::number(column);
        if( !settings.contains( columnKey ) )
            treeWidget->resizeColumnToContents(column);
        else
            treeWidget->header()->resizeSection( column, settings.read( columnKey ).toInt() );
    }
}

}  // end namespace udg
