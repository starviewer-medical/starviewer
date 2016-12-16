#include "autotest.h"

#include <bitset>
#include <cstring>
#include <limits>

#include <QVector2D>
#include <QVector3D>

#include "mathtools.h"
#include "fuzzycomparetesthelper.h"

using namespace udg;
using namespace testing;

class test_MathTools : public QObject {
Q_OBJECT

private slots:
    void getBoundedValue_ReturnsExpectedValue_data();
    void getBoundedValue_ReturnsExpectedValue();
    
    void truncate_ShouldReturnTruncatedValue_data();
    void truncate_ShouldReturnTruncatedValue();

    void roundToNearestInteger_ShouldReturnRoundedValue_data();
    void roundToNearestInteger_ShouldReturnRoundedValue();

    void degreesToRadians_ShouldReturnExpectedValue_data();
    void degreesToRadians_ShouldReturnExpectedValue();

    void radiansToDegrees_ShouldReturnExpectedValue_data();
    void radiansToDegrees_ShouldReturnExpectedValue();

    void logTwo_WithDefaultSecondParameterShouldReturnExpectedValue_data();
    void logTwo_WithDefaultSecondParameterShouldReturnExpectedValue();

    void logTwo_WithExplicitSecondParameterShouldReturnExpectedValue_data();
    void logTwo_WithExplicitSecondParameterShouldReturnExpectedValue();

    void logTwo_WithDefaultSecondParameterShouldReturnNaN_data();
    void logTwo_WithDefaultSecondParameterShouldReturnNaN();

    void logTwo_WithExplicitSecondParameterShouldReturnNaN_data();
    void logTwo_WithExplicitSecondParameterShouldReturnNaN();

    void cubeRoot_ShouldReturnExpectedValue_data();
    void cubeRoot_ShouldReturnExpectedValue();

    void cubeRoot_ShouldReturnNaN_data();
    void cubeRoot_ShouldReturnNaN();

    void closeEnough_ShouldReturnExpectedValue_data();
    void closeEnough_ShouldReturnExpectedValue();

    void angleInRadians_QVector2D_ShouldComputeAngleInRadians_data();
    void angleInRadians_QVector2D_ShouldComputeAngleInRadians();

    void angleInDegrees_QVector2D_ShouldComputeAngleInDegrees_data();
    void angleInDegrees_QVector2D_ShouldComputeAngleInDegrees();

    void angleInRadians_QVector3D_QVector3D_ShouldComputeAngleInRadians_data();
    void angleInRadians_QVector3D_QVector3D_ShouldComputeAngleInRadians();

    void angleInRadians_QVector3D_QVector3D_ShouldReturnNaN_data();
    void angleInRadians_QVector3D_QVector3D_ShouldReturnNaN();

    void angleInDegrees_QVector3D_QVector3D_ShouldComputeAngleInDegrees_data();
    void angleInDegrees_QVector3D_QVector3D_ShouldComputeAngleInDegrees();

    void angleInDegrees_QVector3D_QVector3D_ShouldReturnNaN_data();
    void angleInDegrees_QVector3D_QVector3D_ShouldReturnNaN();

    void normalize_ShouldReturnExpectedValues_data();
    void normalize_ShouldReturnExpectedValues();

    void modulus_ShouldReturnExpectedValues_data();
    void modulus_ShouldReturnExpectedValues();

    void dotProduct_ShouldReturnExpectedValues_data();
    void dotProduct_ShouldReturnExpectedValues();

    void crossProduct_ShouldReturnExpectedValues_data();
    void crossProduct_ShouldReturnExpectedValues();

    void directorVector_ShouldReturnExpectedValues_data();
    void directorVector_ShouldReturnExpectedValues();

    void getDistance3D_ShouldComputeDistanceCorrectly_data();
    void getDistance3D_ShouldComputeDistanceCorrectly();

    void infiniteLinesIntersection_ShouldComputeExpectedIntersectionAndState_data();
    void infiniteLinesIntersection_ShouldComputeExpectedIntersectionAndState();

    void planeIntersection_2Planes_ShouldReturnNoIntersection_data();
    void planeIntersection_2Planes_ShouldReturnNoIntersection();

    void planeIntersection_2Planes_ShouldComputeExpectedIntersection_data();
    void planeIntersection_2Planes_ShouldComputeExpectedIntersection();

    void planeIntersection_2Planes_ShouldComputeIntersectionWithNaNPointAndNullDirectorVector_data();
    void planeIntersection_2Planes_ShouldComputeIntersectionWithNaNPointAndNullDirectorVector();

    void roundUpToPowerOf2_ShouldReturnExpectedValue_data();
    void roundUpToPowerOf2_ShouldReturnExpectedValue();

    void roundUpToMultipleOfNumber_ShouldReturnExpectedValue_data();
    void roundUpToMultipleOfNumber_ShouldReturnExpectedValue();

    void getPointToClosestEdgeDistance_ShouldReturnExpectedValues_data();
    void getPointToClosestEdgeDistance_ShouldReturnExpectedValues();

    void getPointToFiniteLineDistance_ShouldReturnExpectedValues_data();
    void getPointToFiniteLineDistance_ShouldReturnExpectedValues();

    void isNaN_ShouldReturnExpectedValue_data();
    void isNaN_ShouldReturnExpectedValue();

    void copySign_ShouldReturnExpectedValue_data();
    void copySign_ShouldReturnExpectedValue();

    void copySign_ShouldReturnZeroWithExpectedSign_data();
    void copySign_ShouldReturnZeroWithExpectedSign();

    void copySign_ShouldReturnNaNWithExpectedSign_data();
    void copySign_ShouldReturnNaNWithExpectedSign();

private:
    void setupRadiansAndDegreesConversionData();
    void setupComputeAngleOfAVectorData();
    void setupComputeAngleBetweenTwoVectorsData();
    void setupComputeAngleBetweenTwoVectorsNaNData();
    void setupCrossAndDotProductData();
};

Q_DECLARE_METATYPE(MathTools::IntersectionResults)
Q_DECLARE_METATYPE(QList<Vector3>)
Q_DECLARE_METATYPE(Vector3)

// Epsilons pels diferents fuzzyCompare de cada test
const double AngleInDegreesEpsilon = 0.001;
const double AngleInRadiansEpsilon = 0.001;
const double NormalizeEpsilon = 0.000001;
const double ModulusEpsilon = 0.001;
const double crossProductEpsilon = 0.001;
const double directorVectorEpsilon = 0.001;
const double distance3DEpsion = 0.0005;
const double LineIntersectionEpsilon = 0.0001;

void test_MathTools::getBoundedValue_ReturnsExpectedValue_data()
{
    QTest::addColumn<int>("value");
    QTest::addColumn<int>("min");
    QTest::addColumn<int>("max");
    QTest::addColumn<bool>("loop");
    QTest::addColumn<int>("expectedBoundedValue");

    int minimum = 0;
    int maximum = 255;
    int valueInBounds = MathTools::randomInt(minimum, maximum);
    
    bool dummyLoop = MathTools::randomInt(0, 1);
    QTest::newRow("Value is in bounds, does not care about loop") << valueInBounds << minimum << maximum << dummyLoop << valueInBounds;
    
    int valueBelowMinBound = MathTools::randomInt(std::numeric_limits<int>::min(), minimum - 1);
    QTest::newRow("Value below min bound, loop == false") << valueBelowMinBound << minimum << maximum << false << minimum;
    QTest::newRow("Value below min bound, loop == true") << valueBelowMinBound << minimum << maximum << true << maximum;

    int valueAboveMaxBound = MathTools::randomInt(maximum + 1, std::numeric_limits<int>::max());
    QTest::newRow("Value above max bound, loop == false") << valueAboveMaxBound << minimum << maximum << false << maximum;
    QTest::newRow("Value above max bound, loop == true") << valueAboveMaxBound << minimum << maximum << true << minimum;
}

void test_MathTools::getBoundedValue_ReturnsExpectedValue()
{
    QFETCH(int, value);
    QFETCH(int, min);
    QFETCH(int, max);
    QFETCH(bool, loop);
    QFETCH(int, expectedBoundedValue);
    
    QCOMPARE(MathTools::getBoundedValue(value, min, max, loop), expectedBoundedValue);
}

void test_MathTools::truncate_ShouldReturnTruncatedValue_data()
{
    QTest::addColumn<double>("value");
    QTest::addColumn<double>("truncated");

    QTest::newRow("negative") << -32.14 << -32.0;
    QTest::newRow("positive") << 77.97 << 77.0;
    QTest::newRow("+zero") << 0.0 << 0.0;
    QTest::newRow("-zero") << -0.0 << -0.0;
}

void test_MathTools::truncate_ShouldReturnTruncatedValue()
{
    QFETCH(double, value);
    QFETCH(double, truncated);

    QCOMPARE(MathTools::truncate(value), truncated);
}

void test_MathTools::roundToNearestInteger_ShouldReturnRoundedValue_data()
{
    QTest::addColumn<double>("value");
    QTest::addColumn<int>("rounded");

    QTest::newRow("negative < .5") << -28.48 << -28;
    QTest::newRow("positive < .5") << 92.22 << 92;
    QTest::newRow("negative > .5") << -15.99 << -16;
    QTest::newRow("positive > .5") << 34.57 << 35;
    // Quan és .5 s'han de comprovar diferents casos (http://en.wikipedia.org/wiki/IEEE_floating_point#Roundings_to_nearest)
    QTest::newRow("negative odd .5") << -23.5 << -24;
    QTest::newRow("negative even .5") << -22.5 << -23;
    QTest::newRow("positive odd .5") << 23.5 << 24;
    QTest::newRow("positive even .5") << 22.5 << 23;
    QTest::newRow("+zero") << 0.0 << 0;
    QTest::newRow("-zero") << -0.0 << 0;
}

void test_MathTools::roundToNearestInteger_ShouldReturnRoundedValue()
{
    QFETCH(double, value);
    QFETCH(int, rounded);

    QCOMPARE(MathTools::roundToNearestInteger(value), rounded);
}

void test_MathTools::degreesToRadians_ShouldReturnExpectedValue_data()
{
    setupRadiansAndDegreesConversionData();
}

void test_MathTools::degreesToRadians_ShouldReturnExpectedValue()
{
    QFETCH(float, degrees);
    QFETCH(float, radians);

    float result = MathTools::degreesToRadians(degrees);

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, radians),
             qPrintable(QString("actual: %1, expected: %2, difference: %3").arg(result).arg(radians).arg(result - radians)));
}

void test_MathTools::radiansToDegrees_ShouldReturnExpectedValue_data()
{
    setupRadiansAndDegreesConversionData();
}

void test_MathTools::radiansToDegrees_ShouldReturnExpectedValue()
{
    QFETCH(float, radians);
    QFETCH(float, degrees);

    float result = MathTools::radiansToDegrees(radians);

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, degrees),
             qPrintable(QString("actual: %1, expected: %2, difference: %3").arg(result).arg(degrees).arg(result - degrees)));
}

