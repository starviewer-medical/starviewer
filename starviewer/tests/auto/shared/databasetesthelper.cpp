#include "databasetesthelper.h"

#include "databaseconnection.h"
#include "databaseinstallation.h"

using namespace udg;

namespace testing {

DatabaseConnection* DatabaseTestHelper::getEmptyDatabase()
{
    DatabaseConnection *databaseConnection = new DatabaseConnection();
    databaseConnection->setDatabasePath(":memory:");
    return databaseConnection;
}

DatabaseConnection* DatabaseTestHelper::getCreatedDatabase()
{
    DatabaseConnection *databaseConnection = getEmptyDatabase();
    DatabaseInstallation().createDatabase(*databaseConnection);
    return databaseConnection;
}

}
