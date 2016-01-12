#include "databasetesthelper.h"

#include "databaseconnection.h"

using namespace udg;

namespace testing {

DatabaseConnection* DatabaseTestHelper::getEmptyDatabase()
{
    DatabaseConnection *databaseConnection = new DatabaseConnection();
    databaseConnection->setDatabasePath(":memory:");
    return databaseConnection;
}

}
