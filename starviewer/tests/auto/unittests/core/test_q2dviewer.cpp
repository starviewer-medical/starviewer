#include "autotest.h"

#include "q2dviewer.h"

using namespace udg;

class test_Q2DViewer : public QObject {
Q_OBJECT

private slots:
    void getXYZIndexesForView_ShouldReturnExpectedValues_data();
    void getXYZIndexesForView_ShouldReturnExpectedValues();

    void getXYZIndexesForView_ShouldReturnMinusOne_data();
    void getXYZIndexesForView_ShouldReturnMinusOne();

    void getXIndexForView_ShouldReturnExpectedValues_data();
    void getXIndexForView_ShouldReturnExpectedValues();

    void getXIndexForView_ShouldReturnMinusOne_data();
    void getXIndexForView_ShouldReturnMinusOne();

    void getYIndexForView_ShouldReturnExpectedValues_data();
    void getYIndexForView_ShouldReturnExpectedValues();

    void getYIndexForView_ShouldReturnMinusOne_data();
    void getYIndexForView_ShouldReturnMinusOne();
    
    void getZIndexForView_ShouldReturnExpectedValues_data();
    void getZIndexForView_ShouldReturnExpectedValues();

    void getZIndexForView_ShouldReturnMinusOne_data();
    void getZIndexForView_ShouldReturnMinusOne();

private:
    void setupGetIndexForViewShouldReturnMinusOneData();
};

Q_DECLARE_METATYPE(Q2DViewer::CameraOrientationType)

void test_Q2DViewer::getXYZIndexesForView_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<Q2DViewer::CameraOrientationType>("view");
    QTest::addColumn<int>("xIndex");
    QTest::addColumn<int>("yIndex");
    QTest::addColumn<int>("zIndex");

    QTest::newRow("Axial view") << Q2DViewer::Axial << 0 << 1 << 2;
    QTest::newRow("Sagittal view") << Q2DViewer::Sagital << 1 << 2 << 0;
    QTest::newRow("Coronal view") << Q2DViewer::Coronal << 0 << 2 << 1;
}

void test_Q2DViewer::getXYZIndexesForView_ShouldReturnExpectedValues()
{
    QFETCH(Q2DViewer::CameraOrientationType, view);
    QFETCH(int, xIndex);
    QFETCH(int, yIndex);
    QFETCH(int, zIndex);
    
    int x, y, z;
    Q2DViewer::getXYZIndexesForView(x, y, z, view);
    
    QCOMPARE(x, xIndex);
    QCOMPARE(y, yIndex);
    QCOMPARE(z, zIndex);
}

void test_Q2DViewer::getXYZIndexesForView_ShouldReturnMinusOne_data()
{
    this->setupGetIndexForViewShouldReturnMinusOneData();
}

void test_Q2DViewer::getXYZIndexesForView_ShouldReturnMinusOne()
{
    QFETCH(int, view);

    int x, y, z;
    Q2DViewer::getXYZIndexesForView(x, y, z, (Q2DViewer::CameraOrientationType)view);
    
    QCOMPARE(x, -1);
    QCOMPARE(y, -1);
    QCOMPARE(z, -1);
}

void test_Q2DViewer::getXIndexForView_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<Q2DViewer::CameraOrientationType>("view");
    QTest::addColumn<int>("xIndex");

    QTest::newRow("Axial view") << Q2DViewer::Axial << 0;
    QTest::newRow("Sagittal view") << Q2DViewer::Sagital << 1;
    QTest::newRow("Coronal view") << Q2DViewer::Coronal << 0;
}

void test_Q2DViewer::getXIndexForView_ShouldReturnExpectedValues()
{
    QFETCH(Q2DViewer::CameraOrientationType, view);
    QFETCH(int, xIndex);
    
    QCOMPARE(Q2DViewer::getXIndexForView(view), xIndex);
}

void test_Q2DViewer::getXIndexForView_ShouldReturnMinusOne_data()
{
    this->setupGetIndexForViewShouldReturnMinusOneData();
}

void test_Q2DViewer::getXIndexForView_ShouldReturnMinusOne()
{
    QFETCH(int, view);    

    QCOMPARE(Q2DViewer::getXIndexForView((Q2DViewer::CameraOrientationType)view), -1);
}

void test_Q2DViewer::getYIndexForView_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<Q2DViewer::CameraOrientationType>("view");
    QTest::addColumn<int>("yIndex");

    QTest::newRow("Axial view") << Q2DViewer::Axial << 1;
    QTest::newRow("Sagittal view") << Q2DViewer::Sagital << 2;
    QTest::newRow("Coronal view") << Q2DViewer::Coronal << 2;
}

void test_Q2DViewer::getYIndexForView_ShouldReturnExpectedValues()
{
    QFETCH(Q2DViewer::CameraOrientationType, view);
    QFETCH(int, yIndex);
    
    QCOMPARE(Q2DViewer::getYIndexForView(view), yIndex);
}

void test_Q2DViewer::getYIndexForView_ShouldReturnMinusOne_data()
{
    this->setupGetIndexForViewShouldReturnMinusOneData();
}

void test_Q2DViewer::getYIndexForView_ShouldReturnMinusOne()
{
    QFETCH(int, view);

    QCOMPARE(Q2DViewer::getYIndexForView((Q2DViewer::CameraOrientationType)view), -1);
}

void test_Q2DViewer::getZIndexForView_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<Q2DViewer::CameraOrientationType>("view");
    QTest::addColumn<int>("zIndex");

    QTest::newRow("Axial view") << Q2DViewer::Axial << 2;
    QTest::newRow("Sagittal view") << Q2DViewer::Sagital << 0;
    QTest::newRow("Coronal view") << Q2DViewer::Coronal << 1;
}

void test_Q2DViewer::getZIndexForView_ShouldReturnExpectedValues()
{
    QFETCH(Q2DViewer::CameraOrientationType, view);
    QFETCH(int, zIndex);
    
    QCOMPARE(Q2DViewer::getZIndexForView(view), zIndex);
}

void test_Q2DViewer::getZIndexForView_ShouldReturnMinusOne_data()
{
    this->setupGetIndexForViewShouldReturnMinusOneData();
}

void test_Q2DViewer::getZIndexForView_ShouldReturnMinusOne()
{
    QFETCH(int, view);

    QCOMPARE(Q2DViewer::getZIndexForView((Q2DViewer::CameraOrientationType)view), -1);
}

void test_Q2DViewer::setupGetIndexForViewShouldReturnMinusOneData()
{
    QTest::addColumn<int>("view");

    QTest::newRow("view is -2") << -2;
    QTest::newRow("view is -1") << -1;
    QTest::newRow("view is 3") << 3;
    QTest::newRow("view is 4") << 4;
}

DECLARE_TEST(test_Q2DViewer)

#include "test_q2dviewer.moc"
