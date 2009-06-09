#include "settings.h"

#include <QTreeWidget>
#include <QHeaderView> // pel restoreColumnsWidths
#include <QSplitter> // pels saveGeometry(),restoreGeometry() de QSplitter

namespace udg{

Settings::Settings()
{
}

Settings::~Settings()
{
}

QVariant Settings::getValue( const QString &key, const QVariant &defaultValue ) const
{
    return m_settings.value(key, defaultValue);
}

void Settings::setValue( const QString &key, const QVariant &value )
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
        this->setValue( columnKey, treeWidget->columnWidth(column) );
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
            treeWidget->header()->resizeSection( column, this->getValue( columnKey ).toInt() );
    }
}

void Settings::saveGeometry( const QString &key, QWidget *widget )
{
    Q_ASSERT( widget );
    this->setValue( key, widget->saveGeometry() );
}

void Settings::restoreGeometry( const QString &key, QWidget *widget )
{
    Q_ASSERT( widget );
    widget->restoreGeometry( this->getValue(key).toByteArray() );
}

void Settings::saveGeometry( const QString &key, QSplitter *splitter )
{
    Q_ASSERT( splitter );
    this->setValue( key, splitter->saveState() );
}

void Settings::restoreGeometry( const QString &key, QSplitter *splitter )
{
    Q_ASSERT( splitter );
    splitter->restoreState( this->getValue(key).toByteArray() );
}

}  // end namespace udg
