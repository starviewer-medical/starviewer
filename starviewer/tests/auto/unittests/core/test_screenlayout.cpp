#include "autotest.h"
#include "screenlayout.h"
#include "screen.h"

#include <QDebug>

using namespace udg;

class test_ScreenLayout : public QObject {
Q_OBJECT
private slots:
    void addScreeen_returnsExpectedValues_data();
    void addScreeen_returnsExpectedValues();

    void getScreen_returnsExpectedValues_data();
    void getScreen_returnsExpectedValues();

    void clear_doesItsJob_data();
    void clear_doesItsJob();

    void getPrimaryScreenID_returnsExpectedValues_data();
    void getPrimaryScreenID_returnsExpectedValues();
    
    void isOver_returnsExpectedValues_data();
    void isOver_returnsExpectedValues();

    void isUnder_returnsExpectedValues_data();
    void isUnder_returnsExpectedValues();

    void isOnLeft_returnsExpectedValues_data();
    void isOnLeft_returnsExpectedValues();

    void isOnRight_returnsExpectedValues_data();
    void isOnRight_returnsExpectedValues();

    void getScreenOnTheRightOf_returnsExpectedValues_data();
    void getScreenOnTheRightOf_returnsExpectedValues();

    void getScreenOnTheLeftOf_returnsExpectedValues_data();
    void getScreenOnTheLeftOf_returnsExpectedValues();

    void getPreviousScreenOf_returnsExpectedValues_data();
    void getPreviousScreenOf_returnsExpectedValues();

    void getNextScreenOf_returnsExpectedValues_data();
    void getNextScreenOf_returnsExpectedValues();

private:
    /// Retorna un layout base
    ScreenLayout getSampleScreenLayout();

    /// Prepara les dades comunes dels testos unitaris de isOver(), isUnder(), isOnLeft() i isOnRight()
    void setup_isOverUnderLeftRight_data();

    /// Prepara les dades comunes dels testos unitaris de getScreenOnTheRight/LeftOf()
    void setup_getScreenOnThe_data();

    void setup_getPreviousNextScreenOf_data();
};

Q_DECLARE_METATYPE(QList<Screen>)
Q_DECLARE_METATYPE(Screen)
Q_DECLARE_METATYPE(ScreenLayout)

void test_ScreenLayout::addScreeen_returnsExpectedValues_data()
{
    QTest::addColumn<QList<Screen> >("existingScreensInLayout");
    QTest::addColumn<Screen>("screenToAdd");
    QTest::addColumn<bool>("expectedReturnValue");
    QTest::addColumn<int>("expectedNumberOfScreens");

    QTest::newRow("null screen, empty layout") << QList<Screen>() << Screen() << false << 0;

    Screen screen;
    screen.setID(0);
    QTest::newRow("valid non-existing screen, empty layout") << QList<Screen>() << screen << true << 1;

    QList<Screen> screensList;
    Screen screen1;
    screen1.setID(1);

    screensList << screen << screen1;
    QTest::newRow("null screen, 2-screen layout") << screensList << Screen() << false << screensList.count();

    Screen screen2;
    screen2.setID(2);
    QTest::newRow("valid non-existing screen, 2-screen layout") << screensList << screen2 << true << screensList.count() + 1;

    Screen screen3;
    screen3.setID(3);
    screen3.setAsPrimary(true);
    QTest::newRow("adding valid primary screen, 2-screen layout with no primary screen") << screensList << screen3 << true << screensList.count() + 1;

    screensList.clear();
    screen2.setAsPrimary(true);
    screensList << screen1 << screen2;

    QTest::newRow("adding valid primary screen, 2-screen layout with primary screen") << screensList << screen3 << false << screensList.count();

    QTest::newRow("adding valid non-primary screen, 2-screen layout with primary screen") << screensList << screen << true << screensList.count() + 1;
}

