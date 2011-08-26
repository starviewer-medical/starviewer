#include "autotest.h"

#include <QVector3D>

#include "mathtools.h"
#include "fuzzycomparehelper.h"

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

private:
    void setupComputeAngleData();
    void setupComputeAngleNaNData();
    void setupCrossAndDotProductData();
};

// Epsilons pels diferents fuzzyCompare de cada test
const double AngleInDegreesEpsilon = 0.001;
const double AngleInRadiansEpsilon = 0.001;
const double NormalizeEpsilon = 0.000001;
const double ModulusEpsilon = 0.001;
const double crossProductEpsilon = 0.001;
const double directorVectorEpsilon = 0.001;

void test_MathTools::angleInRadians_ShouldComputeAngleInRadians_data()
{
    this->setupComputeAngleData();
}

void test_MathTools::angleInRadians_ShouldComputeAngleInRadians()
{
    QFETCH(QVector3D, vector1);
    QFETCH(QVector3D, vector2);
    QFETCH(double, angleInRadians);

    double v1[3];
    double v2[3];

    v1[0] = vector1.x();
    v1[1] = vector1.y();
    v1[2] = vector1.z();

    v2[0] = vector2.x();
    v2[1] = vector2.y();
    v2[2] = vector2.z();
    
    QVERIFY(FuzzyCompareHelper::fuzzyCompare(MathTools::angleInRadians(v1, v2), angleInRadians, AngleInRadiansEpsilon));
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

    double v1[3];
    double v2[3];

    v1[0] = vector1.x();
    v1[1] = vector1.y();
    v1[2] = vector1.z();

    v2[0] = vector2.x();
    v2[1] = vector2.y();
    v2[2] = vector2.z();
    
    QVERIFY(FuzzyCompareHelper::fuzzyCompare(MathTools::angleInDegrees(v1, v2), angleInRadians * MathTools::RadiansToDegreesAsDouble, AngleInDegreesEpsilon));
}

void test_MathTools::angleInRadians_ShouldReturnNaN_data()
{
    this->setupComputeAngleNaNData();
}

void test_MathTools::angleInRadians_ShouldReturnNaN()
{
    QFETCH(QVector3D, vector1);
    QFETCH(QVector3D, vector2);

    double v1[3];
    double v2[3];

    v1[0] = vector1.x();
    v1[1] = vector1.y();
    v1[2] = vector1.z();

    v2[0] = vector2.x();
    v2[1] = vector2.y();
    v2[2] = vector2.z();

    QVERIFY(MathTools::isNaN(MathTools::angleInRadians(v1, v2)));
}

void test_MathTools::angleInDegrees_ShouldReturnNaN_data()
{
    this->setupComputeAngleNaNData();
}

void test_MathTools::angleInDegrees_ShouldReturnNaN()
{
    QFETCH(QVector3D, vector1);
    QFETCH(QVector3D, vector2);

    double v1[3];
    double v2[3];

    v1[0] = vector1.x();
    v1[1] = vector1.y();
    v1[2] = vector1.z();

    v2[0] = vector2.x();
    v2[1] = vector2.y();
    v2[2] = vector2.z();

    QVERIFY(MathTools::isNaN(MathTools::angleInDegrees(v1, v2)));
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
    
    QVERIFY(FuzzyCompareHelper::fuzzyCompare(cVector[0], result.x(), NormalizeEpsilon));
    QVERIFY(FuzzyCompareHelper::fuzzyCompare(cVector[1], result.y(), NormalizeEpsilon));
    QVERIFY(FuzzyCompareHelper::fuzzyCompare(cVector[2], result.z(), NormalizeEpsilon));
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

    QVERIFY(FuzzyCompareHelper::fuzzyCompare(MathTools::modulus(cVector), modulus, ModulusEpsilon));
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

    QVERIFY(FuzzyCompareHelper::fuzzyCompare(MathTools::dotProduct(cVector1, cVector2), dotProduct));
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

    double cPoint1[3] = { firstPoint.x(), firstPoint.y(), firstPoint.z() };
    double cPoint2[3] = { secondPoint.x(), secondPoint.y(), secondPoint.z() };

    double *directorVector = MathTools::directorVector(cPoint1, cPoint2);

    QVERIFY(FuzzyCompareHelper::fuzzyCompare(directorVector[0], result.x(), directorVectorEpsilon));
    QVERIFY(FuzzyCompareHelper::fuzzyCompare(directorVector[1], result.y(), directorVectorEpsilon));
    QVERIFY(FuzzyCompareHelper::fuzzyCompare(directorVector[2], result.z(), directorVectorEpsilon));

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

    QVERIFY(FuzzyCompareHelper::fuzzyCompare(computedCrossProduct[0], crossProduct.x(), crossProductEpsilon));
    QVERIFY(FuzzyCompareHelper::fuzzyCompare(computedCrossProduct[1], crossProduct.y(), crossProductEpsilon));
    QVERIFY(FuzzyCompareHelper::fuzzyCompare(computedCrossProduct[2], crossProduct.z(), crossProductEpsilon));    
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

DECLARE_TEST(test_MathTools)

#include "test_mathtools.moc"