void test_MathTools::logTwo_WithDefaultSecondParameterShouldReturnExpectedValue_data()
{
    QTest::addColumn<double>("x");
    QTest::addColumn<double>("expectedResult");

    QTest::newRow("0") << 0.0 << 0.0;
    QTest::newRow("< 1") << 0.8442696986 << -0.2442241591313752;
    QTest::newRow("1") << 1.0 << 0.0;
    QTest::newRow("2") << 2.0 << 1.0;
    QTest::newRow("e") << 2.718281828459045 << 1.4426950408889634;
    QTest::newRow("random") << 45.2426428941 << 5.499611303146188;
}

void test_MathTools::logTwo_WithDefaultSecondParameterShouldReturnExpectedValue()
{
    QFETCH(double, x);
    QFETCH(double, expectedResult);

    double result = MathTools::logTwo(x);

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected: %2, difference: %3").arg(result).arg(expectedResult).arg(result - expectedResult)));
}

void test_MathTools::logTwo_WithExplicitSecondParameterShouldReturnExpectedValue_data()
{
    QTest::addColumn<double>("x");
    QTest::addColumn<bool>("zero");
    QTest::addColumn<double>("expectedResult");

    QTest::newRow("0 false") << 0.0 << false << -std::numeric_limits<double>::infinity();
    QTest::newRow("0 true") << 0.0 << true << 0.0;
    QTest::newRow("< 1 false") << 0.8442696986 << false << -0.2442241591313752;
    QTest::newRow("1 true") << 1.0 << true << 0.0;
    QTest::newRow("2 false") << 2.0 << false << 1.0;
    QTest::newRow("e true") << 2.718281828459045 << true << 1.4426950408889634;
    QTest::newRow("random false") << 45.2426428941 << false << 5.499611303146188;
}

void test_MathTools::logTwo_WithExplicitSecondParameterShouldReturnExpectedValue()
{
    QFETCH(double, x);
    QFETCH(bool, zero);
    QFETCH(double, expectedResult);

    double result = MathTools::logTwo(x, zero);

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected: %2, difference: %3").arg(result).arg(expectedResult).arg(result - expectedResult)));
}

void test_MathTools::logTwo_WithDefaultSecondParameterShouldReturnNaN_data()
{
    QTest::addColumn<double>("x");

    QTest::newRow("-1") << -1.0;
    QTest::newRow("random") << -45.2426428941;
}

void test_MathTools::logTwo_WithDefaultSecondParameterShouldReturnNaN()
{
    QFETCH(double, x);

    double result = MathTools::logTwo(x);

    QVERIFY2(MathTools::isNaN(result), qPrintable(QString("actual: %1, expected: nan").arg(result)));
}

void test_MathTools::logTwo_WithExplicitSecondParameterShouldReturnNaN_data()
{
    QTest::addColumn<double>("x");
    QTest::addColumn<bool>("zero");

    QTest::newRow("-1 false") << -1.0 << false;
    QTest::newRow("random true") << -45.2426428941 << true;
}

void test_MathTools::logTwo_WithExplicitSecondParameterShouldReturnNaN()
{
    QFETCH(double, x);
    QFETCH(bool, zero);

    double result = MathTools::logTwo(x, zero);

    QVERIFY2(MathTools::isNaN(result), qPrintable(QString("actual: %1, expected: nan").arg(result)));
}

void test_MathTools::cubeRoot_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<double>("x");
    QTest::addColumn<double>("expectedResult");

    QTest::newRow("0") << 0.0 << 0.0;
    QTest::newRow("-0") << -0.0 << 0.0;
    QTest::newRow("1") << 1.0 << 1.0;
    QTest::newRow("8") << 8.0 << 2.0;
    QTest::newRow("random #1") << 70.71 << 4.135172323084713;
    QTest::newRow("random #2") << 63.62 << 3.9920676130411104;
}

void test_MathTools::cubeRoot_ShouldReturnExpectedValue()
{
    QFETCH(double, x);
    QFETCH(double, expectedResult);

    double result = MathTools::cubeRoot(x);

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected: %2, difference: %3").arg(result).arg(expectedResult).arg(result - expectedResult)));
}

void test_MathTools::cubeRoot_ShouldReturnNaN_data()
{
    QTest::addColumn<double>("x");

    QTest::newRow("-1") << -1.0;
    QTest::newRow("-8") << -8.0;
    QTest::newRow("random #1") << -67.94;
}

void test_MathTools::cubeRoot_ShouldReturnNaN()
{
    QFETCH(double, x);

    double result = MathTools::cubeRoot(x);

    QVERIFY2(MathTools::isNaN(result), qPrintable(QString("actual: %1, expected: nan").arg(result)));
}

void test_MathTools::closeEnough_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<float>("value1");
    QTest::addColumn<float>("value2");
    QTest::addColumn<bool>("expectedResult");

    float epsilon = MathTools::Epsilon;

    QTest::newRow("equal (positive)") << 36.29f << 36.29f << true;
    QTest::newRow("equal (negative)") << -49.4f << -49.4f << true;
    QTest::newRow("equal (+0)") << 0.0f << 0.0f << true;
    QTest::newRow("equal (-0)") << -0.0f << -0.0f << true;
    QTest::newRow("equal (+0, -0)") << 0.0f << -0.0f << true;
    QTest::newRow("equal (-0, +0)") << -0.0f << 0.0f << true;
    QTest::newRow("close but not (0, 1e-10)") << 0.0f << 1e-10f << false;
    QTest::newRow("close but not (0, -1e-10)") << 0.0f << -1e-10f << false;
    QTest::newRow("close but not (0, float min)") << 0.0f << std::numeric_limits<float>::denorm_min() << false;
    QTest::newRow("close but not (0, -float min)") << 0.0f << -std::numeric_limits<float>::denorm_min() << false;
    QTest::newRow("close (1e-10, 0)") << 1e-10f << 0.0f << true;
    QTest::newRow("close (-1e-10, 0)") << -1e-10f << 0.0f << true;
    QTest::newRow("not close (0, epsilon)") << 0.0f << epsilon << false;
    QTest::newRow("not close (0, -epsilon)") << 0.0f << -epsilon << false;
    QTest::newRow("not close but yes (epsilon, 0)") << epsilon << 0.0f << true;
    QTest::newRow("not close but yes (-epsilon, 0)") << epsilon << 0.0f << true;
    QTest::newRow("very different") << -10.96f << -22.0f << false;
    QTest::newRow("equal (+inf)") << std::numeric_limits<float>::infinity() << std::numeric_limits<float>::infinity() << false;
    QTest::newRow("equal (-inf)") << -std::numeric_limits<float>::infinity() << -std::numeric_limits<float>::infinity() << false;
    QTest::newRow("different (+inf, -inf)") << std::numeric_limits<float>::infinity() << -std::numeric_limits<float>::infinity() << false;
    QTest::newRow("different (-inf, +inf)") << -std::numeric_limits<float>::infinity() << +std::numeric_limits<float>::infinity() << false;
}

void test_MathTools::closeEnough_ShouldReturnExpectedValue()
{
    QFETCH(float, value1);
    QFETCH(float, value2);
    QFETCH(bool, expectedResult);

    QCOMPARE(MathTools::closeEnough(value1, value2), expectedResult);
}

void test_MathTools::angleInRadians_QVector2D_ShouldComputeAngleInRadians_data()
{
    this->setupComputeAngleOfAVectorData();
}

void test_MathTools::angleInRadians_QVector2D_ShouldComputeAngleInRadians()
{
    QFETCH(QVector2D, vector);
    QFETCH(double, angleInRadians);

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(MathTools::angleInRadians(vector), angleInRadians, AngleInRadiansEpsilon),
             qPrintable(QString("actual = %1, expected = %2").arg(MathTools::angleInRadians(vector)).arg(angleInRadians)));
}

void test_MathTools::angleInDegrees_QVector2D_ShouldComputeAngleInDegrees_data()
{
    this->setupComputeAngleOfAVectorData();
}

void test_MathTools::angleInDegrees_QVector2D_ShouldComputeAngleInDegrees()
{
    QFETCH(QVector2D, vector);
    QFETCH(double, angleInRadians);

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(MathTools::angleInDegrees(vector),
                                                  angleInRadians * MathTools::RadiansToDegreesAsDouble,
                                                  AngleInDegreesEpsilon),
             qPrintable(QString("actual = %1, expected = %2").arg(MathTools::angleInDegrees(vector))
                                                             .arg(angleInRadians * MathTools::RadiansToDegreesAsDouble)));
}

void test_MathTools::angleInRadians_QVector3D_QVector3D_ShouldComputeAngleInRadians_data()
{
    this->setupComputeAngleBetweenTwoVectorsData();
}

void test_MathTools::angleInRadians_QVector3D_QVector3D_ShouldComputeAngleInRadians()
{
    QFETCH(QVector3D, vector1);
    QFETCH(QVector3D, vector2);
    QFETCH(double, angleInRadians);

    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(MathTools::angleInRadians(vector1, vector2), angleInRadians, AngleInRadiansEpsilon));
}

void test_MathTools::angleInDegrees_QVector3D_QVector3D_ShouldComputeAngleInDegrees_data()
{
    this->setupComputeAngleBetweenTwoVectorsData();
}

void test_MathTools::angleInDegrees_QVector3D_QVector3D_ShouldComputeAngleInDegrees()
{
    QFETCH(QVector3D, vector1);
    QFETCH(QVector3D, vector2);
    QFETCH(double, angleInRadians);

    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(MathTools::angleInDegrees(vector1, vector2), angleInRadians * MathTools::RadiansToDegreesAsDouble, AngleInDegreesEpsilon));
}

void test_MathTools::angleInRadians_QVector3D_QVector3D_ShouldReturnNaN_data()
{
    this->setupComputeAngleBetweenTwoVectorsNaNData();
}

void test_MathTools::angleInRadians_QVector3D_QVector3D_ShouldReturnNaN()
{
    QFETCH(QVector3D, vector1);
    QFETCH(QVector3D, vector2);

    QVERIFY(MathTools::isNaN(MathTools::angleInRadians(vector1, vector2)));
}

void test_MathTools::angleInDegrees_QVector3D_QVector3D_ShouldReturnNaN_data()
{
    this->setupComputeAngleBetweenTwoVectorsNaNData();
}

void test_MathTools::angleInDegrees_QVector3D_QVector3D_ShouldReturnNaN()
{
    QFETCH(QVector3D, vector1);
    QFETCH(QVector3D, vector2);

    QVERIFY(MathTools::isNaN(MathTools::angleInDegrees(vector1, vector2)));
}