void test_ScreenLayout::addScreeen_returnsExpectedValues()
{
    QFETCH(QList<Screen>, existingScreensInLayout);
    QFETCH(Screen, screenToAdd);
    QFETCH(bool, expectedReturnValue);
    QFETCH(int, expectedNumberOfScreens);

    ScreenLayout layout;

    foreach (Screen screen, existingScreensInLayout)
    {
        layout.addScreen(screen);
    }

    QCOMPARE(layout.addScreen(screenToAdd), expectedReturnValue);
    QCOMPARE(layout.getNumberOfScreens(), expectedNumberOfScreens);
}

void test_ScreenLayout::getScreen_returnsExpectedValues_data()
{
    QTest::addColumn<QList<Screen> >("existingScreensInLayout");
    QTest::addColumn<int>("screenID");
    QTest::addColumn<Screen>("expectedScreen");
    
    Screen screen;
    screen.setID(0);
    Screen screen1;
    screen1.setID(1);
    
    QList<Screen> screensList;
    screensList << screen << screen1;
    
    QTest::newRow("empty layout, invalid ID") << QList<Screen>() << -1 << Screen();
    QTest::newRow("empty layout, non-existing ID") << QList<Screen>() << 1 << Screen();
    QTest::newRow("layout with screens, non-existing ID") << screensList << 2 << Screen();
    QTest::newRow("layout with screens, existing ID") << screensList << 1 << screen1;
}

void test_ScreenLayout::getScreen_returnsExpectedValues()
{
    QFETCH(QList<Screen>, existingScreensInLayout);
    QFETCH(int, screenID);
    QFETCH(Screen, expectedScreen);
    
    ScreenLayout layout;
    foreach (Screen screen, existingScreensInLayout)
    {
        layout.addScreen(screen);
    }
    
    QCOMPARE(layout.getScreen(screenID), expectedScreen);
}

void test_ScreenLayout::clear_doesItsJob_data()
{
    QTest::addColumn<ScreenLayout>("layout");

    QTest::newRow("empty list") << ScreenLayout();
    QTest::newRow("layout with some screens") << getSampleScreenLayout();
}

void test_ScreenLayout::clear_doesItsJob()
{
    QFETCH(ScreenLayout, layout);
    
    layout.clear();
    QCOMPARE(layout.getNumberOfScreens(), 0);
}

void test_ScreenLayout::getPrimaryScreenID_returnsExpectedValues_data()
{
    QTest::addColumn<ScreenLayout>("layout");
    QTest::addColumn<int>("expectedValue");

    QTest::newRow("Empty layout") << ScreenLayout() << -1;
    QTest::newRow("No primary screens in layout") << getSampleScreenLayout() << -1;
    
    Screen screen;
    screen.setID(5);
    screen.setAsPrimary(true);

    ScreenLayout layout = getSampleScreenLayout();
    layout.addScreen(screen);
    QTest::newRow("Layout with primary screen") << layout << 5;
}

void test_ScreenLayout::getPrimaryScreenID_returnsExpectedValues()
{
    QFETCH(ScreenLayout, layout);
    QFETCH(int, expectedValue);

    QCOMPARE(layout.getPrimaryScreenID(), expectedValue);
}

void test_ScreenLayout::setup_isOverUnderLeftRight_data()
{
    QTest::addColumn<ScreenLayout>("layout");
    QTest::addColumn<int>("screen1ID");
    QTest::addColumn<int>("screen2ID");
    QTest::addColumn<bool>("expectedIsOverValue");
    QTest::addColumn<bool>("expectedIsUnderValue");
    QTest::addColumn<bool>("expectedIsOnLeftValue");
    QTest::addColumn<bool>("expectedIsOnRightValue");

    QTest::newRow("non-existing IDs 1") << getSampleScreenLayout() << 400 << 500 << false << false << false << false;
    QTest::newRow("non-existing IDs 2") << getSampleScreenLayout() << -1 << -1 << false << false << false << false;
    
    QTest::newRow("same existing IDs") << getSampleScreenLayout() << 1 << 1 << false << false << false << false;
    QTest::newRow("0 left- 1 right") << getSampleScreenLayout() << 0 << 1 << false << false << true << false;
    QTest::newRow("2 left- 3 right") << getSampleScreenLayout() << 2 << 3 << false << false << true << false;
    QTest::newRow("0 left top- 3 right bottom") << getSampleScreenLayout() << 0 << 3 << true << false << true << false;
    QTest::newRow("2 left bottom- 1 right top") << getSampleScreenLayout() << 2 << 1 << false << true << true << false;
    QTest::newRow("1 right- 0 left") << getSampleScreenLayout() << 1 << 0 << false << false << false << true;
    QTest::newRow("3 right- 2 left") << getSampleScreenLayout() << 3 << 2 << false << false << false << true;
    QTest::newRow("1 right top- 2 left bottom") << getSampleScreenLayout() << 1 << 2 << true << false << false << true;
    QTest::newRow("3 right bottom- 0 left top") << getSampleScreenLayout() << 3 << 0 << false << true << false << true;
    QTest::newRow("0 top - 2 bottom") << getSampleScreenLayout() << 0 << 2 << true << false << false << false;
    QTest::newRow("1 top - 3 bottom") << getSampleScreenLayout() << 1 << 3 << true << false << false << false;
    QTest::newRow("2 bottom - 0 top") << getSampleScreenLayout() << 2 << 0 << false << true << false << false;
    QTest::newRow("3 bottom - 1 top") << getSampleScreenLayout() << 3 << 1 << false << true << false << false;
}

