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