void test_MathTools::normalize_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<QVector3D>("vector");
    QTest::addColumn<QVector3D>("result");

    QTest::newRow("X vector") << QVector3D(1, 0, 0) << QVector3D(1, 0, 0);
    QTest::newRow("X vector (2)") << QVector3D(22, 0, 0) << QVector3D(1, 0, 0);
    QTest::newRow("Negative X vector ") << QVector3D(-1.36, 0, 0) << QVector3D(-1, 0, 0);

    QTest::newRow("Y vector") << QVector3D(0, 1, 0) << QVector3D(0, 1, 0);
    QTest::newRow("Y vector (2)") << QVector3D(0, 22, 0) << QVector3D(0, 1, 0);
    QTest::newRow("Negative Y vector ") << QVector3D(0, -1.36, 0) << QVector3D(0, -1, 0);

    QTest::newRow("Z vector") << QVector3D(0, 0, 1) << QVector3D(0, 0, 1);
    QTest::newRow("Z vector (2)") << QVector3D(0, 0, 234) << QVector3D(0, 0, 1);
    QTest::newRow("Negative Z vector ") << QVector3D(0, 0, -1.36) << QVector3D(0, 0, -1);

    QTest::newRow("XY vector") << QVector3D(1, 1, 0) << QVector3D(0.707107, 0.707107, 0);
    QTest::newRow("XY vector (2)") << QVector3D(2, 2, 0) << QVector3D(0.707107, 0.707107, 0);
    QTest::newRow("XY vector (3)") << QVector3D(536, 33, 0) << QVector3D(0.99811, 0.0614508, 0);

    QTest::newRow("random vector (0)") << QVector3D(-45.25, 1.236, 0.002) << QVector3D(-0.999627, 0.027304, 0.000044);
    QTest::newRow("random vector (1)") << QVector3D(-200, -115.60200500488, 26.844478607177) 
                                            << QVector3D(-0.8599907667709096, -0.49708328462200635, 0.11543001870475712);
    QTest::newRow("random vector (2)") << QVector3D(-80, -44, 953.900024) 
                                            << QVector3D(-0.08348469343677656, -0.04591658139022711, 0.9954506384121726);
}

void test_MathTools::normalize_ShouldReturnExpectedValues()
{ 
    QFETCH(QVector3D, vector);
    QFETCH(QVector3D, result);

    double cVector[3] = { vector.x(), vector.y(), vector.z() };
    MathTools::normalize(cVector);
    
    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(cVector[0], result.x(), NormalizeEpsilon));
    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(cVector[1], result.y(), NormalizeEpsilon));
    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(cVector[2], result.z(), NormalizeEpsilon));
}

void test_MathTools::modulus_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<QVector3D>("vector");
    QTest::addColumn<double>("modulus");

    QTest::newRow("0-valued vector") << QVector3D(0, 0, 0) << 0.;
    QTest::newRow("X normalized vector") << QVector3D(1, 0, 0) << 1.;
    QTest::newRow("Y normalized vector") << QVector3D(0, 1, 0) << 1.;
    QTest::newRow("Z normalized vector") << QVector3D(0, 0, 1) << 1.;
    QTest::newRow("-X normalized vector") << QVector3D(-1, 0, 0) << 1.;
    QTest::newRow("-Y normalized vector") << QVector3D(0, -1, 0) << 1.;
    QTest::newRow("-Z normalized vector") << QVector3D(0, 0, -1) << 1.;

    QTest::newRow("X vector") << QVector3D(22, 0, 0) << 22.;
    QTest::newRow("Y vector") << QVector3D(0, 57.3, 0) << 57.3;
    QTest::newRow("Z vector") << QVector3D(0, 0, 12.5) << 12.5;
    QTest::newRow("-X vector") << QVector3D(-101.35687, 0, 0) << 101.35687;
    QTest::newRow("-Y vector") << QVector3D(0, -9.2356, 0) << 9.2356;
    QTest::newRow("-Z vector") << QVector3D(0, 0, -45613) << 45613.;

    QTest::newRow("Random XY vector") << QVector3D(22, -15, 0) << 26.627053911388696;
    QTest::newRow("Random XZ vector") << QVector3D(-4529.36, 0, 872.548) << 4612.639376962392;
    QTest::newRow("Random YZ vector") << QVector3D(0, 6.36, 24.58) << 25.389486012914873;
    QTest::newRow("Random XYZ vector") << QVector3D(758.236, -6.36, -24.58) << 758.6609636036377;
}

void test_MathTools::modulus_ShouldReturnExpectedValues()
{
    QFETCH(QVector3D, vector);
    QFETCH(double, modulus);

    double cVector[3] = { vector.x(), vector.y(), vector.z() };

    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(MathTools::modulus(cVector), modulus, ModulusEpsilon));
}

void test_MathTools::dotProduct_ShouldReturnExpectedValues_data()
{
    this->setupCrossAndDotProductData();
}

void test_MathTools::dotProduct_ShouldReturnExpectedValues()
{
    QFETCH(QVector3D, vector1);
    QFETCH(QVector3D, vector2);
    QFETCH(double, dotProduct);

    double cVector1[3] = { vector1.x(), vector1.y(), vector1.z() };
    double cVector2[3] = { vector2.x(), vector2.y(), vector2.z() };

    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(MathTools::dotProduct(cVector1, cVector2), dotProduct));
}

void test_MathTools::crossProduct_ShouldReturnExpectedValues_data()
{
    this->setupCrossAndDotProductData();
}

void test_MathTools::directorVector_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<Vector3>("firstPoint");
    QTest::addColumn<Vector3>("secondPoint");
    QTest::addColumn<Vector3>("result");

    QTest::newRow("random 1") << Vector3(1, 0, 0) << Vector3(1, 0 ,0) << Vector3(0, 0, 0);
    QTest::newRow("random 2") << Vector3(12.3, 0, 0) << Vector3(3.25, 0 ,0) << Vector3(-9.05, 0, 0);
    QTest::newRow("random 3") << Vector3(-71, -24, 1353.5) << Vector3(-71, -24, 1353) << Vector3(0, 0, -0.5);
    QTest::newRow("random 4") << Vector3(143.24654, 0.31235458, 3.235697) << Vector3(54.2358456, 45.524645, 125.5468)
                              << Vector3(-89.0106944, 45.21229042, 122.311103);
    QTest::newRow("axial 1") << Vector3(-36.8385, 0.946872, 1369.72) << Vector3(-27.8487, 64.1007, 1369.72) << Vector3(8.98986, 63.1538, 0);
    QTest::newRow("axial 2") << Vector3(-52.346, 29.4897, 1465.75) << Vector3(16.4264, -18.8308, 1465.75) << Vector3(68.7725, -48.3205, 0);
    QTest::newRow("axial 3") << Vector3(43.6208, 106.803, 1371.21) << Vector3(-27.1744, -0.401608, 1371.21) << Vector3(-70.7952, -107.204, 0);
    QTest::newRow("sagittal 1") << Vector3(19.1385, 82.1874, 1478.69) << Vector3(19.1385, 41.6725, 1403.12) << Vector3(0, -40.5149, -75.5699);
    QTest::newRow("sagittal 2") << Vector3(-47.1244, 98.1202, 1406.31) << Vector3(-47.1244, 52.5979, 1495.99) << Vector3(0, -45.5224, 89.6791);
    QTest::newRow("sagittal 3") << Vector3(-7.76937, 59.8814, 1519.2) << Vector3(-7.76937, 0.247106, 1424.97) << Vector3(0, -59.6343, -94.23);
    QTest::newRow("coronal 1") << Vector3(-56.3126, 81.3314, 1435.44) << Vector3(-22.626, 81.3314, 1493.25) << Vector3(33.6866, 0, 57.81);
    QTest::newRow("coronal 2") << Vector3(67.5083, 3.17039, 1458.2) << Vector3(-9.87976, 3.17039, 1403.57) << Vector3(-77.3881, -9.76996e-15, -54.63);
    QTest::newRow("coronal 3") << Vector3(17.4337, 74.9248, 1526.94) << Vector3(46.1128, 74.9248, 1413.59) << Vector3(28.6791, 0, -113.35);
}

void test_MathTools::directorVector_ShouldReturnExpectedValues()
{
    QFETCH(Vector3, firstPoint);
    QFETCH(Vector3, secondPoint);
    QFETCH(Vector3, result);

    auto directorVector = MathTools::directorVector(firstPoint, secondPoint);

    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(directorVector, result, directorVectorEpsilon));
}

void test_MathTools::crossProduct_ShouldReturnExpectedValues()
{
    QFETCH(QVector3D, vector1);
    QFETCH(QVector3D, vector2);
    QFETCH(QVector3D, crossProduct);

    double cVector1[3] = { vector1.x(), vector1.y(), vector1.z() };
    double cVector2[3] = { vector2.x(), vector2.y(), vector2.z() };

    double computedCrossProduct[3];

    MathTools::crossProduct(cVector1, cVector2, computedCrossProduct);

    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(computedCrossProduct[0], crossProduct.x(), crossProductEpsilon));
    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(computedCrossProduct[1], crossProduct.y(), crossProductEpsilon));
    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(computedCrossProduct[2], crossProduct.z(), crossProductEpsilon));    
}

void test_MathTools::getDistance3D_ShouldComputeDistanceCorrectly_data()
{
    QTest::addColumn<QVector3D>("vector1");
    QTest::addColumn<QVector3D>("vector2");
    QTest::addColumn<double>("distance3D");

    QTest::newRow("Same vectors") << QVector3D(1, 2, 1) << QVector3D(1, 2, 1) << 0.0;
    QTest::newRow("null vectors") << QVector3D(0, 0, 0) << QVector3D(0, 0, 0) << 0.0;
    QTest::newRow("random coronal") << QVector3D(-89.9476, 49.8371, 599.893) << QVector3D(-79.0086, 207.671, 599.893) << 158.213;
    QTest::newRow("random sagital") << QVector3D(7.68421, -11.8899, 695.178) << QVector3D(7.68421, 144.381, 831.915) << 207.648;
    QTest::newRow("random coronal") << QVector3D(-96.9798, 99.4654, 838.166) << QVector3D(-50.0985, 99.4654, 709.242) << 137.183;
    QTest::newRow("random") << QVector3D(35.44, 12.61, -85.28) << QVector3D(92.56, -95.19, 28.85) << 167.060;
}

void test_MathTools::getDistance3D_ShouldComputeDistanceCorrectly()
{
    QFETCH(QVector3D, vector1);
    QFETCH(QVector3D, vector2);
    QFETCH(double, distance3D);

    double v1[3] = { vector1.x(), vector1.y(), vector1.z() };
    double v2[3] = { vector2.x(), vector2.y(), vector2.z() };

    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(MathTools::getDistance3D(v1, v2), distance3D, distance3DEpsion));
}