void test_ScreenLayout::isOver_returnsExpectedValues_data()
{
    this->setup_isOverUnderLeftRight_data();
}

void test_ScreenLayout::isOver_returnsExpectedValues()
{
    QFETCH(ScreenLayout, layout);
    QFETCH(int, screen1ID);
    QFETCH(int, screen2ID);
    QFETCH(bool, expectedIsOverValue);

    QCOMPARE(layout.isOver(screen1ID, screen2ID), expectedIsOverValue);
}

void test_ScreenLayout::isUnder_returnsExpectedValues_data()
{
    this->setup_isOverUnderLeftRight_data();
}

void test_ScreenLayout::isUnder_returnsExpectedValues()
{
    QFETCH(ScreenLayout, layout);
    QFETCH(int, screen1ID);
    QFETCH(int, screen2ID);
    QFETCH(bool, expectedIsUnderValue);

    QCOMPARE(layout.isUnder(screen1ID, screen2ID), expectedIsUnderValue);
}

void test_ScreenLayout::isOnLeft_returnsExpectedValues_data()
{
    this->setup_isOverUnderLeftRight_data();
}

void test_ScreenLayout::isOnLeft_returnsExpectedValues()
{
    QFETCH(ScreenLayout, layout);
    QFETCH(int, screen1ID);
    QFETCH(int, screen2ID);
    QFETCH(bool, expectedIsOnLeftValue);

    QCOMPARE(layout.isOnLeft(screen1ID, screen2ID), expectedIsOnLeftValue);
}

void test_ScreenLayout::isOnRight_returnsExpectedValues_data()
{
    this->setup_isOverUnderLeftRight_data();
}

void test_ScreenLayout::isOnRight_returnsExpectedValues()
{
    QFETCH(ScreenLayout, layout);
    QFETCH(int, screen1ID);
    QFETCH(int, screen2ID);
    QFETCH(bool, expectedIsOnRightValue);

    QCOMPARE(layout.isOnRight(screen1ID, screen2ID), expectedIsOnRightValue);
}

void test_ScreenLayout::setup_getScreenOnThe_data()
{
    QTest::addColumn<ScreenLayout>("layout");
    QTest::addColumn<int>("screenID");
    QTest::addColumn<int>("expectedOnTheRightValue");
    QTest::addColumn<int>("expectedOnTheLeftValue");

    QTest::newRow("non-existing screen (-1)") << getSampleScreenLayout() << -1 << -1 << -1;
    QTest::newRow("non-existing screen") << getSampleScreenLayout() << 500 << -1 << -1;
    QTest::newRow("has screen on the left (1)") << getSampleScreenLayout() << 1 << -1 << 0;
    QTest::newRow("has screen on the left (2)") << getSampleScreenLayout() << 3 << -1 << 2;
    QTest::newRow("has screen on the right (1)") << getSampleScreenLayout() << 0 << 1 << -1;
    QTest::newRow("has screen on the right (2)") << getSampleScreenLayout() << 2 << 3 << -1;
}

