#include "autotest.h"

#include <QVector3D>

#include "mathtools.h"
#include "fuzzycomparetesthelper.h"

using namespace udg;
using namespace testing;

class test_MathTools : public QObject {
Q_OBJECT

private slots:
    void angleInRadians_ShouldComputeAngleInRadians_data();
    void angleInRadians_ShouldComputeAngleInRadians();
    
    void angleInRadians_ShouldReturnNaN_data();
    void angleInRadians_ShouldReturnNaN();
    
    void angleInDegrees_ShouldComputeAngleInDegrees_data();
    void angleInDegrees_ShouldComputeAngleInDegrees();

    void angleInDegrees_ShouldReturnNaN_data();
    void angleInDegrees_ShouldReturnNaN();

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

    void infiniteLineIntersection_ShouldComputeExpectedIntersectionAndState_data();
    void infiniteLineIntersection_ShouldComputeExpectedIntersectionAndState();

    void roundUpToPowerOf2_ShouldReturnExpectedValue_data();
    void roundUpToPowerOf2_ShouldReturnExpectedValue();

    void getPointToClosestEdgeDistance_ShouldReturnExpectedValues_data();
    void getPointToClosestEdgeDistance_ShouldReturnExpectedValues();

private:
    void setupComputeAngleData();
    void setupComputeAngleNaNData();
    void setupCrossAndDotProductData();
};

Q_DECLARE_METATYPE(MathTools::IntersectionResults)
Q_DECLARE_METATYPE(QList<QVector<double> >)

// Epsilons pels diferents fuzzyCompare de cada test
const double AngleInDegreesEpsilon = 0.001;
const double AngleInRadiansEpsilon = 0.001;
const double NormalizeEpsilon = 0.000001;
const double ModulusEpsilon = 0.001;
const double crossProductEpsilon = 0.001;
const double directorVectorEpsilon = 0.001;
const double distance3DEpsion = 0.0005;
const double LineIntersectionEpsilon = 0.0001;

void test_MathTools::angleInRadians_ShouldComputeAngleInRadians_data()
{
    this->setupComputeAngleData();
}

void test_MathTools::angleInRadians_ShouldComputeAngleInRadians()
{
    QFETCH(QVector3D, vector1);
    QFETCH(QVector3D, vector2);
    QFETCH(double, angleInRadians);

    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(MathTools::angleInRadians(vector1, vector2), angleInRadians, AngleInRadiansEpsilon));
}

void test_MathTools::angleInDegrees_ShouldComputeAngleInDegrees_data()
{
    this->setupComputeAngleData();
}

void test_MathTools::angleInDegrees_ShouldComputeAngleInDegrees()
{
    QFETCH(QVector3D, vector1);
    QFETCH(QVector3D, vector2);
    QFETCH(double, angleInRadians);

    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(MathTools::angleInDegrees(vector1, vector2), angleInRadians * MathTools::RadiansToDegreesAsDouble, AngleInDegreesEpsilon));
}

void test_MathTools::angleInRadians_ShouldReturnNaN_data()
{
    this->setupComputeAngleNaNData();
}

void test_MathTools::angleInRadians_ShouldReturnNaN()
{
    QFETCH(QVector3D, vector1);
    QFETCH(QVector3D, vector2);

    QVERIFY(MathTools::isNaN(MathTools::angleInRadians(vector1, vector2)));
}

void test_MathTools::angleInDegrees_ShouldReturnNaN_data()
{
    this->setupComputeAngleNaNData();
}