void test_MathTools::infiniteLinesIntersection_ShouldComputeExpectedIntersectionAndState_data()
{
    QTest::addColumn<Vector3>("point1");
    QTest::addColumn<Vector3>("point2");
    QTest::addColumn<Vector3>("point3");
    QTest::addColumn<Vector3>("point4");
    QTest::addColumn<MathTools::IntersectionResults>("expectedState");
    QTest::addColumn<Vector3>("expectedIntersection");

    QTest::newRow("all points null") << Vector3(0.0, 0.0, 0.0) << Vector3(0.0, 0.0, 0.0) << Vector3(0.0, 0.0, 0.0) << Vector3(0.0, 0.0, 0.0)
                                     << MathTools::ParallelLines << Vector3(0.0, 0.0, 0.0);

    QTest::newRow("p1 == p2") << Vector3(-7.4, +6.0, +0.5) << Vector3(-7.4, +6.0, +0.5) << Vector3(+9.1, +9.6, -6.6) << Vector3(+8.2, +2.2, -2.7)
                              << MathTools::ParallelLines << Vector3(0.0, 0.0, 0.0);
    QTest::newRow("p3 == p4") << Vector3(-7.5, +6.8, +0.3) << Vector3(-3.9, +3.3, -1.3) << Vector3(-3.9, -8.5, +8.0) << Vector3(-3.9, -8.5, +8.0)
                              << MathTools::ParallelLines << Vector3(0.0, 0.0, 0.0);
    QTest::newRow("p1 == p2 && p3 == p4") << Vector3(-5.7, +7.2, -1.6) << Vector3(-5.7, +7.2, -1.6) << Vector3(-1.9, +7.6, +5.8)
                                          << Vector3(-1.9, +7.6, +5.8) << MathTools::ParallelLines << Vector3(0.0, 0.0, 0.0);
    QTest::newRow("p1 == p2 == p3 == p4") << Vector3(+0.8, -6.0, +5.4) << Vector3(+0.8, -6.0, +5.4) << Vector3(+0.8, -6.0, +5.4)
                                          << Vector3(+0.8, -6.0, +5.4) << MathTools::ParallelLines << Vector3(0.0, 0.0, 0.0);

    QTest::newRow("parallel XY") << Vector3(+3.3, -0.5, +2.5) << Vector3(+2.9, -9.8, +2.5) << Vector3(+3.5, +2.3, +2.5) << Vector3(+6.3, +67.4, +2.5)
                                 << MathTools::ParallelLines << Vector3(0.0, 0.0, 0.0);
    QTest::newRow("parallel XZ") << Vector3(+4.6, -7.3, +5.0) << Vector3(+9.6, -7.3, -9.9) << Vector3(+4.2, -7.3, +2.6) << Vector3(+14.2, -7.3, -27.2)
                                 << MathTools::ParallelLines << Vector3(0.0, 0.0, 0.0);
    QTest::newRow("parallel YZ") << Vector3(+2.5, +8.6, -1.8) << Vector3(+2.5, +7.8, -1.3) << Vector3(+2.5, -2.5, -1.0) << Vector3(+2.5, -0.1, -2.5)
                                 << MathTools::ParallelLines << Vector3(0.0, 0.0, 0.0);
    QTest::newRow("parallel") << Vector3(-8.4, -6.2, +2.7) << Vector3(-0.2, -0.1, -1.0) << Vector3(+8.1, +3.9, +9.5) << Vector3(+16.3, +10.0, +5.8)
                              << MathTools::ParallelLines << Vector3(0.0, 0.0, 0.0);

    QTest::newRow("skew XY") << Vector3(-8.5, -0.8, -2.2) << Vector3(+9.2, -7.0, -2.2) << Vector3(+2.4, -9.8, -8.3) << Vector3(+8.3, -6.5, -8.3)
                             << MathTools::LinesIntersect << Vector3(+8.09689, -6.6136, -2.2);
    QTest::newRow("skew XZ") << Vector3(+4.6, -6.4, +0.5) << Vector3(+7.5, -6.4, +5.5) << Vector3(-8.8, +10.0, -8.1) << Vector3(-5.9, +10.0, -1.1)
                             << MathTools::LinesIntersect << Vector3(-29.83, -6.4, -58.8621);
    QTest::newRow("skew YZ") << Vector3(-5.6, +8.9, -8.8) << Vector3(-5.6, -1.5, -6.5) << Vector3(-7.1, +6.6, -2.3) << Vector3(-7.1, -1.7, -7.3)
                             << MathTools::LinesIntersect << Vector3(-5.6, -0.674905, -6.68247);
    QTest::newRow("skew") << Vector3(+7.6, -3.7, +7.9) << Vector3(+8.8, -5.6, -8.9) << Vector3(-1.3, +3.3, -9.7) << Vector3(-4.3, -9.7, +4.4)
                          << MathTools::LinesIntersect << Vector3(+8.27995, -4.77659, -1.6193);

    QTest::newRow("intersection XY") << Vector3(+1.0, -1.6, -10.0) << Vector3(-6.2, -8.2, -10.0) << Vector3(-8.3, -4.4, -10.0)
                                     << Vector3(+0.3, -1.4, -10.0) << MathTools::LinesIntersect << Vector3(+1.78225, -0.882935, -10.0);
    QTest::newRow("intersection XZ") << Vector3(+8.3, +1.6, -4.2) << Vector3(-2.3, +1.6, +9.9) << Vector3(-8.1, +1.6, +8.2)
                                     << Vector3(-2.4, +1.6, -6.1) << MathTools::LinesIntersect << Vector3(-16.0885, +1.6, +28.2413);
    QTest::newRow("intersection YZ") << Vector3(+9.9, +3.3, +7.1) << Vector3(+9.9, +7.6, +5.8) << Vector3(+9.9, -0.2, -3.7)
                                     << Vector3(+9.9, +5.9, -2.5) << MathTools::LinesIntersect << Vector3(+9.9, +23.5616, +0.974408);
    QTest::newRow("intersection") << Vector3(+1.8, -3.9, +6.5) << Vector3(-6.2, +7.3, +8.6) << Vector3(-9.9, +9.9, +4.0)
                                  << Vector3(-340.09, +495.128, +140.259) << MathTools::LinesIntersect << Vector3(-47.0, +64.42, +19.31);
}

void test_MathTools::infiniteLinesIntersection_ShouldComputeExpectedIntersectionAndState()
{
    QFETCH(Vector3, point1);
    QFETCH(Vector3, point2);
    QFETCH(Vector3, point3);
    QFETCH(Vector3, point4);
    QFETCH(MathTools::IntersectionResults, expectedState);
    QFETCH(Vector3, expectedIntersection);

    int state;
    auto intersection = MathTools::infiniteLinesIntersection(point1, point2, point3, point4, state);

    QCOMPARE(state, static_cast<int>(expectedState));
    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(intersection, expectedIntersection, LineIntersectionEpsilon));
}

void test_MathTools::planeIntersection_2Planes_ShouldReturnNoIntersection_data()
{
    QTest::addColumn<Vector3>("plane1Point");
    QTest::addColumn<Vector3>("plane1Normal");
    QTest::addColumn<Vector3>("plane2Point");
    QTest::addColumn<Vector3>("plane2Normal");

    QTest::newRow("same planes (simple)") << Vector3(39.88, -23.21, 33.92) << Vector3(43, 83, 16) << Vector3(39.88, -23.21, 33.92) << Vector3(43, 83, 16);
    QTest::newRow("parallel planes (simple)") << Vector3(39.88, -23.21, 33.92) << Vector3(43, 83, 16) << Vector3(82.85, 73.2, 87.71) << Vector3(43, 83, 16);
}

void test_MathTools::planeIntersection_2Planes_ShouldReturnNoIntersection()
{
    QFETCH(Vector3, plane1Point);
    QFETCH(Vector3, plane1Normal);
    QFETCH(Vector3, plane2Point);
    QFETCH(Vector3, plane2Normal);

    double p1p[3] = { plane1Point.x, plane1Point.y, plane1Point.z };
    double p1n[3] = { plane1Normal.x, plane1Normal.y, plane1Normal.z };
    double p2p[3] = { plane2Point.x, plane2Point.y, plane2Point.z };
    double p2n[3] = { plane2Normal.x, plane2Normal.y, plane2Normal.z };
    double ip[3];
    double idv[3];

    QCOMPARE(MathTools::planeIntersection(p1p, p1n, p2p, p2n, ip, idv), 0);
}

void test_MathTools::planeIntersection_2Planes_ShouldComputeExpectedIntersection_data()
{
    QTest::addColumn<Vector3>("plane1Point");
    QTest::addColumn<Vector3>("plane1Normal");
    QTest::addColumn<Vector3>("plane2Point");
    QTest::addColumn<Vector3>("plane2Normal");
    QTest::addColumn<Vector3>("expectedIntersectionPoint");
    QTest::addColumn<Vector3>("expectedIntersectionDirectorVector");

    QTest::newRow("random #1") << Vector3(39.88, -23.21, 33.92) << Vector3(30.2, -84.56, 64.9) << Vector3(82.85, 73.2, 87.71)
                               << Vector3(13.81, -73.79, -30.48) << Vector3(35.957, 47.716, 128.158) << Vector3(7366.36, 1816.765, -1060.684);
    QTest::newRow("random #2") << Vector3(50.28, 78.16, -74.91) << Vector3(26.88, -47.3, 70.61) << Vector3(-92.03, -81.97, 95.4)
                               << Vector3(-34.56, -1.63, 16.31) << Vector3(-144.7, 79.007, -0.117) << Vector3(-656.369, -2878.694, -1678.502);
    QTest::newRow("random #3") << Vector3(-43.98, 35.01, 60.77) << Vector3(43.42, 58.97, -77.32) << Vector3(67.02, 24.16, -30.94)
                               << Vector3(68.17, 7.12, 57.74) << Vector3(-23.963, 38.542, 74.704) << Vector3(3955.446, -7777.975, -3710.835);
}

void test_MathTools::planeIntersection_2Planes_ShouldComputeExpectedIntersection()
{
    QFETCH(Vector3, plane1Point);
    QFETCH(Vector3, plane1Normal);
    QFETCH(Vector3, plane2Point);
    QFETCH(Vector3, plane2Normal);
    QFETCH(Vector3, expectedIntersectionPoint);
    QFETCH(Vector3, expectedIntersectionDirectorVector);

    double p1p[3] = { plane1Point.x, plane1Point.y, plane1Point.z };
    double p1n[3] = { plane1Normal.x, plane1Normal.y, plane1Normal.z };
    double p2p[3] = { plane2Point.x, plane2Point.y, plane2Point.z };
    double p2n[3] = { plane2Normal.x, plane2Normal.y, plane2Normal.z };
    double ip[3];
    double idv[3];

    QCOMPARE(MathTools::planeIntersection(p1p, p1n, p2p, p2n, ip, idv), 1);

    Vector3 intersectionPoint(ip[0], ip[1], ip[2]);
    Vector3 intersectionDirectorVector(idv[0], idv[1], idv[2]);

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(intersectionPoint, expectedIntersectionPoint, 0.001),
             qPrintable(QString("actual: %1, expected: %2, difference: %3").arg(intersectionPoint).arg(expectedIntersectionPoint)
                                                                           .arg(intersectionPoint - expectedIntersectionPoint)));
    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(intersectionDirectorVector, expectedIntersectionDirectorVector, 0.001),
             qPrintable(QString("actual: %1, expected: %2, difference: %3").arg(intersectionDirectorVector).arg(expectedIntersectionDirectorVector)
                                                                           .arg(intersectionDirectorVector - expectedIntersectionDirectorVector)));
}

void test_MathTools::planeIntersection_2Planes_ShouldComputeIntersectionWithNaNPointAndNullDirectorVector_data()
{
    QTest::addColumn<Vector3>("plane1Point");
    QTest::addColumn<Vector3>("plane1Normal");
    QTest::addColumn<Vector3>("plane2Point");
    QTest::addColumn<Vector3>("plane2Normal");

    QTest::newRow("null normals") << Vector3() << Vector3() << Vector3() << Vector3();
    QTest::newRow("null plane 1 normal") << Vector3() << Vector3() << Vector3() << Vector3(13.81, -73.79, -30.48);
    QTest::newRow("null plane 2 normal") << Vector3() << Vector3(30.2, -84.56, 64.9) << Vector3() << Vector3();
}

