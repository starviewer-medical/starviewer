#include "autotest.h"
#include "screen.h"

using namespace udg;

class test_Screen : public QObject {
Q_OBJECT
private slots:
    void Screen_initializesValuesCorrectly();

    void operatorEquality_returnsExpectedValues_data();
    void operatorEquality_returnsExpectedValues();

    void isHigher_returnsExpectedValues_data();
    void isHigher_returnsExpectedValues();

    void isLower_returnsExpectedValues_data();
    void isLower_returnsExpectedValues();

    void isMoreToTheLeft_returnsExpectedValues_data();
    void isMoreToTheLeft_returnsExpectedValues();

    void isMoreToTheRight_returnsExpectedValues_data();
    void isMoreToTheRight_returnsExpectedValues();

private:
    void setup_isHigherLower_data();
    void setup_isMoreToTheLeftRight_data();
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

void test_Screen::setup_isHigherLower_data()
{
    QTest::addColumn<Screen>("screen1");
    QTest::addColumn<Screen>("screen2");
    QTest::addColumn<bool>("isHigherExpectedValue");
    QTest::addColumn<bool>("isLowerExpectedValue");

    Screen screen1;
    QRect geometry1(QRect(0, 0, 100, 100));
    screen1.setGeometry(geometry1);
    
    Screen screen2;
    screen2.setGeometry(geometry1);

    QTest::newRow("Same geometry") << screen1 << screen2 << false << false;

    QRect geometry2 = geometry1;
    geometry2.setWidth(200);
    geometry2.setHeight(200);
    screen2.setGeometry(geometry2);
    QTest::newRow("Same topLeft, different size") << screen1 << screen2 << false << false;

    geometry2 = geometry1;
    geometry2.setTop(25);
    screen2.setGeometry(geometry2);
    QTest::newRow("Second screen below, same size") << screen1 << screen2 << true << false;

    QTest::newRow("Second screen above, same size") << screen2 << screen1 << false << true;
}

void test_Screen::isHigher_returnsExpectedValues_data()
{
    this->setup_isHigherLower_data();
}

void test_Screen::isHigher_returnsExpectedValues()
{
    QFETCH(Screen, screen1);
    QFETCH(Screen, screen2);
    QFETCH(bool, isHigherExpectedValue);

    QCOMPARE(screen1.isHigher(screen2), isHigherExpectedValue);
}

void test_Screen::isLower_returnsExpectedValues_data()
{
    this->setup_isHigherLower_data();
}

void test_Screen::isLower_returnsExpectedValues()
{
    QFETCH(Screen, screen1);
    QFETCH(Screen, screen2);
    QFETCH(bool, isLowerExpectedValue);

    QCOMPARE(screen1.isLower(screen2), isLowerExpectedValue);
}

void test_Screen::setup_isMoreToTheLeftRight_data()
{
    QTest::addColumn<Screen>("screen1");
    QTest::addColumn<Screen>("screen2");
    QTest::addColumn<bool>("leftExpectedValue");
    QTest::addColumn<bool>("rightExpectedValue");

    Screen screen1;
    QRect geometry1(QRect(0, 0, 100, 100));
    screen1.setGeometry(geometry1);
    
    Screen screen2;
    screen2.setGeometry(geometry1);

    QTest::newRow("Same geometry") << screen1 << screen2 << false << false;

    QRect geometry2 = geometry1;
    geometry2.setWidth(200);
    geometry2.setHeight(200);
    screen2.setGeometry(geometry2);
    QTest::newRow("Same left, 2nd with gretaer width") << screen1 << screen2 << false << false;

    geometry2 = geometry1;
    geometry2.moveLeft(25);
    screen2.setGeometry(geometry2);
    QTest::newRow("Second screen more on the left, same size") << screen1 << screen2 << true << false;
    QTest::newRow("Second more on the right, same size") << screen2 << screen1 << false << true;
}

void test_Screen::isMoreToTheLeft_returnsExpectedValues_data()
{
    this->setup_isMoreToTheLeftRight_data();
}

void test_Screen::isMoreToTheLeft_returnsExpectedValues()
{
    QFETCH(Screen, screen1);
    QFETCH(Screen, screen2);
    QFETCH(bool, leftExpectedValue);

    QCOMPARE(screen1.isMoreToTheLeft(screen2), leftExpectedValue);
}

void test_Screen::isMoreToTheRight_returnsExpectedValues_data()
{
    this->setup_isMoreToTheLeftRight_data();
}

void test_Screen::isMoreToTheRight_returnsExpectedValues()
{
    QFETCH(Screen, screen1);
    QFETCH(Screen, screen2);
    QFETCH(bool, rightExpectedValue);

    QCOMPARE(screen1.isMoreToTheRight(screen2), rightExpectedValue);
}

DECLARE_TEST(test_Screen)

#include "test_screen.moc"
