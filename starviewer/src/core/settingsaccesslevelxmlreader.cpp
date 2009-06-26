#include "settingsaccesslevelxmlreader.h"

#if QT_VERSION >= 0x040300

#include "logging.h"

namespace udg {

SettingsAccessLevelXMLReader::SettingsAccessLevelXMLReader()
{
}

SettingsAccessLevelXMLReader::~SettingsAccessLevelXMLReader()
{
}

bool SettingsAccessLevelXMLReader::read(QIODevice *device)
{
    m_accessLevelTable.clear();
    setDevice(device);
    while( !atEnd() ) 
    {
        readNext();
        if( isStartElement() )
        {
            if( name() == "settingsAccessLevel" && attributes().value("version") == "1.0" )
            {
                readSettingsFile();
            }
            else
            {
                //raiseError("L'arxiu ["L'arxiu no és un arxiu de 'settingsAccessLevel' versió 1.0");
                DEBUG_LOG("L'arxiu no és un arxiu de 'settingsAccessLevel' versió 1.0");
                WARN_LOG("L'arxiu no és un arxiu de 'settingsAccessLevel' versió 1.0");
            }
        }
    }
    
    if( hasError() ) 
    {
        QString message = QString("[Line: %1, Column:%2] Error a Settings Access Level.\n%3, error: %4").arg( lineNumber() ).arg( columnNumber() ).arg( errorString()).arg( error() );
        DEBUG_LOG( message );
        ERROR_LOG( message );
    }
    
    return !error();
}

QMap<QString, Settings::AccessLevel> SettingsAccessLevelXMLReader::getAccessLevelTable() const
{
    return m_accessLevelTable;
}

void SettingsAccessLevelXMLReader::readSettingsFile()
{
    Q_ASSERT( isStartElement() && name() == "settingsAccessLevel" );

     while( !atEnd() ) 
     {
         readNext();
         if( isEndElement() )
             break;
         
         if( isStartElement() ) 
         {
             if( name() == "section" )
                 readSection();
             else
                 readUnknownElement();
         }
     }
}

void SettingsAccessLevelXMLReader::readSection( QString sectionName )
{
    Q_ASSERT( isStartElement() && name() == "section" );
     
    if( !attributes().value("name").toString().isEmpty() )
        sectionName += attributes().value("name").toString() + "/";
    
    while( !atEnd() ) 
    {
        //TODO check de que tingui aquest atribut?
        readNext();
        if ( isEndElement() )
            break;

        if( isStartElement() ) 
        {
            if( name() == "section" )
                readSection( sectionName );
            else if ( name() == "key" )
                readKey( sectionName );
            else
                readUnknownElement();
        }
    }
}

void SettingsAccessLevelXMLReader::readKey( const QString &sectionName )
{
    Q_ASSERT( isStartElement() && name() == "key" );

    QString key = sectionName + attributes().value("name").toString();

    while( !atEnd() )
    {
        readNext();

        if( isEndElement() )
            break;

        if( isStartElement() ) 
        {
            if( name() == "accessLevel" )
                readAccessLevel(key);
            else
                readUnknownElement();
        }
    }
}

void SettingsAccessLevelXMLReader::readAccessLevel( const QString &key )
{
    Q_ASSERT( isStartElement() && name() == "accessLevel" );
    
    bool ok = true;
    Settings::AccessLevel accessLevel;

    QString value = readElementText();
    if( value == "user" )
        accessLevel = Settings::UserLevel;
    else if( value == "system" )
        accessLevel = Settings::SystemLevel;
    else
    {
        DEBUG_LOG( "Valor inesperat al tag <accessLevel>: " + value );
        WARN_LOG( "Valor inesperat al tag <accessLevel>: " + value );
        ok = false;
    }

    if( ok )
    {
        DEBUG_LOG( ">>>>>>>>>>>>>Inserim la clau: " + key + " amb valor: " + value );
        m_accessLevelTable.insert( key, accessLevel );
    }
}

void SettingsAccessLevelXMLReader::readUnknownElement()
{
    Q_ASSERT( isStartElement() );

    while( !atEnd() ) 
    {
        readNext();
        if( isEndElement() )
            break;

        if( isStartElement() )
            readUnknownElement();
    }
}

} // end namespace udg

#endif

