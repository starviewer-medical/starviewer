/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

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
    UpgradeDatabaseRevisionCommands getUpgradeDatabaseRevisionCommands(int fromDatabaseRevision) const;

    /// Indica quina ha de ser la revisió mínima de la base de dades per poder-se actualitzar
    int getMinimumDatabaseRevisionRequiredToUpgrade() const;

private:

    /// Parseja el UpgradeDatabaseTag i ens retorna UpgradeDatabaseRevisionCommands amb les comandes que s'han d'aplicar per actualitzar la revisió actual de la BD
    UpgradeDatabaseRevisionCommands parseUpgradeDatabaseTag(QXmlStreamReader *reader, int fromDatabaseRevision) const;

    /// Parseja tag UpgradeDatabaseRevision del XML
    QStringList parseUpgradeDatabaseToRevisionChildrenTags(QXmlStreamReader *reader) const;

    /// Transforma el QMap amb els resultats obtinguts de parserjar el XML
    UpgradeDatabaseRevisionCommands fromQMapToUpgradeDatabaseRevisionCommands(QMap<int, QStringList> m_sqlUpgradeCommandsGroupedByDatabaseRevision) const;

    QXmlStreamReader* getXmlReader() const;
    QString m_xmlData;
};

}

#endif // UPGRADEDATABASEXMLPARSER_H
