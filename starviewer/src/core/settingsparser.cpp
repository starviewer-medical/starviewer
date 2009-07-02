#include "settingsparser.h"

#include <QHostInfo> // localhostname, ip
#include <QDir>
#include <QProcess> // pel systemEnvironment
#include "logging.h"

namespace udg {

// caràcter delimitador per les paraules clau
const QChar delimiterChar('%');

SettingsParser::SettingsParser()
{
    initializeParseableStringsTable();
}

SettingsParser::~SettingsParser()
{
}

QString SettingsParser::parse( const QString &stringToParse )
{
    QString parsedString = stringToParse;

    int count = stringToParse.count( delimiterChar );

    QMapIterator<QString, QString> iterator(m_parseableStringsTable);
    while( iterator.hasNext() && count )
    {
        iterator.next();
        parsedString = parsedString.replace( delimiterChar + iterator.key() + delimiterChar, iterator.value() );
        count = stringToParse.count( delimiterChar );
    }
    
    return parsedString;
}

void SettingsParser::initializeParseableStringsTable()
{
    // omplim els valors de les diferents paraules clau
    QString localHostName = QHostInfo::localHostName();
    m_parseableStringsTable["HOSTNAME"] = localHostName;
    
    // obtenció de la ip
    QString ip;
    QHostInfo info = QHostInfo::fromName( localHostName );
    QList<QHostAddress> hostAddresses = info.addresses();
    if( !hostAddresses.isEmpty() )
        ip = hostAddresses.first().toString();
    
    m_parseableStringsTable["IP"] = ip;

    // "partim" els prefixos de la ip
    QStringList ipParts = ip.split(".");
    if( ipParts.count() == 4 )
    {
        m_parseableStringsTable["IP.1"] = ipParts.at(0);
        m_parseableStringsTable["IP.2"] = ipParts.at(1);
        m_parseableStringsTable["IP.3"] = ipParts.at(2);
        m_parseableStringsTable["IP.4"] = ipParts.at(3);
    }
    else
    {
        m_parseableStringsTable["IP.1"] = "0";
        m_parseableStringsTable["IP.2"] = "0";
        m_parseableStringsTable["IP.3"] = "0";
        m_parseableStringsTable["IP.4"] = "0";
    }
    // home path
    m_parseableStringsTable["HOMEPATH"] = QDir::homePath();
    
    // nom d'usuari
    QStringList environmentList = QProcess::systemEnvironment();
    int index = environmentList.indexOf("USERNAME"); // windows
    if( index != -1 )
        m_parseableStringsTable["USERNAME"] = environmentList.at(index);
    else
    {
        index = environmentList.indexOf("USER"); // linux, mac
        if( index != -1 )
            m_parseableStringsTable["USERNAME"] = environmentList.at(index);
    }
}

} // end namespace udg
