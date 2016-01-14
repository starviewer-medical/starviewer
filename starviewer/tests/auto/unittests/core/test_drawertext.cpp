#include "autotest.h"

#include "drawertext.h"
#include "mathtools.h"
#include "fuzzycomparetesthelper.h"

#include <QList>
#include <QVector3D>

using namespace udg;
using namespace testing;

class TestingDrawerText : public DrawerText {
public:
    double m_bounds[6];

public:
    virtual void getBounds(double bounds[6])
    {
        for (int i = 0; i < 6; ++i)
        {
            bounds[i] = m_bounds[i];
        }
    }

    void setBounds(const QList<double> &list)
    {
        int i = 0;
        foreach (double component, list)
        {
            m_bounds[i] = component;
            i++;
        }
    }
};

Q_DECLARE_METATYPE(QList<double>)

class test_DrawerText: public QObject {
Q_OBJECT

private slots:
    void getDistanceToPoint_ShouldReturnExpectedDistanceAndClosestPoint_data();
    void getDistanceToPoint_ShouldReturnExpectedDistanceAndClosestPoint();
};

void test_DrawerText::getDistanceToPoint_ShouldReturnExpectedDistanceAndClosestPoint_data()
{
    QTest::addColumn<QList<double> >("bounds");
    QTest::addColumn<QVector3D>("pointToCheck");
    QTest::addColumn<double>("expectedDistance");
    QTest::addColumn<QVector3D>("expectedClosestPoint");

    QList<double> bounds;
    bounds << 0.0 << 2.0 << 0.2 << 1.0 << -0.15 << 1.0;
    QVector3D pointInsideBounds(0.5, 0.5, 0.5);
    QVector3D pointOutsideBoundsInX(2.4f, 0.5f, 0.5f);
    QVector3D pointOutsideBoundsInXNegative(-2.4f, 0.5f, 0.5f);
    QVector3D pointOutsideBoundsInY(0.5f, 2.4f, 0.5f);
    QVector3D pointOutsideBoundsInYNegative(0.5f, -2.4f, 0.5f);
    QVector3D pointOutsideBoundsInZ(0.5f, 0.5f, 2.4f);
    QVector3D pointOutsideBoundsInZNegative(0.5f, 0.5f, -2.4f);

    QList<double> boundsSameXminXmax;
    boundsSameXminXmax << 1.234 << 1.234 << 0.0 << 1.0 << 0.0 << 1.0;
    QVector3D pointInsideBoundsSameXminXmax(1.234f, 0.2f, 0.2f);

    QList<double> boundsSameYminYmax;
    boundsSameYminYmax << -12.0 << -5.0 << -5.554 << -5.554 << 0.0 << 1.0;
    QVector3D pointInsideBoundsSameYminYmax(-11.666f, -5.554f, 0.8f);

    QList<double> boundsSameZminZmax;
    boundsSameZminZmax << -4.0 << 15.0 << 2.453 << 2.876 << 4.4443 << 4.4443;
    QVector3D pointInsideBoundsSameZminZmax(1.666f, 2.554f, 4.4443f);

    QTest::newRow("all min and max different, point inside") << bounds << pointInsideBounds << 0.0 << pointInsideBounds;
    QTest::newRow("same Xmin and Xmax, point inside") << boundsSameXminXmax << pointInsideBoundsSameXminXmax << 0.0 << pointInsideBoundsSameXminXmax;
    QTest::newRow("same Ymin and Ymax, point inside") << boundsSameYminYmax << pointInsideBoundsSameYminYmax << 0.0 << pointInsideBoundsSameYminYmax;
    QTest::newRow("same Zmin and Zmax, point inside") << boundsSameZminZmax << pointInsideBoundsSameZminZmax << 0.0 << pointInsideBoundsSameZminZmax;

    QTest::newRow("point outside in X") << bounds << pointOutsideBoundsInX << 0.4 << QVector3D(2.0f, 0.5f, 0.5f);
    QTest::newRow("point outside in Y") << bounds << pointOutsideBoundsInY << 1.4 << QVector3D(0.5f, 1.0f, 0.5f);
    QTest::newRow("point outside in Z") << bounds << pointOutsideBoundsInZ << 1.4 << QVector3D(0.5f, 0.5f, 1.0f);
    QTest::newRow("point outside in X negative") << bounds << pointOutsideBoundsInXNegative << 2.4 << QVector3D(0.0f, 0.5f, 0.5f);
    QTest::newRow("point outside in Y negative") << bounds << pointOutsideBoundsInYNegative << 2.6 << QVector3D(0.5f, 0.2f, 0.5f);
    QTest::newRow("point outside in Z negative") << bounds << pointOutsideBoundsInZNegative << 2.25 << QVector3D(0.5f, 0.5f, -0.15f);

}

void test_DrawerText::getDistanceToPoint_ShouldReturnExpectedDistanceAndClosestPoint()
{
    QFETCH(QList<double>, bounds);
    QFETCH(QVector3D, pointToCheck);
    QFETCH(double, expectedDistance);
    QFETCH(QVector3D, expectedClosestPoint);

    TestingDrawerText drawerText;
    drawerText.setBounds(bounds);

    double point[3] = {pointToCheck.x(), pointToCheck.y(), pointToCheck.z()};
    double closestPoint[3];
    double distance = drawerText.getDistanceToPoint(point, closestPoint);
    QVector3D closestPoint3D(closestPoint[0], closestPoint[1], closestPoint[2]);

    QString distanceError(QString("Compared values are not the same\nActual: %1\nExpected: %2").arg(distance).arg(expectedDistance));
    QString closestPointError(QString("Compared values are not the same\nActual: (%1, %2, %3)\nExpected: (%4, %5, %6")
                              .arg(closestPoint[0]).arg(closestPoint[1]).arg(closestPoint[2])
                              .arg(expectedClosestPoint.x()).arg(expectedClosestPoint.y()).arg(expectedClosestPoint.z()));

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(distance, expectedDistance, 0.001), qPrintable(distanceError));
    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(closestPoint3D, expectedClosestPoint), qPrintable(closestPointError));
}

DECLARE_TEST(test_DrawerText)

#include "test_drawertext.moc"

