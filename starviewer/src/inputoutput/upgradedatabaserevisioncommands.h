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

#ifndef UPGRADEDATABASEREVISIONCOMMANDS_H
#define UPGRADEDATABASEREVISIONCOMMANDS_H

#include <QStringList>

namespace udg {

/**
    Classe que conté les comandes de sql que han d'aplicar a una revisió de base dades per actualitzar-se a la següent
  */
class UpgradeDatabaseRevisionCommands
{
public:
    /// Assigna/Obté a quina revisió actualitza la base de dades
    void setUpgradeToDatabaseRevision(int databaseRevision);
    int getUpgradeToDatabaseRevision() const;

    /// Assigna/Obté les comandes sql aplicar
    void setSqlUpgradeCommands(const QStringList &upgradeCommands);
    QStringList getSqlUpgradeCommands() const;

    /// Operador d'igualtat
    bool operator==(const UpgradeDatabaseRevisionCommands &upgradeDatabaseRevisionToCompare);

private:

    int m_upgradeToDatabaseRevision;
    QStringList m_sqlUpgradeCommands;
};

}
#endif // UPGRADEDATABASEREVISIONCOMMANDS_H