void test_ScreenLayout::getScreenOnTheRightOf_returnsExpectedValues_data()
{
    this->setup_getScreenOnThe_data();
}

void test_ScreenLayout::getScreenOnTheRightOf_returnsExpectedValues()
{
    QFETCH(ScreenLayout, layout);
    QFETCH(int, screenID);
    QFETCH(int, expectedOnTheRightValue);

    QCOMPARE(layout.getScreenOnTheRightOf(screenID), expectedOnTheRightValue);
}

void test_ScreenLayout::getScreenOnTheLeftOf_returnsExpectedValues_data()
{
    this->setup_getScreenOnThe_data();
}

void test_ScreenLayout::getScreenOnTheLeftOf_returnsExpectedValues()
{
    QFETCH(ScreenLayout, layout);
    QFETCH(int, screenID);
    QFETCH(int, expectedOnTheLeftValue);

    QCOMPARE(layout.getScreenOnTheLeftOf(screenID), expectedOnTheLeftValue);
}

void test_ScreenLayout::setup_getPreviousNextScreenOf_data()
{
    QTest::addColumn<ScreenLayout>("layout");
    QTest::addColumn<int>("screenID");
    QTest::addColumn<int>("expectedPreviousValue");
    QTest::addColumn<int>("expectedNextValue");

    Screen screen(QRect(0, 0, 100, 100), QRect());
    screen.setID(0);
    ScreenLayout layout;
    layout.addScreen(screen);
    QTest::newRow("Of single screen layout") << layout << 0 << 0 << 0;
    
    QTest::newRow("Non existing screen") << getSampleScreenLayout() << 600 << -1 << -1;
    QTest::newRow("Invalid ID") << getSampleScreenLayout() << -1 << -1 << -1;
    QTest::newRow("Screen 0") << getSampleScreenLayout() << 0 << 3 << 1;
    QTest::newRow("Screen 1") << getSampleScreenLayout() << 1 << 0 << 2;
    QTest::newRow("Screen 2") << getSampleScreenLayout() << 2 << 1 << 3;
    QTest::newRow("Screen 3") << getSampleScreenLayout() << 3 << 2 << 0;
}

void test_ScreenLayout::getPreviousScreenOf_returnsExpectedValues_data()
{
    this->setup_getPreviousNextScreenOf_data();
}

void test_ScreenLayout::getPreviousScreenOf_returnsExpectedValues()
{
    QFETCH(ScreenLayout, layout);
    QFETCH(int, screenID);
    QFETCH(int, expectedPreviousValue);

    QCOMPARE(layout.getPreviousScreenOf(screenID), expectedPreviousValue);
}

void test_ScreenLayout::getNextScreenOf_returnsExpectedValues_data()
{
    this->setup_getPreviousNextScreenOf_data();
}

void test_ScreenLayout::getNextScreenOf_returnsExpectedValues()
{
    QFETCH(ScreenLayout, layout);
    QFETCH(int, screenID);
    QFETCH(int, expectedNextValue);

    QCOMPARE(layout.getNextScreenOf(screenID), expectedNextValue);
}

ScreenLayout test_ScreenLayout::getSampleScreenLayout()
{
    // Fem un layout de 4 pantalles, distribuïdes uniformement en una matriu 2x2
    //  _____ _____
    //  |   | |   |
    //  | 0 | | 1 |
    //  ----- -----
    //  _____ _____
    //  |   | |   |
    //  | 2 | | 3 |
    //  ----- -----
    
    ScreenLayout layout;
    
    Screen screen;
    screen.setID(0);
    screen.setGeometry(QRect(0, 0, 20, 20));
    
    layout.addScreen(screen);

    screen.setID(1);
    screen.setGeometry(QRect(25, 0, 20, 20));

    layout.addScreen(screen);

    screen.setID(2);
    screen.setGeometry(QRect(0, 25, 20, 20));

    layout.addScreen(screen);

    screen.setID(3);
    screen.setGeometry(QRect(25, 25, 20, 20));

    layout.addScreen(screen);

    return layout;
}

DECLARE_TEST(test_ScreenLayout)

#include "test_screenlayout.moc"