void test_MathTools::angleInDegrees_ShouldReturnNaN()
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
    QTest::addColumn<QVector3D>("firstPoint");
    QTest::addColumn<QVector3D>("secondPoint");
    QTest::addColumn<QVector3D>("result");

    QTest::newRow("random 1") << QVector3D(1, 0, 0) << QVector3D(1, 0 ,0) << QVector3D(0, 0, 0);
    QTest::newRow("random 2") << QVector3D(12.3, 0, 0) << QVector3D(3.25, 0 ,0) << QVector3D(-9.05, 0, 0);
    QTest::newRow("random 3") << QVector3D(-71, -24, 1353.5) << QVector3D(-71, -24, 1353) << QVector3D(0, 0, -0.5);
    QTest::newRow("random 4") << QVector3D(143.24654, 0.31235458, 3.235697) << QVector3D(54.2358456, 45.524645, 125.5468) 
        << QVector3D(-89.0106944, 45.21229042, 122.311103);
    QTest::newRow("axial 1") << QVector3D(-36.8385, 0.946872, 1369.72) << QVector3D(-27.8487, 64.1007, 1369.72) << QVector3D(8.98986, 63.1538, 0);
    QTest::newRow("axial 2") << QVector3D(-52.346, 29.4897, 1465.75) << QVector3D(16.4264, -18.8308, 1465.75) << QVector3D(68.7725, -48.3205, 0);
    QTest::newRow("axial 3") << QVector3D(43.6208, 106.803, 1371.21) << QVector3D(-27.1744, -0.401608, 1371.21) << QVector3D(-70.7952, -107.204, 0);
    QTest::newRow("sagittal 1") << QVector3D(19.1385, 82.1874, 1478.69) << QVector3D(19.1385, 41.6725, 1403.12) << QVector3D(0, -40.5149, -75.5699);
    QTest::newRow("sagittal 2") << QVector3D(-47.1244, 98.1202, 1406.31) << QVector3D(-47.1244, 52.5979, 1495.99) << QVector3D(0, -45.5224, 89.6791);
    QTest::newRow("sagittal 3") << QVector3D(-7.76937, 59.8814, 1519.2) << QVector3D(-7.76937, 0.247106, 1424.97) << QVector3D(0, -59.6343, -94.23);
    QTest::newRow("coronal 1") << QVector3D(-56.3126, 81.3314, 1435.44) << QVector3D(-22.626, 81.3314, 1493.25) << QVector3D(33.6866, 0, 57.81);
    QTest::newRow("coronal 2") << QVector3D(67.5083, 3.17039, 1458.2) << QVector3D(-9.87976, 3.17039, 1403.57) << QVector3D(-77.3881, -9.76996e-15, -54.63);
    QTest::newRow("coronal 3") << QVector3D(17.4337, 74.9248, 1526.94) << QVector3D(46.1128, 74.9248, 1413.59) << QVector3D(28.6791, 0, -113.35);
}

