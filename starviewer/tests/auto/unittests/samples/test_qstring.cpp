/*@
    "name": "test_QString",
    "requirements": ["technical.platforms"]
 */
// The requirement listed above allows to avoid a red flag for platforms that do not have a specific test. In fact, technically, all tests should list this
// requirement or one for a specific platform, because these are always implicitly tested but it's easier to just add this here to have a good graphic without
// all the repetition.

#include "autotest.h"
#include <QString>

class test_QString : public QObject {
Q_OBJECT

private slots:
    void toUpper_ShouldReturnAnUppercaseCopyOfTheString_data();
    void toUpper_ShouldReturnAnUppercaseCopyOfTheString();

    void prepend_ShouldPrependTheFirstStringToTheBeginningOfTheSecondString_data();
    void prepend_ShouldPrependTheFirstStringToTheBeginningOfTheSecondString();
};

void test_QString::toUpper_ShouldReturnAnUppercaseCopyOfTheString()
{
    QFETCH(QString, string);
    QFETCH(QString, result);

    QCOMPARE(string.toUpper(), result);
}

void test_QString::toUpper_ShouldReturnAnUppercaseCopyOfTheString_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");

    QTest::newRow("all lower") << "starviewer" << "STARVIEWER";
    QTest::newRow("mixed")     << "StarViewer" << "STARVIEWER";
    QTest::newRow("all upper") << "STARVIEWER" << "STARVIEWER";
}

void test_QString::prepend_ShouldPrependTheFirstStringToTheBeginningOfTheSecondString()
{
    QFETCH(QString, prependString);
    QFETCH(QString, string);
    QFETCH(QString, result);

    QCOMPARE(string.prepend(prependString), result);
}

void test_QString::prepend_ShouldPrependTheFirstStringToTheBeginningOfTheSecondString_data()
{
    QTest::addColumn<QString>("prependString");
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");

    QTest::newRow("") << "Star" << "viewer" << "Starviewer";
    QTest::newRow("") << " " << "Starviewer" << " Starviewer";
    QTest::newRow("") << "" << "Starviewer" << "Starviewer";
    QTest::newRow("") << "Medical " << "Imaging Software" << "Medical Imaging Software";
}


DECLARE_TEST(test_QString)

#include "test_qstring.moc"