void test_MathTools::planeIntersection_2Planes_ShouldComputeIntersectionWithNaNPointAndNullDirectorVector()
{
    QFETCH(Vector3, plane1Point);
    QFETCH(Vector3, plane1Normal);
    QFETCH(Vector3, plane2Point);
    QFETCH(Vector3, plane2Normal);

    double p1p[3] = { plane1Point.x, plane1Point.y, plane1Point.z };
    double p1n[3] = { plane1Normal.x, plane1Normal.y, plane1Normal.z };
    double p2p[3] = { plane2Point.x, plane2Point.y, plane2Point.z };
    double p2n[3] = { plane2Normal.x, plane2Normal.y, plane2Normal.z };
    double ip[3];
    double idv[3];

    QCOMPARE(MathTools::planeIntersection(p1p, p1n, p2p, p2n, ip, idv), 1);
    QVERIFY(MathTools::isNaN(ip[0]));
    QVERIFY(MathTools::isNaN(ip[1]));
    QVERIFY(MathTools::isNaN(ip[2]));
    QCOMPARE(idv[0], 0.0);
    QCOMPARE(idv[1], 0.0);
    QCOMPARE(idv[2], 0.0);
}

void test_MathTools::roundUpToPowerOf2_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<unsigned int>("input");
    QTest::addColumn<unsigned int>("output");

    QTest::newRow("0")         <<        0u <<         1u;
    QTest::newRow("random #1") <<        1u <<         1u;
    QTest::newRow("random #2") <<       16u <<        16u;
    QTest::newRow("random #3") <<      425u <<       512u;
    QTest::newRow("random #4") <<     1335u <<      2048u;
    QTest::newRow("random #5") <<    58102u <<     65536u;
    QTest::newRow("random #6") << 16044632u <<  16777216u;
    QTest::newRow("random #7") << 19122171u <<  33554432u;
    QTest::newRow("random #8") << 91138018u << 134217728u;
}

void test_MathTools::roundUpToPowerOf2_ShouldReturnExpectedValue()
{
    QFETCH(unsigned int, input);
    QFETCH(unsigned int, output);

    QCOMPARE(MathTools::roundUpToPowerOf2(input), output);
}

void test_MathTools::roundUpToMultipleOfNumber_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<unsigned int>("input");
    QTest::addColumn<unsigned int>("multiple");
    QTest::addColumn<unsigned int>("output");

    QTest::newRow("random #1") <<  0u << 49u <<  0u;
    QTest::newRow("random #2") <<  9u << 14u << 14u;
    QTest::newRow("random #3") << 23u << 15u << 30u;
    QTest::newRow("random #4") << 30u << 15u << 30u;
    QTest::newRow("random #5") <<  2u << 32u << 32u;
    QTest::newRow("random #6") << 34u << 38u << 38u;
    QTest::newRow("random #7") << 53u <<  4u << 56u;
    QTest::newRow("random #8") << 88u << 24u << 96u;
}

void test_MathTools::roundUpToMultipleOfNumber_ShouldReturnExpectedValue()
{
    QFETCH(unsigned int, input);
    QFETCH(unsigned int, multiple);
    QFETCH(unsigned int, output);

    QCOMPARE(MathTools::roundUpToMultipleOfNumber(input, multiple), output);
}

void test_MathTools::getPointToClosestEdgeDistance_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<QList<Vector3>>("points");
    QTest::addColumn<bool>("lastToFirstEdge");
    QTest::addColumn<Vector3>("point3D");
    QTest::addColumn<double>("expectedDistance");
    QTest::addColumn<int>("expectedClosestEdge");
    QTest::addColumn<Vector3>("expectedClosestPoint");

    QList<Vector3> pointsInAxialView;
    pointsInAxialView << Vector3( -2.74219,  -4.71093, 1153.41);
    pointsInAxialView << Vector3(-34.0234 ,  32.9688 , 1153.41);
    pointsInAxialView << Vector3( 11.4766 ,  59.2734 , 1153.41);
    pointsInAxialView << Vector3( 71.9062 ,  20.1719 , 1153.41);
    pointsInAxialView << Vector3( 61.9531 , -11.8203 , 1153.41);
    pointsInAxialView << Vector3( 22.1406 ,  20.8828 , 1153.41);
    QList<Vector3> pointsInSagitalView;
    pointsInSagitalView << Vector3(3.9731,   2.40157, 1239.02);
    pointsInSagitalView << Vector3(3.9731,  28.0331 , 1241.06);
    pointsInSagitalView << Vector3(3.9731,   7.28377, 1223.15);
    pointsInSagitalView << Vector3(3.9731, -13.8724 , 1236.17);
    pointsInSagitalView << Vector3(3.9731,  -6.95596, 1263.03);
    pointsInSagitalView << Vector3(3.9731,   9.31801, 1256.11);

    QList<Vector3> pointsInCoronalView;
    pointsInCoronalView << Vector3( 24.3605 , 27.4214, 1264.25);
    pointsInCoronalView << Vector3(  9.30705, 27.4214, 1245.94);
    pointsInCoronalView << Vector3( 22.7331 , 27.4214, 1230.88);
    pointsInCoronalView << Vector3( -2.49158, 27.4214, 1232.92);
    pointsInCoronalView << Vector3(-13.0697 , 27.4214, 1247.97);
    pointsInCoronalView << Vector3( -5.33952, 27.4214, 1260.58);

    QTest::newRow("axial. Closed. Edge0 out") << pointsInAxialView << true << Vector3(-34.7344, 8.08594, 1153.41) << 16.441 << 0 << Vector3(-22.0845,18.5877,1153.41);
    QTest::newRow("axial. Closed. Edge0 in") << pointsInAxialView << true << Vector3(-9.14062, 23.7266, 1153.41) << 13.2416 << 0 << Vector3(-19.3288,15.2684,1153.41);
    QTest::newRow("axial. Closed. Edge3 out") << pointsInAxialView << true << Vector3(111.719, 72.0703, 1153.41) << 65.4101 << 2 << Vector3(71.9062,20.1719,1153.41);
    QTest::newRow("axial. Closed. Edge5 on") << pointsInAxialView << true << Vector3(15.0312, 15.1953, 1153.41) << 1.13275 << 5 << Vector3(15.8434,14.4057,1153.41);
    QTest::newRow("axial. Opened. Edge5 on") << pointsInAxialView << false << Vector3(15.0312, 15.1953, 1153.41) << 9.10444 << 4 << Vector3(22.1406,20.8828,1153.41);

    QTest::newRow("sagital. Closed. Edge4 out") << pointsInSagitalView << true << Vector3(3.9731, 1.99472, 1264.65) << 4.99872 << 4 << Vector3(3.9731,0.0395213,1260.05);
    QTest::newRow("sagital. Closed. Edge3 in") << pointsInSagitalView << true << Vector3(3.9731, -6.14226, 1254.08) << 3.02059 << 3 << Vector3(3.9731,-9.06737,1254.83);
    QTest::newRow("sagital. Closed. Edge1 out") << pointsInSagitalView << true << Vector3(3.9731, 45.5276, 1194.27) << 46.8536 << 1 << Vector3(3.9731,14.9213,1229.74);
    QTest::newRow("sagital. Closed. Edge5 on") << pointsInSagitalView << true << Vector3(3.9731, 4.84267, 1243.9) << 0.431003 << 5 << Vector3(3.9731,4.44315,1244.07);
    QTest::newRow("sagital. Opened. Edge5 on") << pointsInSagitalView << false << Vector3(3.9731, 4.84267, 1243.9) << 4.67376 << 0 << Vector3(3.9731,5.21244,1239.25);

    QTest::newRow("coronal. Closed. Edge0 out") << pointsInCoronalView << true << Vector3(53.2468, 27.4214, 1264.25) << 28.8863 << 0 << Vector3(24.3605,27.4214,1264.25);
    QTest::newRow("coronal. Closed. Edge4 in") << pointsInCoronalView << true << Vector3(-13.8834, 27.4214, 1256.92) << 5.37105 << 4 << Vector3(-9.304,27.4214,1254.12);
    QTest::newRow("coronal. Closed. Edge2 out") << pointsInCoronalView << true << Vector3(7.27281, 27.4214, 1169.45) << 62.4782 << 2 << Vector3(12.2951,27.4214,1231.73);
    QTest::newRow("coronal. Closed. Edge5 on") << pointsInCoronalView << true << Vector3(15.0029, 27.4214, 1262.21) << 0.873961 << 5 << Vector3(14.896,27.4214,1263.08);
    QTest::newRow("coronal. Opened. Edge5 on") << pointsInCoronalView << false << Vector3(15.0029, 27.4214, 1262.21) << 5.93604 << 0 << Vector3(19.5881,27.4214,1258.44);
}

void test_MathTools::getPointToClosestEdgeDistance_ShouldReturnExpectedValues()
{
    QFETCH(QList<Vector3>, points);
    QFETCH(bool, lastToFirstEdge);
    QFETCH(Vector3, point3D);
    QFETCH(double, expectedDistance);
    QFETCH(int, expectedClosestEdge);
    QFETCH(Vector3, expectedClosestPoint);

    int closestEdge;
    Vector3 closestPoint;
    double distance = MathTools::getPointToClosestEdgeDistance(point3D, points, lastToFirstEdge, closestPoint, closestEdge);

    QString distanceError(QString("\nActual: %1\nExpected: %2").arg(distance).arg(expectedDistance));
    QString closestPointError(QString("\nActual: %1\nExpected: %2").arg(closestPoint).arg(expectedClosestPoint));

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(closestPoint, expectedClosestPoint, 0.1), qPrintable(closestPointError));
    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(distance, expectedDistance, 0.1), qPrintable(distanceError));
    QCOMPARE(closestEdge, expectedClosestEdge);
}

