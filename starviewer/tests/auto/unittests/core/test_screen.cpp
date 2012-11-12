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

    void isOver_returnsExpectedValues_data();
    void isOver_returnsExpectedValues();

    void isUnder_returnsExpectedValues_data();
    void isUnder_returnsExpectedValues();

    void isOnLeft_returnsExpectedValues_data();
    void isOnLeft_returnsExpectedValues();

    void isOnRight_returnsExpectedValues_data();
    void isOnRight_returnsExpectedValues();

    void isTop_returnsExpectedValues_data();
    void isTop_returnsExpectedValues();

    void isBottom_returnsExpectedValues_data();
    void isBottom_returnsExpectedValues();

    void isLeft_returnsExpectedValues_data();
    void isLeft_returnsExpectedValues();

    void isRight_returnsExpectedValues_data();
    void isRight_returnsExpectedValues();

    void isTopLeft_returnsExpectedValues_data();
    void isTopLeft_returnsExpectedValues();

    void isTopRight_returnsExpectedValues_data();
    void isTopRight_returnsExpectedValues();

    void isBottomLeft_returnsExpectedValues_data();
    void isBottomLeft_returnsExpectedValues();

    void isBottomRight_returnsExpectedValues_data();
    void isBottomRight_returnsExpectedValues();

private:
    void setup_isHigherLower_data();
    void setup_isMoreToTheLeftRight_data();
    void setup_isOverUnderLeftRight_data();
    void setup_isTopBottomEtc_data();
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

void test_Screen::setup_isOverUnderLeftRight_data()
{
    QTest::addColumn<Screen>("screen1");
    QTest::addColumn<Screen>("screen2");
    QTest::addColumn<bool>("expectedIsOverValue");
    QTest::addColumn<bool>("expectedIsUnderValue");
    QTest::addColumn<bool>("expectedIsOnLeftValue");
    QTest::addColumn<bool>("expectedIsOnRightValue");

    Screen topLeftScreen;
    topLeftScreen.setGeometry(QRect(0, 0, 20, 20));
    
    Screen topRightScreen;
    topRightScreen.setGeometry(QRect(25, 0, 20, 20));

    Screen bottomLeftScreen;
    bottomLeftScreen.setGeometry(QRect(0, 25, 20, 20));

    Screen bottomRightScreen;
    bottomRightScreen.setID(3);
    bottomRightScreen.setGeometry(QRect(25, 25, 20, 20));

    QTest::newRow("Default build  empty screens") << Screen() << Screen() << true << true << true << true;
    QTest::newRow("Same geometry screens") << topLeftScreen << topLeftScreen << false << false << false << false;
    
    QTest::newRow("0 left- 1 right") << topLeftScreen << topRightScreen << false << false << true << false;
    QTest::newRow("2 left- 3 right") << bottomLeftScreen << bottomRightScreen << false << false << true << false;
    QTest::newRow("0 left top- 3 right bottom") << topLeftScreen << bottomRightScreen << true << false << true << false;
    QTest::newRow("2 left bottom- 1 right top") << bottomLeftScreen << topRightScreen << false << true << true << false;
    QTest::newRow("1 right- 0 left") << topRightScreen << topLeftScreen << false << false << false << true;
    QTest::newRow("3 right- 2 left") << bottomRightScreen << bottomLeftScreen << false << false << false << true;
    QTest::newRow("1 right top- 2 left bottom") << topRightScreen << bottomLeftScreen << true << false << false << true;
    QTest::newRow("3 right bottom- 0 left top") << bottomRightScreen << topLeftScreen << false << true << false << true;
    QTest::newRow("0 top - 2 bottom") << topLeftScreen << bottomLeftScreen << true << false << false << false;
    QTest::newRow("1 top - 3 bottom") << topRightScreen << bottomRightScreen << true << false << false << false;
    QTest::newRow("2 bottom - 0 top") << bottomLeftScreen << topLeftScreen<< false << true << false << false;
    QTest::newRow("3 bottom - 1 top") << bottomRightScreen << topRightScreen << false << true << false << false;
}

void test_Screen::isOver_returnsExpectedValues_data()
{
    this->setup_isOverUnderLeftRight_data();
}

void test_Screen::isOver_returnsExpectedValues()
{
    QFETCH(Screen, screen1);
    QFETCH(Screen, screen2);
    QFETCH(bool, expectedIsOverValue);

    QCOMPARE(screen1.isOver(screen2), expectedIsOverValue);
}

