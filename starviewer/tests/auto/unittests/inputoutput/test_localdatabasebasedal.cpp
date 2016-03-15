#include "autotest.h"
#include "localdatabasebasedal.h"

using namespace udg;

class test_LocalDatabaseBaseDAL : public QObject {

    Q_OBJECT

private slots:
    void convertToQString_QChar_ShouldReturnExpectedValue_data();
    void convertToQString_QChar_ShouldReturnExpectedValue();

};

void test_LocalDatabaseBaseDAL::convertToQString_QChar_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<QChar>("qchar");
    QTest::addColumn<QString>("expectedValue");

    QTest::newRow("default QChar") << QChar() << QString("");
    QTest::newRow("null char") << QChar('\0') << QString("");
    QTest::newRow("normal char") << QChar('a') << QString("a");
}

void test_LocalDatabaseBaseDAL::convertToQString_QChar_ShouldReturnExpectedValue()
{
    QFETCH(QChar, qchar);
    QFETCH(QString, expectedValue);

    QCOMPARE(LocalDatabaseBaseDAL::convertToQString(qchar), expectedValue);
}

DECLARE_TEST(test_LocalDatabaseBaseDAL)

#include "test_localdatabasebasedal.moc"