void test_MathTools::getPointToFiniteLineDistance_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<Vector3>("point");
    QTest::addColumn<Vector3>("lineFirstPoint");
    QTest::addColumn<Vector3>("lineSecondPoint");
    QTest::addColumn<Vector3>("expectedClosestPoint");
    QTest::addColumn<double>("expectedDistance");

    QTest::newRow("null point, null line") << Vector3() << Vector3() << Vector3() << Vector3() << 0.0;

    QTest::newRow("axial, point = line first point") << Vector3(84.29, -71.5, -3.39) << Vector3(84.29, -71.5, -3.39) << Vector3(29.1, 37.75, -3.39)
                                                     << Vector3(84.29, -71.5, -3.39) << 0.0;
    QTest::newRow("axial, point = line second point") << Vector3(29.1, 37.75, -3.39) << Vector3(84.29, -71.5, -3.39) << Vector3(29.1, 37.75, -3.39)
                                                      << Vector3(29.1, 37.75, -3.39) << 0.0;
    QTest::newRow("axial, point in line") << Vector3(70.494, -44.19, -3.39) << Vector3(84.29, -71.5, -3.39) << Vector3(29.1, 37.75, -3.39)
                                          << Vector3(70.494, -44.19, -3.39) << 0.0;
    QTest::newRow("axial, point out of line") << Vector3(23.78, 85.88, -3.39) << Vector3(84.29, -71.5, -3.39) << Vector3(29.1, 37.75, -3.39)
                                              << Vector3(29.1, 37.75, -3.39) << 48.423;

    QTest::newRow("sagital, point = line first point") << Vector3(67.85, -52.41, 7.71) << Vector3(67.85, -52.41, 7.71) << Vector3(67.85, 28.65, -98.6)
                                                       << Vector3(67.85, -52.41, 7.71) << 0.0;
    QTest::newRow("sagital, point = line second point") << Vector3(67.85, 28.65, -98.6) << Vector3(67.85, -52.41, 7.71) << Vector3(67.85, 28.65, -98.6)
                                                        << Vector3(67.85, 28.65, -98.6) << 0.0;
    QTest::newRow("sagital, point in line") << Vector3(67.85, 10.588, -74.912) << Vector3(67.85, -52.41, 7.71) << Vector3(67.85, 28.65, -98.6)
                                            << Vector3(67.85, 10.588, -74.912) << 0.0;
    QTest::newRow("sagital, point out of line") << Vector3(67.85, 36.56, 76.41) << Vector3(67.85, -52.41, 7.71) << Vector3(67.85, 28.65, -98.6)
                                                << Vector3(67.85, -52.41, 7.71) << 112.407;

    QTest::newRow("coronal, point = line first point") << Vector3(98.46, 98.28, 33.42) << Vector3(98.46, 98.28, 33.42) << Vector3(-7.8, 98.28, 61.2)
                                                       << Vector3(98.46, 98.28, 33.42) << 0.0;
    QTest::newRow("coronal, point = line second point") << Vector3(-7.8, 98.28, 61.2) << Vector3(98.46, 98.28, 33.42) << Vector3(-7.8, 98.28, 61.2)
                                                        << Vector3(-7.8, 98.28, 61.2) << 0.0;
    QTest::newRow("coronal, point in line") << Vector3(-0.032, 98.28, 59.169) << Vector3(98.46, 98.28, 33.42) << Vector3(-7.8, 98.28, 61.2)
                                            << Vector3(-0.032, 98.28, 59.169) << 0.0;
    QTest::newRow("coronal, point out of line") << Vector3(65.71, 98.28, 11.61) << Vector3(98.46, 98.28, 33.42) << Vector3(-7.8, 98.28, 61.2)
                                                << Vector3(73.142, 98.28, 40.039) << 29.384;

    QTest::newRow("3D, point = line first point") << Vector3(35.77, -92.8, -46.9) << Vector3(35.77, -92.8, -46.9) << Vector3(-7.52, -23.94, -62.23)
                                                  << Vector3(35.77, -92.8, -46.9) << 0.0;
    QTest::newRow("3D, point = line second point") << Vector3(-7.52, -23.94, -62.23) << Vector3(35.77, -92.8, -46.9) << Vector3(-7.52, -23.94, -62.23)
                                                   << Vector3(-7.52, -23.94, -62.23) << 0.0;
    QTest::newRow("3D, point in line") << Vector3(-2.142, -32.495, -60.325) << Vector3(35.77, -92.8, -46.9) << Vector3(-7.52, -23.94, -62.23)
                                       << Vector3(-2.142, -32.495, -60.325) << 0.0;
    QTest::newRow("3D, point out of line") << Vector3(-23.28, 0.6, -17) << Vector3(35.77, -92.8, -46.9) << Vector3(-7.52, -23.94, -62.23)
                                           << Vector3(-7.52, -23.94, -62.23) << 53.818;
}

void test_MathTools::getPointToFiniteLineDistance_ShouldReturnExpectedValues()
{
    QFETCH(Vector3, point);
    QFETCH(Vector3, lineFirstPoint);
    QFETCH(Vector3, lineSecondPoint);
    QFETCH(Vector3, expectedClosestPoint);
    QFETCH(double, expectedDistance);

    Vector3 closestPoint;
    double distance = MathTools::getPointToFiniteLineDistance(point, lineFirstPoint, lineSecondPoint, closestPoint);

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(closestPoint, expectedClosestPoint, 0.001),
             qPrintable(QString("actual: %1, expected: %2, difference: %3").arg(closestPoint).arg(expectedClosestPoint)
                                                                           .arg(closestPoint - expectedClosestPoint)));
    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(distance, expectedDistance, 0.001),
             qPrintable(QString("actual: %1, expected: %2, difference: %3").arg(distance).arg(expectedDistance).arg(distance - expectedDistance)));
}

void test_MathTools::isNaN_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<double>("x");
    QTest::addColumn<bool>("isNaN");

    QTest::newRow("real number") << 0.0 << false;
    QTest::newRow("+inf") << std::numeric_limits<double>::infinity() << false;
    QTest::newRow("-inf") << -std::numeric_limits<double>::infinity() << false;
    QTest::newRow("+quiet NaN") << std::numeric_limits<double>::quiet_NaN() << true;
    QTest::newRow("-quiet NaN") << -std::numeric_limits<double>::quiet_NaN() << true;
    QTest::newRow("+signaling NaN") << std::numeric_limits<double>::signaling_NaN() << true;
    QTest::newRow("-signaling NaN") << -std::numeric_limits<double>::signaling_NaN() << true;
}

void test_MathTools::isNaN_ShouldReturnExpectedValue()
{
    QFETCH(double, x);
    QFETCH(bool, isNaN);

    QCOMPARE(MathTools::isNaN(x), isNaN);
}

void test_MathTools::copySign_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<double>("value");
    QTest::addColumn<double>("sign");
    QTest::addColumn<double>("expectedResult");

    QTest::newRow("+,+") << 7.78 << 18.61 << 7.78;
    QTest::newRow("+,-") << 71.78 << -23.27 << -71.78;
    QTest::newRow("-,+") << -83.84 << 81.16 << 83.84;
    QTest::newRow("-,-") << -78.51 << -36.61 << -78.51;
    QTest::newRow("+,+0") << 7.78 << 0.0 << 7.78;
    QTest::newRow("+,-0") << 71.78 << -0.0 << 71.78;
    QTest::newRow("-,+0") << -83.84 << 0.0 << -83.84;
    QTest::newRow("-,-0") << -78.51 << -0.0 << -78.51;
    QTest::newRow("+,+qNaN") << 7.78 << std::numeric_limits<double>::quiet_NaN() << -7.78;
    QTest::newRow("+,-qNaN") << 71.78 << -std::numeric_limits<double>::quiet_NaN() << -71.78;
    QTest::newRow("-,+qNaN") << -83.84 << std::numeric_limits<double>::quiet_NaN() << 83.84;
    QTest::newRow("-,-qNaN") << -78.51 << -std::numeric_limits<double>::quiet_NaN() << 78.51;
    QTest::newRow("+,+sNaN") << 7.78 << std::numeric_limits<double>::signaling_NaN() << -7.78;
    QTest::newRow("+,-sNaN") << 71.78 << -std::numeric_limits<double>::signaling_NaN() << -71.78;
    QTest::newRow("-,+sNaN") << -83.84 << std::numeric_limits<double>::signaling_NaN() << 83.84;
    QTest::newRow("-,-sNaN") << -78.51 << -std::numeric_limits<double>::signaling_NaN() << 78.51;
}

void test_MathTools::copySign_ShouldReturnExpectedValue()
{
    QFETCH(double, value);
    QFETCH(double, sign);
    QFETCH(double, expectedResult);

    double result = MathTools::copySign(value, sign);

    QCOMPARE(result, expectedResult);
}

void test_MathTools::copySign_ShouldReturnZeroWithExpectedSign_data()
{
    QTest::addColumn<double>("inputZero");
    QTest::addColumn<double>("sign");
    QTest::addColumn<double>("zeroWithExpectedSign");

    QTest::newRow("+0,+") << 0.0 << 18.61 << 0.0;
    QTest::newRow("+0,-") << 0.0 << -23.27 << 0.0;
    QTest::newRow("-0,+") << -0.0 << 81.16 << -0.0;
    QTest::newRow("-0,-") << -0.0 << -36.61 << -0.0;
    QTest::newRow("+0,+0") << 0.0 << 0.0 << 0.0;
    QTest::newRow("+0,-0") << 0.0 << -0.0 << 0.0;
    QTest::newRow("-0,+0") << -0.0 << 0.0 << -0.0;
    QTest::newRow("-0,-0") << -0.0 << -0.0 << -0.0;
    QTest::newRow("+0,+qNaN") << 0.0 << std::numeric_limits<double>::quiet_NaN() << -0.0;
    QTest::newRow("+0,-qNaN") << 0.0 << -std::numeric_limits<double>::quiet_NaN() << -0.0;
    QTest::newRow("-0,+qNaN") << -0.0 << std::numeric_limits<double>::quiet_NaN() << 0.0;
    QTest::newRow("-0,-qNaN") << -0.0 << -std::numeric_limits<double>::quiet_NaN() << 0.0;
    QTest::newRow("+0,+sNaN") << 0.0 << std::numeric_limits<double>::signaling_NaN() << -0.0;
    QTest::newRow("+0,-sNaN") << 0.0 << -std::numeric_limits<double>::signaling_NaN() << -0.0;
    QTest::newRow("-0,+sNaN") << -0.0 << std::numeric_limits<double>::signaling_NaN() << 0.0;
    QTest::newRow("-0,-sNaN") << -0.0 << -std::numeric_limits<double>::signaling_NaN() << 0.0;
}

void test_MathTools::copySign_ShouldReturnZeroWithExpectedSign()
{
    QFETCH(double, inputZero);
    QFETCH(double, sign);
    QFETCH(double, zeroWithExpectedSign);

    // Hem de distingir +0 de -0. Una de les maneres, d'acord amb la Wikipedia (http://en.wikipedia.org/wiki/Signed_zero#Comparisons) és comprovar el el patró
    // de bits. Per tant, agafem punters a les dues variables i comparem que siguin exactament els mateixos bits.
    double zero = MathTools::copySign(inputZero, sign);
    void *pZero = &zero;
    void *pZeroWithExpectedSign = &zeroWithExpectedSign;
    QVERIFY(memcmp(pZero, pZeroWithExpectedSign, sizeof(double)) == 0);
}

