#include "upgradedatabasexmlparser.h"

#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamAttribute>
#include <QXmlStreamAttributes>
#include <QScopedPointer>

#include "upgradedatabaserevisioncommands.h"
#include "logging.h"

namespace udg {

UpgradeDatabaseXMLParser::UpgradeDatabaseXMLParser(const QString &xmlData)
{
    m_xmlData = xmlData;
}

int UpgradeDatabaseXMLParser::getMinimumDatabaseRevisionRequiredToUpgrade()
{
    QScopedPointer<QXmlStreamReader> reader(getXmlReader());

    if (reader.isNull())
    {
        ERROR_LOG("XML d'actualització de la base de dades buit");
        return -1;
    }
    if (!reader->readNextStartElement())
    {
        ERROR_LOG("XML d'actualitzacio de base de dades no valid");
        return -1;
    }
    else if (reader->name().toString() != "upgradeDatabase")
    {
        ERROR_LOG("S'esperava tag upgradeDatabase i s'ha trobat" + reader->name().toString());
        return -1;
    }

    return reader->attributes().value("minimumDatabaseRevisionRequired").toString().toInt();
}

UpgradeDatabaseRevisionCommands UpgradeDatabaseXMLParser::getUpgradeDatabaseRevisionCommands(int fromDatabaseRevision)
{
    QScopedPointer<QXmlStreamReader> reader(getXmlReader());

    if (reader.isNull())
    {
        return UpgradeDatabaseRevisionCommands();
    }

    if (!reader->readNextStartElement())
    {
        ERROR_LOG("XML d'actualitzacio de base de dades no valid");
        return UpgradeDatabaseRevisionCommands();
    }
    else if (reader->name().toString() != "upgradeDatabase")
    {
        ERROR_LOG("S'esperava tag upgradeDatabase i s'ha trobat" + reader->name().toString());
        return UpgradeDatabaseRevisionCommands();
    }

    return parseUpgradeDatabaseTag(reader.data(), fromDatabaseRevision);
}

UpgradeDatabaseRevisionCommands UpgradeDatabaseXMLParser::parseUpgradeDatabaseTag(QXmlStreamReader *reader, int fromDatabaseRevision)
{
    UpgradeDatabaseRevisionCommands upgradeDatabaseRevisionCommands;
    QStringList sqlUpgradeCommands;

    while (!reader->atEnd())
    {
        if (reader->isStartElement() && reader->name() == "upgradeDatabaseToRevision")
        {
             int upgradeToDatabaseRevision = reader->attributes().value("updateToRevision").toString().toInt();

            if (upgradeToDatabaseRevision > fromDatabaseRevision)
            {
                upgradeDatabaseRevisionCommands.setUpgradeToDatabaseRevision(upgradeToDatabaseRevision);

                reader->readNextStartElement();
                sqlUpgradeCommands.append(parseUpgradeDatabaseToRevisionChildrenTags(reader));
            }
            else
            {
                //Aquesta canvis ja estant aplicats, saltem als següents
                reader->skipCurrentElement();
            }
        }
        else
        {
            reader->readNext();
        }
    }

    upgradeDatabaseRevisionCommands.setSqlUpgradeCommands(sqlUpgradeCommands);

    return upgradeDatabaseRevisionCommands;
}

QStringList UpgradeDatabaseXMLParser::parseUpgradeDatabaseToRevisionChildrenTags(QXmlStreamReader *reader) const
{
    QStringList sqlUpgradeCommands;

    while (!reader->atEnd() && reader->name() == "upgradeCommand")
    {
        if (reader->isStartElement())
        {
           sqlUpgradeCommands.append(reader->readElementText());
        }

        reader->readNextStartElement();
    }

    return sqlUpgradeCommands;
}

QXmlStreamReader* UpgradeDatabaseXMLParser::getXmlReader()
{
    return new QXmlStreamReader(m_xmlData);
}

}
