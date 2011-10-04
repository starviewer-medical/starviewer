#ifndef UPGRADEDATABASEXMLPARSER_H
#define UPGRADEDATABASEXMLPARSER_H

#include <QMap>
#include <QString>

class QXmlStreamReader;

namespace udg {

/**
  Aquesta classe parserja el xml amb els diferents comandes sql que s'han d'aplicar a la base de dades per actualitzar-se a la última versió.
  */

class UpgradeDatabaseRevisionCommands;

class UpgradeDatabaseXMLParser
{
public:

    UpgradeDatabaseXMLParser(const QString &xmlData);

    /// Retorna les comandes per actualitzar la base de dades des de la revisió passada per paràmetre
    UpgradeDatabaseRevisionCommands getUpgradeDatabaseRevisionCommands(int fromDatabaseRevision);

    /// Indica quina ha de ser la revisió mínima de la base de dades per poder-se actualitzar
    int getMinimumDatabaseRevisionRequiredToUpgrade();

private:

    /// Parseja el UpgradeDatabaseTag i ens retorna UpgradeDatabaseRevisionCommands amb les comandes que s'han d'aplicar per actualitzar la revisió actual de la BD
    UpgradeDatabaseRevisionCommands parseUpgradeDatabaseTag(QXmlStreamReader *reader, int fromDatabaseRevision);

    /// Parseja tag UpgradeDatabaseRevision del XML
    QStringList parseUpgradeDatabaseToRevisionChildrenTags(QXmlStreamReader *reader) const;

    /// Transforma el QMap amb els resultats obtinguts de parserjar el XML
    UpgradeDatabaseRevisionCommands fromQMapToUpgradeDatabaseRevisionCommands(QMap<int, QStringList> m_sqlUpgradeCommandsGroupedByDatabaseRevision);

    QXmlStreamReader* getXmlReader();
    QString m_xmlData;
};

}

#endif // UPGRADEDATABASEXMLPARSER_H
