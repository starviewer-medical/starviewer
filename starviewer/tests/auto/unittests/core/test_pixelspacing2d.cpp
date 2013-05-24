#include "autotest.h"

#include "pixelspacing2d.h"
#include "mathtools.h"

using namespace udg;

class test_PixelSpacing2D : public QObject {
Q_OBJECT

private slots:
    void isValid_ReturnsExpectedValues_data();
    void isValid_ReturnsExpectedValues();

    void isEqual_ReturnsExpectedValues_data();
    void isEqual_ReturnsExpectedValues();
};

Q_DECLARE_METATYPE(PixelSpacing2D)

void test_PixelSpacing2D::isValid_ReturnsExpectedValues_data()
{
    QTest::addColumn<double>("x");
    QTest::addColumn<double>("y");
    QTest::addColumn<bool>("expectedValue");

    double randomNegativeNumber = MathTools::randomDouble(-RAND_MAX, 0.0);
    double randomNumber = MathTools::randomDouble(-RAND_MAX, RAND_MAX);
    
    QTest::newRow("x < 0, random y : invalid spacing") << randomNegativeNumber << randomNumber << false;
    QTest::newRow("x == 0.0, random y : invalid spacing") << 0.0 << randomNumber << false;
    QTest::newRow("random x, y < 0 : invalid spacing") << randomNumber << randomNegativeNumber << false;
    QTest::newRow("random x, y == 0.0 : invalid spacing") << randomNumber << 0.0 << false;
    QTest::newRow("x && y == 0.0 : invalid spacing") << 0.0 << 0.0 << false;

    double randomPositiveNumberGreaterThanZero = MathTools::randomDouble(0.1, RAND_MAX);
    QTest::newRow("x && y random positive number greater than zero : valid spacing") << randomPositiveNumberGreaterThanZero << randomPositiveNumberGreaterThanZero << true;
}

void test_PixelSpacing2D::isValid_ReturnsExpectedValues()
{
    QFETCH(double, x);
    QFETCH(double, y);
    QFETCH(bool, expectedValue);

    PixelSpacing2D spacing(x, y);
    QCOMPARE(spacing.isValid(), expectedValue);
}

void test_PixelSpacing2D::isEqual_ReturnsExpectedValues_data()
{
    QTest::addColumn<PixelSpacing2D>("spacing1");
    QTest::addColumn<PixelSpacing2D>("spacing2");
    QTest::addColumn<bool>("expectedResult");

    QTest::newRow("uninitialized objects are equal") << PixelSpacing2D() << PixelSpacing2D() << true;
    
    double randomValue1 = MathTools::randomDouble(-RAND_MAX, RAND_MAX);
    double randomValue2 = MathTools::randomDouble(-RAND_MAX, RAND_MAX);
    
    PixelSpacing2D spacing1(randomValue1, randomValue2);
    QTest::newRow("same compared object is equal") << spacing1 << spacing1 << true;

    PixelSpacing2D spacing2(randomValue2, randomValue1);
    QTest::newRow("diferent objects with different values are not equal") << spacing1 << spacing2 << false;

    QTest::newRow("different objects with same values are equal") << PixelSpacing2D(randomValue1, randomValue2) << PixelSpacing2D(randomValue1, randomValue2) << true;
}

void test_PixelSpacing2D::isEqual_ReturnsExpectedValues()
{
    QFETCH(PixelSpacing2D, spacing1);
    QFETCH(PixelSpacing2D, spacing2);
    QFETCH(bool, expectedResult);

    QCOMPARE(spacing1.isEqual(spacing2), expectedResult);
}

DECLARE_TEST(test_PixelSpacing2D)

#include "test_pixelspacing2d.moc"