void test_Screen::isUnder_returnsExpectedValues_data()
{
    this->setup_isOverUnderLeftRight_data();
}

void test_Screen::isUnder_returnsExpectedValues()
{
    QFETCH(Screen, screen1);
    QFETCH(Screen, screen2);
    QFETCH(bool, expectedIsUnderValue);

    QCOMPARE(screen1.isUnder(screen2), expectedIsUnderValue);
}

void test_Screen::isOnLeft_returnsExpectedValues_data()
{
    this->setup_isOverUnderLeftRight_data();
}

void test_Screen::isOnLeft_returnsExpectedValues()
{
    QFETCH(Screen, screen1);
    QFETCH(Screen, screen2);
    QFETCH(bool, expectedIsOnLeftValue);

    QCOMPARE(screen1.isOnLeft(screen2), expectedIsOnLeftValue);
}

void test_Screen::isOnRight_returnsExpectedValues_data()
{
    this->setup_isOverUnderLeftRight_data();
}

void test_Screen::isOnRight_returnsExpectedValues()
{
    QFETCH(Screen, screen1);
    QFETCH(Screen, screen2);
    QFETCH(bool, expectedIsOnRightValue);

    QCOMPARE(screen1.isOnRight(screen2), expectedIsOnRightValue);
}

void test_Screen::setup_isTopBottomEtc_data()
{
    QTest::addColumn<Screen>("screen1");
    QTest::addColumn<Screen>("screen2");
    QTest::addColumn<bool>("expectedIsTopValue");
    QTest::addColumn<bool>("expectedIsBottomValue");
    QTest::addColumn<bool>("expectedIsLeftValue");
    QTest::addColumn<bool>("expectedIsRightValue");
    QTest::addColumn<bool>("expectedIsTopLeftValue");
    QTest::addColumn<bool>("expectedIsTopRightValue");
    QTest::addColumn<bool>("expectedIsBottomLeftValue");
    QTest::addColumn<bool>("expectedIsBottomRightValue");
    
    Screen topLeftScreen;
    topLeftScreen.setGeometry(QRect(0, 0, 20, 20));
    
    Screen topRightScreen;
    topRightScreen.setGeometry(QRect(21, 0, 20, 20));

    Screen bottomLeftScreen;
    bottomLeftScreen.setGeometry(QRect(0, 21, 20, 20));

    Screen bottomRightScreen;
    bottomRightScreen.setGeometry(QRect(21, 21, 20, 20));
    
    QTest::newRow("Default build screens") << Screen() << Screen() << true << true << true << true
        << true << true << true << true;
    
    QTest::newRow("top screen, same width & height, within separation limits (1)") << topLeftScreen << bottomLeftScreen << true << false << false << false
        << false << false << false << false;
    QTest::newRow("top screen, same width & height, within separation limits (2)") << topRightScreen << bottomRightScreen << true << false << false << false
        << false << false << false << false;
    
    QTest::newRow("bottom screen, same width & height, within separation limits (1)") << bottomLeftScreen << topLeftScreen << false << true << false << false
        << false << false << false << false;
    QTest::newRow("bottom screen, same width & height, within separation limits (2)") << bottomRightScreen << topRightScreen << false << true << false << false
        << false << false << false << false;
    
    QTest::newRow("left screen, same width & height, same top, within separation limits (1)") << topLeftScreen << topRightScreen << false << false << true << false
        << false << false << false << false;
    QTest::newRow("left screen, same width & height, same top, within separation limits (2)") << bottomLeftScreen << bottomRightScreen << false << false << true << false
        << false << false << false << false;
    QTest::newRow("left screen, same width & height, different top, within separation limits (1)") << topLeftScreen << bottomRightScreen << false << false << false << false
        << true << false << false << true; // TODO mirar cas true bottomRight, potser cal corregir mètode
    QTest::newRow("left screen, same width & height, different top, within separation limits (2)") << bottomLeftScreen << topRightScreen << false << false << false << false
        << false << true << true << false; // TODO mirar cas true topRight, potser cal corregir mètode

    QTest::newRow("right screen, same width & height, same top, within separation limits (1)") << topRightScreen << topLeftScreen << false << false << false << true
        << false << false << false << false;
    QTest::newRow("right screen, same width & height, same top, within separation limits (2)") << bottomRightScreen << bottomLeftScreen << false << false << false << true
        << false << false << false << false;
    
    QTest::newRow("right screen, same width & height, different top, within separation limits (1)") << bottomRightScreen << topLeftScreen << false << false << false << false
        << true << false << false << true; // TODO mirar cas true topLeft, potser cal corregir mètode
    QTest::newRow("right screen, same width & height, different top, within separation limits (2)") << topRightScreen << bottomLeftScreen << false << false << false << false
        << false << true << true << false; // TODO mirar cas true bottomLeft, potser cal corregir mètode

    // Fem que estiguin prou separades com perquè cap mètode doni true
    topLeftScreen.setGeometry(QRect(0, 0, 20, 20));
    topRightScreen.setGeometry(QRect(24, 0, 20, 20));
    bottomLeftScreen.setGeometry(QRect(0, 24, 20, 20));
    bottomRightScreen.setGeometry(QRect(24, 24, 20, 20));
    
    QTest::newRow("top screen, same width & height, outside separation limits (1)") << topLeftScreen << bottomLeftScreen << false << false << false << false
        << false << false << false << false;
    QTest::newRow("top screen, same width & height, outside separation limits (2)") << topRightScreen << bottomRightScreen << false << false << false << false
        << false << false << false << false;
    
    QTest::newRow("bottom screen, same width & height, outside separation limits (1)") << bottomLeftScreen << topLeftScreen << false << false << false << false
        << false << false << false << false;
    QTest::newRow("bottom screen, same width & height, outside separation limits (2)") << bottomRightScreen << topRightScreen << false << false << false << false
        << false << false << false << false;
    
    QTest::newRow("left screen, same width & height, same top, outside separation limits (1)") << topLeftScreen << topRightScreen << false << false << false << false
        << false << false << false << false;
    QTest::newRow("left screen, same width & height, same top, outside separation limits (2)") << bottomLeftScreen << bottomRightScreen << false << false << false << false
        << false << false << false << false;
    QTest::newRow("left screen, same width & height, different top, outside separation limits (1)") << topLeftScreen << bottomRightScreen << false << false << false << false
        << true << false << false << true; // TODO Mirar cas bottomRight true, potser cal corregir mètode
    QTest::newRow("left screen, same width & height, different top, outside separation limits (2)") << bottomLeftScreen << topRightScreen << false << false << false << false
        << false << true << true << false; // TODO Mirar cas topRight true, potser cal corregir mètode

    QTest::newRow("right screen, same width & height, same top, outside separation limits (1)") << topRightScreen << topLeftScreen << false << false << false << false
        << false << false << false << false;
    QTest::newRow("right screen, same width & height, same top, outside separation limits (2)") << bottomRightScreen << bottomLeftScreen << false << false << false << false
        << false << false << false << false;
    QTest::newRow("right screen, same width & height, different top, outside separation limits (1)") << bottomRightScreen << topLeftScreen << false << false << false << false
        << true << false << false << true; // TODO mirar cas true topLeft, potser cal corregir mètode
    QTest::newRow("right screen, same width & height, different top, outside separation limits (2)") << topRightScreen << bottomLeftScreen << false << false << false << false
        << false << true << true << false; // TODO mirar cas true bottomLeft, potser cal corregir mètode

    // Fem que tinguin mides diferents
    topLeftScreen.setGeometry(QRect(0, 0, 20, 15));
    topRightScreen.setGeometry(QRect(22, 0, 20, 30));
    bottomLeftScreen.setGeometry(QRect(0, 17, 20, 25));
    bottomRightScreen.setGeometry(QRect(22, 32, 30, 10));
    
    QTest::newRow("top screen, same width, different height, within separation limits (1)") << topLeftScreen << bottomLeftScreen << true << false << false << false
        << false << false << false << false;
    QTest::newRow("top screen, different width, same height, within separation limits (2)") << topRightScreen << bottomRightScreen << false << false << false << false
        << false << false << false << false;

    QTest::newRow("bottom screen, same width, different height, within separation limits (1)") << bottomLeftScreen << topLeftScreen << false << true << false << false
        << false << false << false << false;
    QTest::newRow("bottom screen, different width, same height, within separation limits (2)") << bottomRightScreen << topRightScreen << false << false << false << false
        << false << false << false << false;

    QTest::newRow("left screen, same width, different height, same top, within separation limits (1)") << topLeftScreen << topRightScreen << false << false << false << false
        << false << false << false << false;
    QTest::newRow("left screen, different width & height, different top, within separation limits (2)") << bottomLeftScreen << bottomRightScreen << false << false << false << false
        << false << false << false << false;
    QTest::newRow("left screen, different width & height, different top, within separation limits (1)") << topLeftScreen << bottomRightScreen << false << false << false << false
        << false << false << false << false;
    QTest::newRow("left screen, same width, different height, different top, within separation limits (2)") << bottomLeftScreen << topRightScreen << false << false << false << false
        << false << true << false << false; // TODO mirar cas topRight true, potser cal corregir el mètode

    QTest::newRow("right screen, same width, different height, same top, within separation limits (1)") << topRightScreen << topLeftScreen << false << false << false << false
        << false << false << false << false;
    QTest::newRow("right screen, different width & height, different top, within separation limits (2)") << bottomRightScreen << bottomLeftScreen << false << false << false << false
        << false << false << false << false;
    QTest::newRow("right screen, different width & height, different top, within separation limits (1)") << bottomRightScreen << topLeftScreen << false << false << false << false
        << false << false << false << false;
    QTest::newRow("right screen, same width, different height, different top, within separation limits (2)") << topRightScreen << bottomLeftScreen << false << false << false << false
        << false << false << true << false; // TODO mirar cas bottomLeft true, potser cal corregir el mètode
}

