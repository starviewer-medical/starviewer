#include "settings.h"

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
    return m_settings.value(key, defaultValue);
}

void Settings::write( const QString &key, const QVariant &value )
{
    m_settings.setValue(key, value);
}

bool Settings::contains( const QString &key ) const
{
    return m_settings.contains(key);
}

void Settings::remove( const QString &key )
{
    m_settings.remove(key);
}

void Settings::saveColumnsWidths( const QString &key, QTreeWidget *treeWidget )
{
    Q_ASSERT( treeWidget );

    int columnCount = treeWidget->columnCount();
    QString columnKey;
    for( int column = 0; column < columnCount; column++ )
    {   
        columnKey = key + "/columnWidth" + QString::number(column);
        this->write( columnKey, treeWidget->columnWidth(column) );
    }
}

void Settings::restoreColumnsWidths( const QString &key, QTreeWidget *treeWidget )
{
    Q_ASSERT( treeWidget );

    int columnCount = treeWidget->columnCount();
    QString columnKey;
    for( int column = 0; column < columnCount; column++ )
    {   
        columnKey = key + "/columnWidth" + QString::number(column);
        if( !this->contains( columnKey ) )
            treeWidget->resizeColumnToContents(column);
        else
            treeWidget->header()->resizeSection( column, this->read( columnKey ).toInt() );
    }
}

}  // end namespace udg
