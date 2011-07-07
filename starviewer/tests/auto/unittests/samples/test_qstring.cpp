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