void test_Screen::isTop_returnsExpectedValues_data()
{
    this->setup_isTopBottomEtc_data();
}

void test_Screen::isTop_returnsExpectedValues()
{
    QFETCH(Screen, screen1);
    QFETCH(Screen, screen2);
    QFETCH(bool, expectedIsTopValue);

    QCOMPARE(screen1.isTop(screen2), expectedIsTopValue);
}

void test_Screen::isBottom_returnsExpectedValues_data()
{
    this->setup_isTopBottomEtc_data();
}

void test_Screen::isBottom_returnsExpectedValues()
{
    QFETCH(Screen, screen1);
    QFETCH(Screen, screen2);
    QFETCH(bool, expectedIsBottomValue);

    QCOMPARE(screen1.isBottom(screen2), expectedIsBottomValue);
}

void test_Screen::isLeft_returnsExpectedValues_data()
{
    this->setup_isTopBottomEtc_data();
}

void test_Screen::isLeft_returnsExpectedValues()
{
    QFETCH(Screen, screen1);
    QFETCH(Screen, screen2);
    QFETCH(bool, expectedIsLeftValue);

    QCOMPARE(screen1.isLeft(screen2), expectedIsLeftValue);
}

void test_Screen::isRight_returnsExpectedValues_data()
{
    this->setup_isTopBottomEtc_data();
}

