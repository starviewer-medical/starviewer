#include "autotest.h"
#include "screen.h"

using namespace udg;

class test_Screen : public QObject {
Q_OBJECT
private slots:
    void Screen_initializesValuesCorrectly();

    void operatorEquality_returnsExpectedValues_data();
    void operatorEquality_returnsExpectedValues();
};

Q_DECLARE_METATYPE(Screen)

void test_Screen::Screen_initializesValuesCorrectly()
{
    Screen screen;

    QCOMPARE(screen.getID(), -1);
    QCOMPARE(screen.isPrimary(), false);
    QCOMPARE(screen.getGeometry(), QRect());
    QCOMPARE(screen.getAvailableGeometry(), QRect());

    QRect geometry(3, 5, 100, 200);
    QRect availableGeometry(0, 2, 56, 123);

    Screen screen2(geometry, availableGeometry);
    QCOMPARE(screen2.getID(), -1);
    QCOMPARE(screen2.isPrimary(), false);
    QCOMPARE(screen2.getGeometry(), geometry);
    QCOMPARE(screen2.getAvailableGeometry(), availableGeometry);
}

void test_Screen::operatorEquality_returnsExpectedValues_data()
{
    QTest::addColumn<Screen>("screen1");
    QTest::addColumn<Screen>("screen2");
    QTest::addColumn<bool>("expectedValue");

    Screen screen1;
    Screen screen2;
    
    QTest::newRow("Ready build screens are equal") << screen1 << screen2 << true;

    QRect availableGeometry(5, 8, 210, 230);
    QRect geometry(0, 0, 250, 250);
    int id = 3;
    
    screen1.setAsPrimary(true);
    screen1.setAvailableGeometry(availableGeometry);
    screen1.setGeometry(geometry);
    screen1.setID(id);
    
    screen2.setAsPrimary(true);
    screen2.setAvailableGeometry(availableGeometry);
    screen2.setGeometry(geometry);
    screen2.setID(id);
    QTest::newRow("Modified screens with same values are equal") << screen1 << screen2 << true;

    QTest::newRow("Ready build screen and modified screen are not equal") << Screen() << screen2 << false;

    screen2.setAsPrimary(false);
    QTest::newRow("Equal values but primary, are not equal") << screen1 << screen2 << false;

    screen1.setAsPrimary(false);
    screen1.setAvailableGeometry(QRect(10, 10, 100, 100));
    QTest::newRow("Equal values but available geometry, are not equal") << screen1 << screen2 << false;

    screen1.setAvailableGeometry(availableGeometry);
    screen2.setGeometry(QRect(0, 0, 150, 150));
    QTest::newRow("Equal values but geometry, are not equal") << screen1 << screen2 << false;

    screen2.setGeometry(geometry);
    screen1.setID(5);
    QTest::newRow("Equal values but ID, are not equal") << screen1 << screen2 << false;
}

void test_Screen::operatorEquality_returnsExpectedValues()
{
    QFETCH(Screen, screen1);
    QFETCH(Screen, screen2);
    QFETCH(bool, expectedValue);

    QCOMPARE(screen1 == screen2, expectedValue);
}

DECLARE_TEST(test_Screen)

#include "test_screen.moc"
