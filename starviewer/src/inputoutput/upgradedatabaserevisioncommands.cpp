#include "upgradedatabaserevisioncommands.h"

namespace udg {

void UpgradeDatabaseRevisionCommands::setUpgradeToDatabaseRevision(int databaseRevision)
{
    m_upgradeToDatabaseRevision = databaseRevision;
}

int UpgradeDatabaseRevisionCommands::getUpgradeToDatabaseRevision() const
{
    return m_upgradeToDatabaseRevision;
}

void UpgradeDatabaseRevisionCommands::setSqlUpgradeCommands(const QStringList &upgradeCommands)
{
    m_sqlUpgradeCommands = upgradeCommands;
}

QStringList UpgradeDatabaseRevisionCommands::getSqlUpgradeCommands() const
{
    return m_sqlUpgradeCommands;
}


bool UpgradeDatabaseRevisionCommands::operator==(const UpgradeDatabaseRevisionCommands &upgradeDatabaseRevisionToCompare)
{
    return upgradeDatabaseRevisionToCompare.getUpgradeToDatabaseRevision() == this->getUpgradeToDatabaseRevision() &&
            upgradeDatabaseRevisionToCompare.getSqlUpgradeCommands() == this->getSqlUpgradeCommands();
}

}
