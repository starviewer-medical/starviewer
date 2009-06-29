#include "settings.h"

#include "logging.h"
#include "starviewerapplication.h"
#include "settingsregistry.h"

#include <QTreeWidget>
#include <QHeaderView> // pel restoreColumnsWidths
#include <QSplitter> // pels saveGeometry(),restoreGeometry() de QSplitter

namespace udg{

Settings::Settings()
{
    QSettings *userSettings = new QSettings( QSettings::UserScope, OrganizationNameString, ApplicationNameString );
    QSettings *systemSettings = new QSettings( QSettings::SystemScope, OrganizationNameString, ApplicationNameString );

    m_settings.insert( UserLevel, userSettings );
    m_settings.insert( SystemLevel, systemSettings );
}

Settings::~Settings()
{
}

QVariant Settings::getValue( const QString &key ) const
{
    QVariant value;
    // Primer mirem si tenim valor als settings
    // Si estigués buit, llavors agafem el valor per defecte que tinguem al registre
    // TODO hauríem d'obtenir l'objecte de settings amb getSettingsObject(key) 
    // però cal resoldre abans un problema de linkatge produit per projectes externs (crashreporter/sapwrapper)
    value = m_settings.value( SettingsRegistry::instance()->getAccessLevel(key) )->value(key);
    if( value == QVariant() )
    {
        value = SettingsRegistry::instance()->getDefaultValue(key);
    }

    return value;
}

void Settings::setValue( const QString &key, const QVariant &value )
{
    getSettingsObject(key)->setValue(key, value);
}

bool Settings::contains( const QString &key ) const
{
    // TODO hauríem d'obtenir l'objecte de settings amb getSettingsObject(key) 
    // però cal resoldre abans un problema de linkatge produit per projectes externs (crashreporter/sapwrapper)
    return m_settings.value( SettingsRegistry::instance()->getAccessLevel(key) )->contains(key);
}

void Settings::remove( const QString &key )
{
    getSettingsObject(key)->remove(key);
}

Settings::KeyValueMapType Settings::getListItem( const QString &key, int index )
{
    KeyValueMapType item;
    QSettings *qsettings = getSettingsObject(key);

    int size = qsettings->beginReadArray(key);
    if( index < size && index >= 0 )
    {
        qsettings->setArrayIndex(index);
        // Omplim el conjunt de claus-valor a partir de les claus de l'índex de la llista
        item = fillKeyValueMapFromKeyList( qsettings->allKeys(), qsettings );
    }
    else
    {
        DEBUG_LOG("Índex fora de rang. L'element retornat serà buit");
    }
    qsettings->endArray();

    return item;
}

Settings::SettingListType Settings::getList( const QString &key )
{
    SettingListType list;
    QSettings *qsettings = getSettingsObject(key);

    int size = qsettings->beginReadArray(key);
    for(int i = 0; i < size; ++i)
    {
        qsettings->setArrayIndex(i);

        KeyValueMapType item;
        // Omplim el conjunt de claus-valor a partir de les claus de l'índex de la llista
        item = fillKeyValueMapFromKeyList( qsettings->allKeys(), qsettings );
        // Afegim el nou conjunts de valors a la llista
        list << item;
    }
    qsettings->endArray();

    return list;
}

void Settings::addListItem( const QString &key, const KeyValueMapType &item )
{
    QSettings *qsettings = getSettingsObject(key);

    int arraySize = qsettings->beginReadArray(key);
    qsettings->endArray();
    setListItem( arraySize, key, item );
}

void Settings::setListItem( int index, const QString &key, const KeyValueMapType &item )
{
    QSettings *qsettings = getSettingsObject(key);
    // no comprobem si l'índex està dins d'un rang determinat
    // farem servir la política que tingui QSettings::setArrayIndex()
    qsettings->beginWriteArray(key); 
    qsettings->setArrayIndex(index);
    // omplim
    dumpKeyValueMap(item,qsettings);
    qsettings->endArray();
}

void Settings::removeListItem( const QString &key, int index )
{
    // ara mateix simplement el que fa és posar-li la clau adequada
    // TODO mirar si és necessari fer alguna comprovació més o si cal "re-ordenar" 
    // la llista, és a dir, si elimino l'element 3 de 5, potser cal renombrar l'element
    // 4 a "3" i el 5 a "4"    
    QSettings *qsettings = getSettingsObject(key);
    qsettings->remove( key + "/" + QString::number(index+1) );
}

void Settings::setList( const QString &key, const SettingListType &list )
{
    QSettings *qsettings = getSettingsObject(key);
    // eliminem tot el que pogués haver d'aquella llista anteriorment
    remove(key);
    // escrivim la llista
    qsettings->beginWriteArray(key);
    foreach( KeyValueMapType item, list )
    {
        dumpKeyValueMap( item, qsettings );
    }
    qsettings->endArray();
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

Settings::KeyValueMapType Settings::fillKeyValueMapFromKeyList( const QStringList &keysList, QSettings *qsettings )
{
    Q_ASSERT( qsettings );

    KeyValueMapType item;
    
    foreach( QString key, keysList )
    {
        item[ key ] = qsettings->value( key );
    }
    return item;
}

void Settings::dumpKeyValueMap( const KeyValueMapType &item, QSettings *qsettings )
{
    Q_ASSERT( qsettings );

    QStringList keysList = item.keys();
    foreach( QString key, keysList )
    {
        qsettings->setValue( key, item.value(key) );
    }   
}

QSettings *Settings::getSettingsObject( const QString &key )
{
    return m_settings.value( SettingsRegistry::instance()->getAccessLevel(key) );
}

}  // end namespace udg