void test_MathTools::directorVector_ShouldReturnExpectedValues()
{
    QFETCH(QVector3D, firstPoint);
    QFETCH(QVector3D, secondPoint);
    QFETCH(QVector3D, result);

    QVector3D directorVector = MathTools::directorVector(firstPoint, secondPoint);

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

void test_MathTools::infiniteLineIntersection_ShouldComputeExpectedIntersectionAndState_data()
{
    QTest::addColumn<QVector3D>("point1");
    QTest::addColumn<QVector3D>("point2");
    QTest::addColumn<QVector3D>("point3");
    QTest::addColumn<QVector3D>("point4");
    QTest::addColumn<MathTools::IntersectionResults>("expectedState");
    QTest::addColumn<QVector3D>("expectedIntersection");

    QTest::newRow("all points null") << QVector3D(0.0, 0.0, 0.0) << QVector3D(0.0, 0.0, 0.0) << QVector3D(0.0, 0.0, 0.0) << QVector3D(0.0, 0.0, 0.0)
                                     << MathTools::ParallelLines << QVector3D(0.0, 0.0, 0.0);

    QTest::newRow("p1 == p2") << QVector3D(-7.4, +6.0, +0.5) << QVector3D(-7.4, +6.0, +0.5) << QVector3D(+9.1, +9.6, -6.6) << QVector3D(+8.2, +2.2, -2.7)
                              << MathTools::ParallelLines << QVector3D(0.0, 0.0, 0.0);
    QTest::newRow("p3 == p4") << QVector3D(-7.5, +6.8, +0.3) << QVector3D(-3.9, +3.3, -1.3) << QVector3D(-3.9, -8.5, +8.0) << QVector3D(-3.9, -8.5, +8.0)
                              << MathTools::ParallelLines << QVector3D(0.0, 0.0, 0.0);
    QTest::newRow("p1 == p2 && p3 == p4") << QVector3D(-5.7, +7.2, -1.6) << QVector3D(-5.7, +7.2, -1.6) << QVector3D(-1.9, +7.6, +5.8)
                                          << QVector3D(-1.9, +7.6, +5.8) << MathTools::ParallelLines << QVector3D(0.0, 0.0, 0.0);
    QTest::newRow("p1 == p2 == p3 == p4") << QVector3D(+0.8, -6.0, +5.4) << QVector3D(+0.8, -6.0, +5.4) << QVector3D(+0.8, -6.0, +5.4)
                                          << QVector3D(+0.8, -6.0, +5.4) << MathTools::ParallelLines << QVector3D(0.0, 0.0, 0.0);

    QTest::newRow("parallel XY") << QVector3D(+3.3, -0.5, +2.5) << QVector3D(+2.9, -9.8, +2.5) << QVector3D(+3.5, +2.3, +2.5) << QVector3D(+6.3, +67.4, +2.5)
                                 << MathTools::ParallelLines << QVector3D(0.0, 0.0, 0.0);
    QTest::newRow("parallel XZ") << QVector3D(+4.6, -7.3, +5.0) << QVector3D(+9.6, -7.3, -9.9) << QVector3D(+4.2, -7.3, +2.6) << QVector3D(+14.2, -7.3, -27.2)
                                 << MathTools::ParallelLines << QVector3D(0.0, 0.0, 0.0);
    QTest::newRow("parallel YZ") << QVector3D(+2.5, +8.6, -1.8) << QVector3D(+2.5, +7.8, -1.3) << QVector3D(+2.5, -2.5, -1.0) << QVector3D(+2.5, -0.1, -2.5)
                                 << MathTools::ParallelLines << QVector3D(0.0, 0.0, 0.0);
    QTest::newRow("parallel") << QVector3D(-8.4, -6.2, +2.7) << QVector3D(-0.2, -0.1, -1.0) << QVector3D(+8.1, +3.9, +9.5) << QVector3D(+16.3, +10.0, +5.8)
                              << MathTools::ParallelLines << QVector3D(0.0, 0.0, 0.0);

    QTest::newRow("skew XY") << QVector3D(-8.5, -0.8, -2.2) << QVector3D(+9.2, -7.0, -2.2) << QVector3D(+2.4, -9.8, -8.3) << QVector3D(+8.3, -6.5, -8.3)
                             << MathTools::LinesIntersect << QVector3D(+8.09689, -6.6136, -2.2);
    QTest::newRow("skew XZ") << QVector3D(+4.6, -6.4, +0.5) << QVector3D(+7.5, -6.4, +5.5) << QVector3D(-8.8, +10.0, -8.1) << QVector3D(-5.9, +10.0, -1.1)
                             << MathTools::LinesIntersect << QVector3D(-29.83, -6.4, -58.8621);
    QTest::newRow("skew YZ") << QVector3D(-5.6, +8.9, -8.8) << QVector3D(-5.6, -1.5, -6.5) << QVector3D(-7.1, +6.6, -2.3) << QVector3D(-7.1, -1.7, -7.3)
                             << MathTools::LinesIntersect << QVector3D(-5.6, -0.674905, -6.68247);
    QTest::newRow("skew") << QVector3D(+7.6, -3.7, +7.9) << QVector3D(+8.8, -5.6, -8.9) << QVector3D(-1.3, +3.3, -9.7) << QVector3D(-4.3, -9.7, +4.4)
                          << MathTools::LinesIntersect << QVector3D(+8.27995, -4.77659, -1.6193);

    QTest::newRow("intersection XY") << QVector3D(+1.0, -1.6, -10.0) << QVector3D(-6.2, -8.2, -10.0) << QVector3D(-8.3, -4.4, -10.0)
                                     << QVector3D(+0.3, -1.4, -10.0) << MathTools::LinesIntersect << QVector3D(+1.78225, -0.882935, -10.0);
    QTest::newRow("intersection XZ") << QVector3D(+8.3, +1.6, -4.2) << QVector3D(-2.3, +1.6, +9.9) << QVector3D(-8.1, +1.6, +8.2)
                                     << QVector3D(-2.4, +1.6, -6.1) << MathTools::LinesIntersect << QVector3D(-16.0885, +1.6, +28.2413);
    QTest::newRow("intersection YZ") << QVector3D(+9.9, +3.3, +7.1) << QVector3D(+9.9, +7.6, +5.8) << QVector3D(+9.9, -0.2, -3.7)
                                     << QVector3D(+9.9, +5.9, -2.5) << MathTools::LinesIntersect << QVector3D(+9.9, +23.5616, +0.974408);
    QTest::newRow("intersection") << QVector3D(+1.8, -3.9, +6.5) << QVector3D(-6.2, +7.3, +8.6) << QVector3D(-9.9, +9.9, +4.0)
                                  << QVector3D(-340.09, +495.128, +140.259) << MathTools::LinesIntersect << QVector3D(-47.0, +64.42, +19.31);
}

void test_MathTools::infiniteLineIntersection_ShouldComputeExpectedIntersectionAndState()
{
    QFETCH(QVector3D, point1);
    QFETCH(QVector3D, point2);
    QFETCH(QVector3D, point3);
    QFETCH(QVector3D, point4);
    QFETCH(MathTools::IntersectionResults, expectedState);
    QFETCH(QVector3D, expectedIntersection);

    double p1[3] = { point1.x(), point1.y(), point1.z() };
    double p2[3] = { point2.x(), point2.y(), point2.z() };
    double p3[3] = { point3.x(), point3.y(), point3.z() };
    double p4[3] = { point4.x(), point4.y(), point4.z() };
    int state;
    double *i = MathTools::infiniteLinesIntersection(p1, p2, p3, p4, state);
    QVector3D intersection(i[0], i[1], i[2]);

    QCOMPARE(state, static_cast<int>(expectedState));
    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(intersection, expectedIntersection, LineIntersectionEpsilon));
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

void test_MathTools::setupComputeAngleData()
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

void test_MathTools::setupComputeAngleNaNData()
{
    QTest::addColumn<QVector3D>("vector1");
    QTest::addColumn<QVector3D>("vector2");

    QTest::newRow("Two Zero-valued vectors") << QVector3D(0, 0, 0) << QVector3D(0, 0, 0);
    QTest::newRow("First vector is zero-valued") << QVector3D(0, 0, 0) << QVector3D(1, 1, 0);
    QTest::newRow("Second vector is zero-valued") << QVector3D(1, 1, 0) << QVector3D(0, 0, 0) ;
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


void test_MathTools::getPointToClosestEdgeDistance_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<QList<QVector<double> > >("points");
    QTest::addColumn<bool>("lastToFirstEdge");
    QTest::addColumn<QVector3D>("point3D");
    QTest::addColumn<double>("expectedDistance");
    QTest::addColumn<int>("expectedClosestEdge");
    QTest::addColumn<QVector3D>("expectedClosestPoint");

    QList<QVector<double> > pointsInAxialView;
    pointsInAxialView << (QVector<double>() << -2.74219 << -4.71093 << 1153.41);
    pointsInAxialView << (QVector<double>() << -34.0234 << 32.9688 << 1153.41);
    pointsInAxialView << (QVector<double>() << 11.4766 << 59.2734 << 1153.41);
    pointsInAxialView << (QVector<double>() << 71.9062 << 20.1719 << 1153.41);
    pointsInAxialView << (QVector<double>() << 61.9531 << -11.8203 << 1153.41);
    pointsInAxialView << (QVector<double>() << 22.1406 << 20.8828 << 1153.41);
    QList<QVector<double> > pointsInSagitalView;
    pointsInSagitalView << (QVector<double>() << 3.9731 << 2.40157 << 1239.02);
    pointsInSagitalView << (QVector<double>() << 3.9731 << 28.0331 << 1241.06);
    pointsInSagitalView << (QVector<double>() << 3.9731 << 7.28377 << 1223.15);
    pointsInSagitalView << (QVector<double>() << 3.9731 << -13.8724 << 1236.17);
    pointsInSagitalView << (QVector<double>() << 3.9731 << -6.95596 << 1263.03);
    pointsInSagitalView << (QVector<double>() << 3.9731 << 9.31801 << 1256.11);

    QList<QVector<double> > pointsInCoronalView;
    pointsInCoronalView << (QVector<double>() << 24.3605 << 27.4214 << 1264.25);
    pointsInCoronalView << (QVector<double>() << 9.30705 << 27.4214 << 1245.94);
    pointsInCoronalView << (QVector<double>() << 22.7331 << 27.4214 << 1230.88);
    pointsInCoronalView << (QVector<double>() << -2.49158 << 27.4214 << 1232.92);
    pointsInCoronalView << (QVector<double>() << -13.0697 << 27.4214 << 1247.97);
    pointsInCoronalView << (QVector<double>() << -5.33952 << 27.4214 << 1260.58);

    QTest::newRow("axial. Closed. Edge0 out") << pointsInAxialView << true << QVector3D(-34.7344, 8.08594, 1153.41) << 16.441 << 0 << QVector3D(-22.0845,18.5877,1153.41);
    QTest::newRow("axial. Closed. Edge0 in") << pointsInAxialView << true << QVector3D(-9.14062, 23.7266, 1153.41) << 13.2416 << 0 << QVector3D(-19.3288,15.2684,1153.41);
    QTest::newRow("axial. Closed. Edge3 out") << pointsInAxialView << true << QVector3D(111.719, 72.0703, 1153.41) << 65.4101 << 2 << QVector3D(71.9062,20.1719,1153.41);
    QTest::newRow("axial. Closed. Edge5 on") << pointsInAxialView << true << QVector3D(15.0312, 15.1953, 1153.41) << 1.13275 << 5 << QVector3D(15.8434,14.4057,1153.41);
    QTest::newRow("axial. Opened. Edge5 on") << pointsInAxialView << false << QVector3D(15.0312, 15.1953, 1153.41) << 9.10444 << 4 << QVector3D(22.1406,20.8828,1153.41);

    QTest::newRow("sagital. Closed. Edge4 out") << pointsInSagitalView << true << QVector3D(3.9731, 1.99472, 1264.65) << 4.99872 << 4 << QVector3D(3.9731,0.0395213,1260.05);
    QTest::newRow("sagital. Closed. Edge3 in") << pointsInSagitalView << true << QVector3D(3.9731, -6.14226, 1254.08) << 3.02059 << 3 << QVector3D(3.9731,-9.06737,1254.83);
    QTest::newRow("sagital. Closed. Edge1 out") << pointsInSagitalView << true << QVector3D(3.9731, 45.5276, 1194.27) << 46.8536 << 1 << QVector3D(3.9731,14.9213,1229.74);
    QTest::newRow("sagital. Closed. Edge5 on") << pointsInSagitalView << true << QVector3D(3.9731, 4.84267, 1243.9) << 0.431003 << 5 << QVector3D(3.9731,4.44315,1244.07);
    QTest::newRow("sagital. Opened. Edge5 on") << pointsInSagitalView << false << QVector3D(3.9731, 4.84267, 1243.9) << 4.67376 << 0 << QVector3D(3.9731,5.21244,1239.25);

    QTest::newRow("coronal. Closed. Edge0 out") << pointsInCoronalView << true << QVector3D(53.2468, 27.4214, 1264.25) << 28.8863 << 0 << QVector3D(24.3605,27.4214,1264.25);
    QTest::newRow("coronal. Closed. Edge4 in") << pointsInCoronalView << true << QVector3D(-13.8834, 27.4214, 1256.92) << 5.37105 << 4 << QVector3D(-9.304,27.4214,1254.12);
    QTest::newRow("coronal. Closed. Edge2 out") << pointsInCoronalView << true << QVector3D(7.27281, 27.4214, 1169.45) << 62.4782 << 2 << QVector3D(12.2951,27.4214,1231.73);
    QTest::newRow("coronal. Closed. Edge5 on") << pointsInCoronalView << true << QVector3D(15.0029, 27.4214, 1262.21) << 0.873961 << 5 << QVector3D(14.896,27.4214,1263.08);
    QTest::newRow("coronal. Opened. Edge5 on") << pointsInCoronalView << false << QVector3D(15.0029, 27.4214, 1262.21) << 5.93604 << 0 << QVector3D(19.5881,27.4214,1258.44);
}
void test_MathTools::getPointToClosestEdgeDistance_ShouldReturnExpectedValues()
{
    QFETCH(QList<QVector<double> >, points);
    QFETCH(bool, lastToFirstEdge);
    QFETCH(QVector3D, point3D);
    QFETCH(double, expectedDistance);
    QFETCH(int, expectedClosestEdge);
    QFETCH(QVector3D, expectedClosestPoint);

    int closestEdge;
    double closestPoint[3];
    double inputPoint[3] = {point3D.x(), point3D.y(), point3D.z()};
    double distance = MathTools::getPointToClosestEdgeDistance(inputPoint, points, lastToFirstEdge, closestPoint, closestEdge);
    QVector3D closestPoint3D(closestPoint[0], closestPoint[1], closestPoint[2]);

    QString distanceError(QString("\nActual: %1\nExpected: %2").arg(distance).arg(expectedDistance));
    QString closestPointError(QString("\nActual: (%1, %2, %3)\nExpected: (%4, %5, %6)").arg(closestPoint[0]).arg(closestPoint[1]).arg(closestPoint[2])
                              .arg(expectedClosestPoint.x()).arg(expectedClosestPoint.y()).arg(expectedClosestPoint.z()));

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(closestPoint3D, expectedClosestPoint, 0.1), qPrintable(closestPointError));
    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(distance, expectedDistance, 0.1), qPrintable(distanceError));
    QCOMPARE(closestEdge, expectedClosestEdge);
}
DECLARE_TEST(test_MathTools)

#include "test_mathtools.moc"
