#include "autotest.h"

#include "drawertext.h"
#include "mathtools.h"
#include "fuzzycomparetesthelper.h"

#include <QList>

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
Q_DECLARE_METATYPE(Vector3)

class test_DrawerText: public QObject {
Q_OBJECT

private slots:
    void getDistanceToPoint_ShouldReturnExpectedDistanceAndClosestPoint_data();
    void getDistanceToPoint_ShouldReturnExpectedDistanceAndClosestPoint();
};

void test_DrawerText::getDistanceToPoint_ShouldReturnExpectedDistanceAndClosestPoint_data()
{
    QTest::addColumn<QList<double> >("bounds");
    QTest::addColumn<Vector3>("pointToCheck");
    QTest::addColumn<double>("expectedDistance");
    QTest::addColumn<Vector3>("expectedClosestPoint");

    QList<double> bounds;
    bounds << 0.0 << 2.0 << 0.2 << 1.0 << -0.15 << 1.0;
    Vector3 pointInsideBounds(0.5, 0.5, 0.5);
    Vector3 pointOutsideBoundsInX(2.4f, 0.5f, 0.5f);
    Vector3 pointOutsideBoundsInXNegative(-2.4f, 0.5f, 0.5f);
    Vector3 pointOutsideBoundsInY(0.5f, 2.4f, 0.5f);
    Vector3 pointOutsideBoundsInYNegative(0.5f, -2.4f, 0.5f);
    Vector3 pointOutsideBoundsInZ(0.5f, 0.5f, 2.4f);
    Vector3 pointOutsideBoundsInZNegative(0.5f, 0.5f, -2.4f);

    QList<double> boundsSameXminXmax;
    boundsSameXminXmax << 1.234 << 1.234 << 0.0 << 1.0 << 0.0 << 1.0;
    Vector3 pointInsideBoundsSameXminXmax(1.234f, 0.2f, 0.2f);

    QList<double> boundsSameYminYmax;
    boundsSameYminYmax << -12.0 << -5.0 << -5.554 << -5.554 << 0.0 << 1.0;
    Vector3 pointInsideBoundsSameYminYmax(-11.666f, -5.554f, 0.8f);

    QList<double> boundsSameZminZmax;
    boundsSameZminZmax << -4.0 << 15.0 << 2.453 << 2.876 << 4.4443 << 4.4443;
    Vector3 pointInsideBoundsSameZminZmax(1.666f, 2.554f, 4.4443f);

    QTest::newRow("all min and max different, point inside") << bounds << pointInsideBounds << 0.0 << pointInsideBounds;
    QTest::newRow("same Xmin and Xmax, point inside") << boundsSameXminXmax << pointInsideBoundsSameXminXmax << 0.0 << pointInsideBoundsSameXminXmax;
    QTest::newRow("same Ymin and Ymax, point inside") << boundsSameYminYmax << pointInsideBoundsSameYminYmax << 0.0 << pointInsideBoundsSameYminYmax;
    QTest::newRow("same Zmin and Zmax, point inside") << boundsSameZminZmax << pointInsideBoundsSameZminZmax << 0.0 << pointInsideBoundsSameZminZmax;

    QTest::newRow("point outside in X") << bounds << pointOutsideBoundsInX << 0.4 << Vector3(2.0f, 0.5f, 0.5f);
    QTest::newRow("point outside in Y") << bounds << pointOutsideBoundsInY << 1.4 << Vector3(0.5f, 1.0f, 0.5f);
    QTest::newRow("point outside in Z") << bounds << pointOutsideBoundsInZ << 1.4 << Vector3(0.5f, 0.5f, 1.0f);
    QTest::newRow("point outside in X negative") << bounds << pointOutsideBoundsInXNegative << 2.4 << Vector3(0.0f, 0.5f, 0.5f);
    QTest::newRow("point outside in Y negative") << bounds << pointOutsideBoundsInYNegative << 2.6 << Vector3(0.5f, 0.2f, 0.5f);
    QTest::newRow("point outside in Z negative") << bounds << pointOutsideBoundsInZNegative << 2.25 << Vector3(0.5f, 0.5f, -0.15f);

}

void test_DrawerText::getDistanceToPoint_ShouldReturnExpectedDistanceAndClosestPoint()
{
    QFETCH(QList<double>, bounds);
    QFETCH(Vector3, pointToCheck);
    QFETCH(double, expectedDistance);
    QFETCH(Vector3, expectedClosestPoint);

    TestingDrawerText drawerText;
    drawerText.setBounds(bounds);

    Vector3 closestPoint;
    double distance = drawerText.getDistanceToPoint(pointToCheck, closestPoint);

    QString distanceError(QString("Compared values are not the same\nActual: %1\nExpected: %2").arg(distance).arg(expectedDistance));
    QString closestPointError(QString("Compared values are not the same\nActual: %1\nExpected: %2").arg(closestPoint).arg(expectedClosestPoint));

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(distance, expectedDistance, 0.001), qPrintable(distanceError));
    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(closestPoint, expectedClosestPoint, 0.001), qPrintable(closestPointError));
}

DECLARE_TEST(test_DrawerText)

#include "test_drawertext.moc"

