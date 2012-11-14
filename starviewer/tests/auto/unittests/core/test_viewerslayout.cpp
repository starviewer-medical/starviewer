#include "autotest.h"
#include "viewerslayout.h"

using namespace udg;

class test_ViewersLayout : public QObject {
Q_OBJECT

private slots:
    void isRegular_ReturnsExpectedValues_data();
    void isRegular_ReturnsExpectedValues();
};

Q_DECLARE_METATYPE(ViewersLayout*)

void test_ViewersLayout::isRegular_ReturnsExpectedValues_data()
{
    QTest::addColumn<ViewersLayout*>("layout");
    QTest::addColumn<bool>("exepectedValue");

    ViewersLayout *layout = new ViewersLayout(0);
    QTest::newRow("Newly created layout") << layout << true; // TODO Potser s'hauria canviar el comportament d'aquest cas

    layout = new ViewersLayout(0);
    layout->cleanUp();
    QTest::newRow("Newly created layout after cleanUp()") << layout << false;
    
    layout = new ViewersLayout(0);
    layout->addRows(3);
    QTest::newRow("Newly created layout after addRows()") << layout << true;
    
    layout = new ViewersLayout(0);
    layout->addColumns(2);
    QTest::newRow("Newly created layout after addColumns()") << layout << true;
    
    layout = new ViewersLayout(0);
    layout->setGrid(3, 4);
    QTest::newRow("Newly created layout after setGrid()") << layout << true;
    
    QString viewerGeometry("0.0\\1.0\\1.0\\0.0");
    layout = new ViewersLayout(0);
    layout->addViewer(viewerGeometry);
    QTest::newRow("Newly created layout after addViewer()") << layout << false;
    
    layout = new ViewersLayout(0);
    layout->addViewer(viewerGeometry);
    layout->addRows(0);
    QTest::newRow("addRows(0) after addViewer()") << layout << false;
    
    layout = new ViewersLayout(0);
    layout->addViewer(viewerGeometry);
    layout->addColumns(0);
    QTest::newRow("addColumns(0) after addViewer()") << layout << false;
    
    layout = new ViewersLayout(0);
    layout->cleanUp();
    layout->addRows(0);
    QTest::newRow("addRows(0) after cleanUp()") << layout << false;

    layout = new ViewersLayout(0);
    layout->cleanUp();
    layout->addColumns(0);
    QTest::newRow("addColumns(0) after cleanUp()") << layout << false;
    
    // TODO En aquests casos en teoria s'hauria de canviar comportament perquè donin el resultat contrari
    layout = new ViewersLayout(0);
    layout->cleanUp();
    layout->setGrid(0, 0);
    QTest::newRow("setGrid(0, 0) after cleanUp()") << layout << true;
    
    layout = new ViewersLayout(0);
    layout->addViewer(viewerGeometry);
    layout->addRows(3);
    QTest::newRow("addRows() after addViewer()") << layout << false;

    layout = new ViewersLayout(0);
    layout->addViewer(viewerGeometry);
    layout->addColumns(4);
    QTest::newRow("addColumns() after addViewer()") << layout << false;

    layout = new ViewersLayout(0);
    layout->addViewer(viewerGeometry);
    layout->setGrid(0, 0);
    QTest::newRow("setGrid(0, 0) after addViewer()") << layout << true;
    
    layout = new ViewersLayout(0);
    layout->cleanUp();
    layout->addRows(3);
    QTest::newRow("addRows() after cleanUp()") << layout << false;

    layout = new ViewersLayout(0);
    layout->cleanUp();
    layout->addColumns(3);
    QTest::newRow("addColumns() after cleanUp()") << layout << false;

    // Aquest dóna el resultat correcte
    layout = new ViewersLayout(0);
    layout->cleanUp();
    layout->setGrid(3, 2);
    QTest::newRow("setGrid() after cleanUp()") << layout << true;

    // TODO En aquests casos en teoria s'hauria de canviar comportament perquè donin el resultat contrari
    layout = new ViewersLayout(0);
    layout->addRows(0);
    QTest::newRow("Newly created layout after addRows(0)") << layout << true;
    
    layout = new ViewersLayout(0);
    layout->addColumns(0);
    QTest::newRow("Newly created layout after addColumns(0)") << layout << true;
    
    layout = new ViewersLayout(0);
    layout->setGrid(0, 0);
    QTest::newRow("Newly created layout after setGrid(0, 0)") << layout << true;
}

void test_ViewersLayout::isRegular_ReturnsExpectedValues()
{
    QFETCH(ViewersLayout*, layout);
    QFETCH(bool, exepectedValue);

    QCOMPARE(layout->isRegular(), exepectedValue);
}

DECLARE_TEST(test_ViewersLayout)

#include "test_ViewersLayout.moc"