void test_Screen::isRight_returnsExpectedValues()
{
    QFETCH(Screen, screen1);
    QFETCH(Screen, screen2);
    QFETCH(bool, expectedIsRightValue);

    QCOMPARE(screen1.isRight(screen2), expectedIsRightValue);
}

void test_Screen::isTopLeft_returnsExpectedValues_data()
{
    this->setup_isTopBottomEtc_data();
}

void test_Screen::isTopLeft_returnsExpectedValues()
{
    QFETCH(Screen, screen1);
    QFETCH(Screen, screen2);
    QFETCH(bool, expectedIsTopLeftValue);

    QCOMPARE(screen1.isTopLeft(screen2), expectedIsTopLeftValue);
}

void test_Screen::isTopRight_returnsExpectedValues_data()
{
    this->setup_isTopBottomEtc_data();
}

void test_Screen::isTopRight_returnsExpectedValues()
{
    QFETCH(Screen, screen1);
    QFETCH(Screen, screen2);
    QFETCH(bool, expectedIsTopRightValue);

    QCOMPARE(screen1.isTopRight(screen2), expectedIsTopRightValue);
}

void test_Screen::isBottomLeft_returnsExpectedValues_data()
{
    this->setup_isTopBottomEtc_data();
}

void test_Screen::isBottomLeft_returnsExpectedValues()
{
    QFETCH(Screen, screen1);
    QFETCH(Screen, screen2);
    QFETCH(bool, expectedIsBottomLeftValue);

    QCOMPARE(screen1.isBottomLeft(screen2), expectedIsBottomLeftValue);
}

void test_Screen::isBottomRight_returnsExpectedValues_data()
{
    this->setup_isTopBottomEtc_data();
}

void test_Screen::isBottomRight_returnsExpectedValues()
{
    QFETCH(Screen, screen1);
    QFETCH(Screen, screen2);
    QFETCH(bool, expectedIsBottomRightValue);

    QCOMPARE(screen1.isBottomRight(screen2), expectedIsBottomRightValue);
}

DECLARE_TEST(test_Screen)

#include "test_screen.moc"