void test_MathTools::copySign_ShouldReturnNaNWithExpectedSign_data()
{
    QTest::addColumn<double>("inputNaN");
    QTest::addColumn<double>("sign");
    QTest::addColumn<bool>("expectedSign");
    // Interpretem true = +, false = -

    QTest::newRow("+qNaN,+") << std::numeric_limits<double>::quiet_NaN() << 18.61 << false;
    QTest::newRow("+qNaN,-") << std::numeric_limits<double>::quiet_NaN() << -23.27 << false;
    QTest::newRow("-qNaN,+") << -std::numeric_limits<double>::quiet_NaN() << 81.16 << true;
    QTest::newRow("-qNaN,-") << -std::numeric_limits<double>::quiet_NaN() << -36.61 << true;
    QTest::newRow("+qNaN,+0") << std::numeric_limits<double>::quiet_NaN() << 0.0 << false;
    QTest::newRow("+qNaN,-0") << std::numeric_limits<double>::quiet_NaN() << -0.0 << false;
    QTest::newRow("-qNaN,+0") << -std::numeric_limits<double>::quiet_NaN() << 0.0 << true;
    QTest::newRow("-qNaN,-0") << -std::numeric_limits<double>::quiet_NaN() << -0.0 << true;
    QTest::newRow("+qNaN,+qNaN") << std::numeric_limits<double>::quiet_NaN() << std::numeric_limits<double>::quiet_NaN() << false;
    QTest::newRow("+qNaN,-qNaN") << std::numeric_limits<double>::quiet_NaN() << -std::numeric_limits<double>::quiet_NaN() << false;
    QTest::newRow("-qNaN,+qNaN") << -std::numeric_limits<double>::quiet_NaN() << std::numeric_limits<double>::quiet_NaN() << true;
    QTest::newRow("-qNaN,-qNaN") << -std::numeric_limits<double>::quiet_NaN() << -std::numeric_limits<double>::quiet_NaN() << true;
    QTest::newRow("+qNaN,+sNaN") << std::numeric_limits<double>::quiet_NaN() << std::numeric_limits<double>::signaling_NaN() << false;
    QTest::newRow("+qNaN,-sNaN") << std::numeric_limits<double>::quiet_NaN() << -std::numeric_limits<double>::signaling_NaN() << false;
    QTest::newRow("-qNaN,+sNaN") << -std::numeric_limits<double>::quiet_NaN() << std::numeric_limits<double>::signaling_NaN() << true;
    QTest::newRow("-qNaN,-sNaN") << -std::numeric_limits<double>::quiet_NaN() << -std::numeric_limits<double>::signaling_NaN() << true;
    QTest::newRow("+sNaN,+") << std::numeric_limits<double>::signaling_NaN() << 18.61 << false;
    QTest::newRow("+sNaN,-") << std::numeric_limits<double>::signaling_NaN() << -23.27 << false;
    QTest::newRow("-sNaN,+") << -std::numeric_limits<double>::signaling_NaN() << 81.16 << true;
    QTest::newRow("-sNaN,-") << -std::numeric_limits<double>::signaling_NaN() << -36.61 << true;
    QTest::newRow("+sNaN,+0") << std::numeric_limits<double>::signaling_NaN() << 0.0 << false;
    QTest::newRow("+sNaN,-0") << std::numeric_limits<double>::signaling_NaN() << -0.0 << false;
    QTest::newRow("-sNaN,+0") << -std::numeric_limits<double>::signaling_NaN() << 0.0 << true;
    QTest::newRow("-sNaN,-0") << -std::numeric_limits<double>::signaling_NaN() << -0.0 << true;
    QTest::newRow("+sNaN,+qNaN") << std::numeric_limits<double>::signaling_NaN() << std::numeric_limits<double>::quiet_NaN() << false;
    QTest::newRow("+sNaN,-qNaN") << std::numeric_limits<double>::signaling_NaN() << -std::numeric_limits<double>::quiet_NaN() << false;
    QTest::newRow("-sNaN,+qNaN") << -std::numeric_limits<double>::signaling_NaN() << std::numeric_limits<double>::quiet_NaN() << true;
    QTest::newRow("-sNaN,-qNaN") << -std::numeric_limits<double>::signaling_NaN() << -std::numeric_limits<double>::quiet_NaN() << true;
    QTest::newRow("+sNaN,+sNaN") << std::numeric_limits<double>::signaling_NaN() << std::numeric_limits<double>::signaling_NaN() << false;
    QTest::newRow("+sNaN,-sNaN") << std::numeric_limits<double>::signaling_NaN() << -std::numeric_limits<double>::signaling_NaN() << false;
    QTest::newRow("-sNaN,+sNaN") << -std::numeric_limits<double>::signaling_NaN() << std::numeric_limits<double>::signaling_NaN() << true;
    QTest::newRow("-sNaN,-sNaN") << -std::numeric_limits<double>::signaling_NaN() << -std::numeric_limits<double>::signaling_NaN() << true;
}

void test_MathTools::copySign_ShouldReturnNaNWithExpectedSign()
{
    QFETCH(double, inputNaN);
    QFETCH(double, sign);
    QFETCH(bool, expectedSign);

    double NaN = MathTools::copySign(inputNaN, sign);
    QVERIFY2(MathTools::isNaN(NaN), qPrintable(QString("actual: %1, expected: nan").arg(NaN)));

    // Hem de distingir +NaN de -NaN. Aquest cop no podem comparar tot el patró de bits com amb els zeros, perquè hi ha bits que poden tenir diferents valors
    // que representen NaN (http://en.wikipedia.org/wiki/NaN). El que sí podem fer és comprovar el bit més significatiu, el de més a l'esquerra, que és el del
    // signe. Si el bit és zero vol dir positiu, i si és 1 vol dir negatiu (http://en.wikipedia.org/wiki/Sign_bit). Per fer-ho, llegim aquest float com si fos
    // un unsigned long long per crear un bitset. D'aquest bitset n'hem de consultar l'últim bit, perquè estan ordenats de dreta a esquerra. Abans de tot
    // comprovem que el format dels doubles sigui IEC 559 / IEEE 754. Sinó, tota la resta no té sentit.
    QVERIFY(std::numeric_limits<double>::is_iec559);
    unsigned long long *pNaN = reinterpret_cast<unsigned long long*>(&NaN);
    std::bitset<8*sizeof(unsigned long long)> fNaNBits(*pNaN);
    bool signBit = fNaNBits.test(fNaNBits.size() - 1);
    QCOMPARE(!signBit, expectedSign);
}

void test_MathTools::setupRadiansAndDegreesConversionData()
{
    QTest::addColumn<float>("radians");
    QTest::addColumn<float>("degrees");

    QTest::newRow("0 | 0") << 0.0f << 0.0f;
    QTest::newRow("pi/6 | 30") << 0.5235987755982988f << 30.0f;
    QTest::newRow("pi/4 | 45") << 0.7853981633974483f << 45.0f;
    QTest::newRow("pi/3 | 60") << 1.0471975511965976f << 60.0f;
    QTest::newRow("pi/2 | 90") << 1.5707963267948966f << 90.0f;
    QTest::newRow("2*pi/3 | 120") << 2.0943951023931953f << 120.0f;
    QTest::newRow("3*pi/4 | 135") << 2.356194490192345f << 135.0f;
    QTest::newRow("5*pi/6 | 150") << 2.6179938779914944f << 150.0f;
    QTest::newRow("pi | 180") << 3.141592653589793f << 180.0f;
    QTest::newRow("7*pi/6 | 210") << 3.665191429188092f << 210.0f;
    QTest::newRow("5*pi/4 | 225") << 3.9269908169872414f << 225.0f;
    QTest::newRow("4*pi/3 | 240") << 4.1887902047863905f << 240.0f;
    QTest::newRow("3*pi/2 | 270") << 4.71238898038469f << 270.0f;
    QTest::newRow("5*pi/3 | 300") << 5.235987755982989f << 300.0f;
    QTest::newRow("7*pi/4 | 315") << 5.497787143782138f << 315.0f;
    QTest::newRow("11*pi/6 | 330") << 5.759586531581287f << 330.0f;
    QTest::newRow("2*pi | 360") << 6.283185307179586f << 360.0f;
    QTest::newRow("-0 | -0") << -0.0f << -0.0f;
    QTest::newRow("-pi/6 | -30") << -0.5235987755982988f << -30.0f;
    QTest::newRow("-pi/4 | -45") << -0.7853981633974483f << -45.0f;
    QTest::newRow("-pi/3 | -60") << -1.0471975511965976f << -60.0f;
    QTest::newRow("-pi/2 | -90") << -1.5707963267948966f << -90.0f;
    QTest::newRow("-2*pi/3 | -120") << -2.0943951023931953f << -120.0f;
    QTest::newRow("-3*pi/4 | -135") << -2.356194490192345f << -135.0f;
    QTest::newRow("-5*pi/6 | -150") << -2.6179938779914944f << -150.0f;
    QTest::newRow("-pi | -180") << -3.141592653589793f << -180.0f;
    QTest::newRow("-7*pi/6 | -210") << -3.665191429188092f << -210.0f;
    QTest::newRow("-5*pi/4 | -225") << -3.9269908169872414f << -225.0f;
    QTest::newRow("-4*pi/3 | -240") << -4.1887902047863905f << -240.0f;
    QTest::newRow("-3*pi/2 | -270") << -4.71238898038469f << -270.0f;
    QTest::newRow("-5*pi/3 | -300") << -5.235987755982989f << -300.0f;
    QTest::newRow("-7*pi/4 | -315") << -5.497787143782138f << -315.0f;
    QTest::newRow("-11*pi/6 | -330") << -5.759586531581287f << -330.0f;
    QTest::newRow("-2*pi | -360") << -6.283185307179586f << -360.0f;
}

void test_MathTools::setupComputeAngleOfAVectorData()
{
    QTest::addColumn<QVector2D>("vector");
    QTest::addColumn<double>("angleInRadians");

    QTest::newRow("-X-Y / -3pi/4 rad / -135º") << QVector2D(-1.0, -1.0) << -3.0 * MathTools::PiNumber / 4.0;
    QTest::newRow("-Y / -pi/2 rad / -90º") << QVector2D(0.0, -1.0) << -MathTools::PiNumber / 2.0;
    QTest::newRow("+X-Y / -pi/4 rad / -45º") << QVector2D(1.0, -1.0) << -MathTools::PiNumber / 4.0;
    QTest::newRow("+X / 0 rad / 0º") << QVector2D(1.0, 0.0) << 0.0;
    QTest::newRow("+X+Y / pi/4 rad / 45º") << QVector2D(1.0, 1.0) << MathTools::PiNumber / 4.0;
    QTest::newRow("+Y / pi/2 rad / 90º") << QVector2D(0.0, 1.0) << MathTools::PiNumber / 2.0;
    QTest::newRow("-X+Y / 3pi/4 rad / 135º") << QVector2D(-1.0, 1.0) << 3.0 * MathTools::PiNumber / 4.0;
    QTest::newRow("-X / pi rad / 180º") << QVector2D(-1.0, 0.0) << MathTools::PiNumber;

    QTest::newRow("random Q1") << QVector2D(1.1, 4.0) << 1.30243;
    QTest::newRow("random Q2") << QVector2D(-9.7, 4.2) << 2.73297;
    QTest::newRow("random Q3") << QVector2D(-7.0, -6.2) << -2.41673;
    QTest::newRow("random Q4") << QVector2D(9.4, -9.3) << -0.78005;

    QTest::newRow("+0+0") << QVector2D(+0.0, +0.0) << +0.0;
    QTest::newRow("-0+0") << QVector2D(-0.0, +0.0) << MathTools::PiNumber;
    QTest::newRow("-0-0") << QVector2D(-0.0, -0.0) << -MathTools::PiNumber;
    QTest::newRow("+0-0") << QVector2D(+0.0, -0.0) << -0.0;
}

