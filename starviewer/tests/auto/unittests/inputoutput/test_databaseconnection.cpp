#include "autotest.h"
#include "databaseconnection.h"

#include "databasetesthelper.h"

#include <QSqlDatabase>

using namespace udg;
using namespace testing;

class test_DatabaseConnection : public QObject {

    Q_OBJECT

private slots:
    void getConnection_ShouldReturnOpenDatabase();

};

void test_DatabaseConnection::getConnection_ShouldReturnOpenDatabase()
{
    QScopedPointer<DatabaseConnection> databaseConnection(DatabaseTestHelper::getEmptyDatabase());
    QSqlDatabase connection = databaseConnection->getConnection();

    QVERIFY(connection.isOpen());
}

DECLARE_TEST(test_DatabaseConnection)

#include "test_databaseconnection.moc"
