#include "autotest.h"

#include "q2dviewer.h"

using namespace udg;

class test_Q2DViewer : public QObject {
Q_OBJECT

private slots:
    void getXYZIndexesForView_ShouldReturnExpectedValues_data();
    void getXYZIndexesForView_ShouldReturnExpectedValues();

    void getXIndexForView_ShouldReturnExpectedValues_data();
    void getXIndexForView_ShouldReturnExpectedValues();

    void getYIndexForView_ShouldReturnExpectedValues_data();
    void getYIndexForView_ShouldReturnExpectedValues();

    void getZIndexForView_ShouldReturnExpectedValues_data();
    void getZIndexForView_ShouldReturnExpectedValues();
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

DECLARE_TEST(test_Q2DViewer)

#include "test_q2dviewer.moc"
