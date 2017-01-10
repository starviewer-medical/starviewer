#include "autotest.h"

#include "drawertext.h"
#include "mathtools.h"
#include "fuzzycomparetesthelper.h"

#include <QList>

using namespace udg;
using namespace testing;

using DisplayBounds = std::array<double, 4>;

class TestingDrawerText : public DrawerText {
public:
    DisplayBounds m_displayBounds;

public:
    virtual DisplayBounds getDisplayBounds(std::function<Vector3(const Vector3&)>) override
    {
        return m_displayBounds;
    }

    void setDisplayBounds(DisplayBounds bounds)
    {
        m_displayBounds = std::move(bounds);
    }
};

Q_DECLARE_METATYPE(DisplayBounds)
Q_DECLARE_METATYPE(Vector3)

class test_DrawerText: public QObject {
Q_OBJECT

private slots:
    void getDistanceToPointInDisplay_ShouldReturnExpectedDistanceAndClosestPoint_data();
    void getDistanceToPointInDisplay_ShouldReturnExpectedDistanceAndClosestPoint();
};

void test_DrawerText::getDistanceToPointInDisplay_ShouldReturnExpectedDistanceAndClosestPoint_data()
{
    QTest::addColumn<DisplayBounds>("bounds");
    QTest::addColumn<Vector3>("pointToCheck");
    QTest::addColumn<double>("expectedDistance");
    QTest::addColumn<Vector3>("expectedClosestPoint");

    DisplayBounds bounds{{0.0, 2.0, 0.2, 1.0}};
    Vector3 pointInsideBounds(0.5, 0.5, 0);
    Vector3 pointOutsideBoundsInX(2.4f, 0.5f, 0);
    Vector3 pointOutsideBoundsInXNegative(-2.4f, 0.5f, 0);
    Vector3 pointOutsideBoundsInY(0.5f, 2.4f, 0);
    Vector3 pointOutsideBoundsInYNegative(0.5f, -2.4f, 0);

    DisplayBounds boundsSameXminXmax{{1.234, 1.234, 0.0, 1.0}};
    Vector3 pointInsideBoundsSameXminXmax(1.234f, 0.2f, 0);

    DisplayBounds boundsSameYminYmax{{-12.0, -5.0, -5.554, -5.554}};
    Vector3 pointInsideBoundsSameYminYmax(-11.666f, -5.554f, 0);

    QTest::newRow("all min and max different, point inside") << bounds << pointInsideBounds << 0.0 << pointInsideBounds;
    QTest::newRow("same Xmin and Xmax, point inside") << boundsSameXminXmax << pointInsideBoundsSameXminXmax << 0.0 << pointInsideBoundsSameXminXmax;
    QTest::newRow("same Ymin and Ymax, point inside") << boundsSameYminYmax << pointInsideBoundsSameYminYmax << 0.0 << pointInsideBoundsSameYminYmax;

    QTest::newRow("point outside in X") << bounds << pointOutsideBoundsInX << 0.4 << Vector3(2.0f, 0.5f, 0);
    QTest::newRow("point outside in Y") << bounds << pointOutsideBoundsInY << 1.4 << Vector3(0.5f, 1.0f, 0);
    QTest::newRow("point outside in X negative") << bounds << pointOutsideBoundsInXNegative << 2.4 << Vector3(0.0f, 0.5f, 0);
    QTest::newRow("point outside in Y negative") << bounds << pointOutsideBoundsInYNegative << 2.6 << Vector3(0.5f, 0.2f, 0);
}

void test_DrawerText::getDistanceToPointInDisplay_ShouldReturnExpectedDistanceAndClosestPoint()
{
    QFETCH(DisplayBounds, bounds);
    QFETCH(Vector3, pointToCheck);
    QFETCH(double, expectedDistance);
    QFETCH(Vector3, expectedClosestPoint);

    TestingDrawerText drawerText;
    drawerText.setDisplayBounds(bounds);

    Vector3 closestPoint;
    double distance = drawerText.getDistanceToPointInDisplay(pointToCheck, closestPoint, [](const Vector3 &v) { return v; });

    QString distanceError(QString("Compared values are not the same\nActual: %1\nExpected: %2").arg(distance).arg(expectedDistance));
    QString closestPointError(QString("Compared values are not the same\nActual: %1\nExpected: %2").arg(closestPoint).arg(expectedClosestPoint));

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(distance, expectedDistance, 0.001), qPrintable(distanceError));
    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(closestPoint, expectedClosestPoint, 0.001), qPrintable(closestPointError));
}

DECLARE_TEST(test_DrawerText)

#include "test_drawertext.moc"