void test_MathTools::setupComputeAngleBetweenTwoVectorsData()
{
    QTest::addColumn<QVector3D>("vector1");
    QTest::addColumn<QVector3D>("vector2");
    QTest::addColumn<double>("angleInRadians");

    const QVector3D xVector(1, 0, 0);
    const QVector3D xMinusVector(-1, 0, 0);
    const QVector3D yVector(0, 1, 0);
    const QVector3D yMinusVector(0, -1, 0);
    const QVector3D zVector(0, 0, 1);
    const QVector3D zMinusVector(0, 0, -1);
    const QVector3D xMinusYVector(-1, 1, 0);
    const QVector3D xyVector(1, 1, 0);
    
    QTest::newRow("Two equal non-zero-valued vectors (X, X)") << xVector << xVector << 0.0;
    QTest::newRow("Two equal non-zero-valued vectors (Y, Y)") << yVector << yVector << 0.0;
    QTest::newRow("Two equal non-zero-valued vectors (Z, Z)") << zVector << zVector << 0.0;
    QTest::newRow("Two equal non-zero-valued vectors (-X, -X)") << xMinusVector << xMinusVector << 0.0;
    QTest::newRow("Two equal non-zero-valued vectors (-Y, -Y)") << yMinusVector << yMinusVector << 0.0;
    QTest::newRow("Two equal non-zero-valued vectors (-Z, -Z)") << zMinusVector << zMinusVector << 0.0;
    
    QTest::newRow("X Y vectors (90º)") << xVector << QVector3D(0, 1, 0) << 1.5708;
    QTest::newRow("X -Y vectors (90º)") << xVector << QVector3D(0, -1, 0) << 1.5708;
    QTest::newRow("-X Y vectors (90º)") << xMinusVector << QVector3D(0, 1, 0) << 1.5708;
    QTest::newRow("-X -Y vectors (90º)") << xMinusVector << QVector3D(0, -1, 0) << 1.5708;

    QTest::newRow("X Z vectors (90º)") << xVector << zVector << 1.5708;
    QTest::newRow("X -Z vectors (90º)") << xVector << zMinusVector << 1.5708;
    QTest::newRow("-X Z vectors (90º)") << xMinusVector << zVector << 1.5708;
    QTest::newRow("-X -Z vectors (90º)") << xMinusVector << zMinusVector << 1.5708;

    QTest::newRow("Y Z vectors (90º)") << yVector << zVector << 1.5708;
    QTest::newRow("Y -Z vectors (90º)") << yVector << zMinusVector << 1.5708;
    QTest::newRow("-Y Z vectors (90º)") << yMinusVector << zVector << 1.5708;
    QTest::newRow("-Y -Z vectors (90º)") << yMinusVector << zMinusVector << 1.5708;

    QTest::newRow("-XY Z vectors (90º)") << xMinusYVector << zVector << 1.5708;
    QTest::newRow("Z -XY vectors (90º)") << zVector << xMinusYVector << 1.5708;
    QTest::newRow("XY Z vectors (90º)") << xyVector << zVector << 1.5708;
    QTest::newRow("Z XY vectors (90º)") << zVector << xyVector << 1.5708;

    QTest::newRow("X -X vectors (180º)") << xVector << xMinusVector << 3.1416;
    QTest::newRow("-X X vectors (180º)") << xMinusVector << xVector << 3.1416;
    QTest::newRow("Y -Y vectors (180º)") << yVector << yMinusVector << 3.1416;
    QTest::newRow("-Y Y vectors (180º)") << yMinusVector << yVector << 3.1416;
    QTest::newRow("Z -Z vectors (180º)") << zVector << zMinusVector << 3.1416;
    QTest::newRow("-Z Z vectors (180º)") << zMinusVector << zVector << 3.1416;

    QTest::newRow("-XY Y vectors (45º)") << xMinusYVector << yVector << 0.7854;
    QTest::newRow("Y -XY vectors (45º)") << yVector << xMinusYVector << 0.7854;
    QTest::newRow("XY X vectors (45º)") << xyVector << xVector << 0.7854;
    QTest::newRow("X XY vectors (45º)") << xVector << xyVector << 0.7854;
    
    QTest::newRow("-XY -Y vectors (135º)") << xMinusYVector << yMinusVector << 2.3562;
    QTest::newRow("-Y -XY vectors (135º)") << yMinusVector << xMinusYVector << 2.3562;
    QTest::newRow("-XY X vectors (135º)") << xMinusYVector << xVector << 2.3562;
    QTest::newRow("X -XY vectors (135º)") << xVector << xMinusYVector << 2.3562;

    QTest::newRow("Two random vectors (73.9º)") << QVector3D(7.2, 1.0, -2) << QVector3D(0.3, 0.0003, -24.3) << 1.29007;
}

void test_MathTools::setupComputeAngleBetweenTwoVectorsNaNData()
{
    QTest::addColumn<QVector3D>("vector1");
    QTest::addColumn<QVector3D>("vector2");

    QTest::newRow("Two Zero-valued vectors") << QVector3D(0, 0, 0) << QVector3D(0, 0, 0);
    QTest::newRow("First vector is zero-valued") << QVector3D(0, 0, 0) << QVector3D(1, 1, 0);
    QTest::newRow("Second vector is zero-valued") << QVector3D(1, 1, 0) << QVector3D(0, 0, 0);
}

void test_MathTools::setupCrossAndDotProductData()
{
    QTest::addColumn<QVector3D>("vector1");
    QTest::addColumn<QVector3D>("vector2");
    QTest::addColumn<double>("dotProduct");
    QTest::addColumn<QVector3D>("crossProduct");

    QVector3D XVector(1, 0, 0);
    QVector3D YVector(0, 1, 0);
    QVector3D ZVector(0, 0, 1);
    
    QVector3D XYVector(1, 1, 0);
    QVector3D XZVector(1, 0, 1);
    QVector3D YZVector(0, 1, 1);

    QVector3D XYZVector(1, 1, 1);

    QTest::newRow("normalized X, X") << XVector << XVector << 1. << QVector3D(0, 0, 0);
    QTest::newRow("normalized Y, Y") << YVector << YVector << 1. << QVector3D(0, 0, 0);
    QTest::newRow("normalized Z, Z") << ZVector << ZVector << 1. << QVector3D(0, 0, 0);

    QTest::newRow("normalized X, Y") << XVector << YVector << 0. << QVector3D(0, 0, 1);
    QTest::newRow("normalized -X, Y") << QVector3D(-1, 0, 0) << YVector << 0. << QVector3D(0, 0, -1);
    QTest::newRow("normalized X, -Y") << XVector << QVector3D(0, -1, 0) << 0. << QVector3D(0, 0, -1);
    QTest::newRow("normalized -X, -Y") << QVector3D(-1, 0, 0) << QVector3D(0, -1, 0) << 0. << QVector3D(0, 0, 1);
    QTest::newRow("random X, Y") << QVector3D(45.6, 0, 0) << QVector3D(0, 78.22, 0) << 0. << QVector3D(0, 0, 3566.832);
    QTest::newRow("random -X, Y") << QVector3D(-45.6, 0, 0) << QVector3D(0, 78.22, 0) << 0. << QVector3D(0, 0, -3566.832);

    QTest::newRow("normalized X, Z") << XVector << ZVector << 0. << QVector3D(0, -1, 0);
    QTest::newRow("normalized -X, Z") << QVector3D(-1, 0, 0) << ZVector << 0. << QVector3D(0, 1, 0);
    QTest::newRow("normalized X, -Z") << XVector << QVector3D(0, 0, -1) << 0. << QVector3D(0, 1, 0);
    QTest::newRow("normalized -X, -Z") << QVector3D(-1, 0, 0) << QVector3D(0, 0, -1) << 0. << QVector3D(0, -1, 0);
    QTest::newRow("random X, Z") << QVector3D(83.5, 0, 0) << QVector3D(0, 0, 536.11) << 0. << QVector3D(0, -44765.185, 0);

    QTest::newRow("normalized Y, Z") << YVector << ZVector << 0. << QVector3D(1, 0, 0);
    QTest::newRow("normalized -Y, Z") << QVector3D(0, -1, 0) << ZVector << 0. << QVector3D(-1, 0, 0);
    QTest::newRow("normalized Y, -Z") << YVector << QVector3D(0, 0, -1) << 0. << QVector3D(-1, 0, 0);
    QTest::newRow("normalized -Y, -Z") << QVector3D(0, -1, 0) << QVector3D(0, 0, -1) << 0. << QVector3D(1, 0, 0);
    QTest::newRow("random Y, Z") << QVector3D(0, 12.3, 0) << QVector3D(0, 0, 0.36) << 0. << QVector3D(4.428, 0, 0);
    
    QTest::newRow("normalized XY, X") << XYVector << XVector << 1. << QVector3D(0, 0, -1);
    QTest::newRow("normalized XY, Y") << XYVector << YVector << 1. << QVector3D(0, 0, 1);
    QTest::newRow("normalized XY, Z") << XYVector << ZVector << 0. << QVector3D(1, -1, 0);

    QTest::newRow("normalized XY, XY") << XYVector << XYVector << 2. << QVector3D(0, 0, 0);
    QTest::newRow("normalized XY, YZ") << XYVector << YZVector << 1. << QVector3D(1, -1, 1);
    QTest::newRow("normalized XY, XZ") << XYVector << XZVector << 1. << QVector3D(1, -1, -1);

    QTest::newRow("normalized YZ, X") << YZVector << XVector << 0. << QVector3D(0, 1, -1);
    QTest::newRow("normalized YZ, Y") << YZVector << YVector << 1. << QVector3D(-1, 0, 0);
    QTest::newRow("normalized YZ, Z") << YZVector << ZVector << 1. << QVector3D(1, 0, 0);

    QTest::newRow("normalized YZ, XY") << YZVector << XYVector << 1. << QVector3D(-1, 1, -1);
    QTest::newRow("normalized YZ, YZ") << YZVector << YZVector << 2. << QVector3D(0, 0, 0);
    QTest::newRow("normalized YZ, XZ") << YZVector << XZVector << 1. << QVector3D(1, 1, -1);

    QTest::newRow("normalized XZ, X") << YZVector << XVector << 0. << QVector3D(0, 1, -1);
    QTest::newRow("normalized XZ, Y") << YZVector << YVector << 1. << QVector3D(-1, 0, 0);
    QTest::newRow("normalized XZ, Z") << YZVector << ZVector << 1. << QVector3D(1, 0, 0);

    QTest::newRow("normalized XZ, XY") << XZVector << XYVector << 1. << QVector3D(-1, 1, 1);
    QTest::newRow("normalized XZ, YZ") << XZVector << YZVector << 1. << QVector3D(-1, -1, 1);
    QTest::newRow("normalized XZ, XZ") << XZVector << XZVector << 2. << QVector3D(0, 0, 0);

    QTest::newRow("normalized XYZ, X") << XYZVector << XVector << 1. << QVector3D(0, 1, -1);
    QTest::newRow("normalized XYZ, Y") << XYZVector << YVector << 1. << QVector3D(-1, 0, 1);
    QTest::newRow("normalized XYZ, Z") << XYZVector << ZVector << 1. << QVector3D(1, -1, 0);

    QTest::newRow("normalized XYZ, XY") << XYZVector << XYVector << 2. << QVector3D(-1, 1, 0);
    QTest::newRow("normalized XYZ, XZ") << XYZVector << XZVector << 2. << QVector3D(1, 0, -1);
    QTest::newRow("normalized XYZ, YZ") << XYZVector << YZVector << 2. << QVector3D(0, -1, 1);

    QTest::newRow("normalized XYZ, XYZ") << XYZVector << XYZVector << 3. << QVector3D(0, 0, 0);
}

DECLARE_TEST(test_MathTools)

#include "test_mathtools.moc"
