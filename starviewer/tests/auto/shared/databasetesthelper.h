#ifndef DATABASETESTHELPER_H
#define DATABASETESTHELPER_H

namespace udg {
class DatabaseConnection;
}

namespace testing {

/**
 * @brief The DatabaseTestHelper class has methods to create in-memory databases for testing.
 */
class DatabaseTestHelper
{
public:
    /// Returns an empty in-memory database.
    static udg::DatabaseConnection* getEmptyDatabase();
    /// Returns an in-memory database with the tables created but empty.
    static udg::DatabaseConnection* getCreatedDatabase();
};

}

#endif // DATABASETESTHELPER_H
