#include "autotest.h"
#include "vector3.h"

#include "fuzzycomparetesthelper.h"

using namespace udg;
using namespace testing;

class test_Vector3Double : public QObject {

    Q_OBJECT

private slots:

    void dot_ShouldReturnExpectedValue_data();
    void dot_ShouldReturnExpectedValue();

    void cross_ShouldReturnExpectedValue_data();
    void cross_ShouldReturnExpectedValue();

    void length_ShouldReturnExpectedValue_data();
    void length_ShouldReturnExpectedValue();

    void lengthSquared_ShouldReturnExpectedValue_data();
    void lengthSquared_ShouldReturnExpectedValue();

    void normalize_ShouldNormalizeAndReturnVector_data();
    void normalize_ShouldNormalizeAndReturnVector();

    void operatorEquals_ShouldReturnExpectedValue_data();
    void operatorEquals_ShouldReturnExpectedValue();

    void operatorNotEquals_ShouldReturnExpectedValue_data();
    void operatorNotEquals_ShouldReturnExpectedValue();

    void operatorPositive_ShouldReturnSameVector_data();
    void operatorPositive_ShouldReturnSameVector();

    void operatorNegative_ShouldReturnOpposite_data();
    void operatorNegative_ShouldReturnOpposite();

    void operatorPlus_ShouldReturnExpectedValue_data();
    void operatorPlus_ShouldReturnExpectedValue();

    void operatorPlusEqual_ShouldModifyVectorAndReturnIt_data();
    void operatorPlusEqual_ShouldModifyVectorAndReturnIt();

    void operatorMinus_ShouldReturnExpectedValue_data();
    void operatorMinus_ShouldReturnExpectedValue();

    void operatorMinusEqual_ShouldModifyVectorAndReturnIt_data();
    void operatorMinusEqual_ShouldModifyVectorAndReturnIt();

    void operatorDotProduct_ShouldReturnExpectedValue_data();
    void operatorDotProduct_ShouldReturnExpectedValue();

    void operatorCrossProduct_ShouldReturnExpectedValue_data();
    void operatorCrossProduct_ShouldReturnExpectedValue();

    void toString_ShoudReturnExpectedValue_data();
    void toString_ShoudReturnExpectedValue();

    void operatorQString_ShouldReturnExpectedValue_data();
    void operatorQString_ShouldReturnExpectedValue();

    void operatorProductByScalarLeft_ShouldReturnExpectedValue_data();
    void operatorProductByScalarLeft_ShouldReturnExpectedValue();

    void operatorProductByScalarRight_ShouldReturnExpectedValue_data();
    void operatorProductByScalarRight_ShouldReturnExpectedValue();

    void operatorProductByScalarEqual_ShouldModifyVectorAndReturnIt_data();
    void operatorProductByScalarEqual_ShouldModifyVectorAndReturnIt();

    void operatorDivisionByScalar_ShouldReturnExpectedValue_data();
    void operatorDivisionByScalar_ShouldReturnExpectedValue();

    void operatorDivisionByScalarEqual_ShouldModifyVectorAndReturnIt_data();
    void operatorDivisionByScalarEqual_ShouldModifyVectorAndReturnIt();

    void operatorQDataStreamIn_ShouldFillVectorAndReturnStream_data();
    void operatorQDataStreamIn_ShouldFillVectorAndReturnStream();

    void operatorQDataStreamOut_ShouldWriteToStreamAndReturnIt_data();
    void operatorQDataStreamOut_ShouldWriteToStreamAndReturnIt();

private:

    void setupDotProductData();
    void setupCrossProductData();
    void setupAdditionData();
    void setupSubtractionData();
    void setupToStringData();
    void setupProductByScalarData();
    void setupDivisionByScalarData();

};

Q_DECLARE_METATYPE(Vector3)
Q_DECLARE_METATYPE(QSharedPointer<QByteArray>)
Q_DECLARE_METATYPE(QSharedPointer<QDataStream>)

void test_Vector3Double::dot_ShouldReturnExpectedValue_data()
{
    setupDotProductData();
}

void test_Vector3Double::dot_ShouldReturnExpectedValue()
{
    QFETCH(Vector3, vector1);
    QFETCH(Vector3, vector2);
    QFETCH(double, expectedResult);

    double result = Vector3::dot(vector1, vector2);

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(result).arg(expectedResult).arg(result - expectedResult)));
}

void test_Vector3Double::cross_ShouldReturnExpectedValue_data()
{
    setupCrossProductData();
}

void test_Vector3Double::cross_ShouldReturnExpectedValue()
{
    QFETCH(Vector3, vector1);
    QFETCH(Vector3, vector2);
    QFETCH(Vector3, expectedResult);

    Vector3 result = Vector3::cross(vector1, vector2);

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(result).arg(expectedResult).arg((result - expectedResult).toString())));
}

void test_Vector3Double::length_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<Vector3>("vector");
    QTest::addColumn<double>("expectedLength");

    QTest::newRow("zero") << Vector3() << 0.0;
    QTest::newRow("i") << Vector3(1, 0, 0) << 1.0;
    QTest::newRow("-i") << Vector3(-1, 0, 0) << 1.0;
    QTest::newRow("j") << Vector3(0, 1, 0) << 1.0;
    QTest::newRow("-j") << Vector3(0, -1, 0) << 1.0;
    QTest::newRow("k") << Vector3(0, 0, 1) << 1.0;
    QTest::newRow("-k") << Vector3(0, 0, -1) << 1.0;
    QTest::newRow("i+j") << Vector3(1, 1, 0) << 1.4142135623730951;
    QTest::newRow("i+k") << Vector3(1, 0, 1) << 1.4142135623730951;
    QTest::newRow("j+k") << Vector3(0, 1, 1) << 1.4142135623730951;
    QTest::newRow("i+j+k") << Vector3(1, 1, 1) << 1.7320508075688772;
    QTest::newRow("random #1") << Vector3(-1, -4, -5) << 6.48074069840786;
    QTest::newRow("random #2") << Vector3(1, 3, -5) << 5.916079783099616;
    QTest::newRow("random #3") << Vector3(-3, 0, 4) << 5.0;
}

void test_Vector3Double::length_ShouldReturnExpectedValue()
{
    QFETCH(Vector3, vector);
    QFETCH(double, expectedLength);

    double length = vector.length();

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(length, expectedLength),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(length).arg(expectedLength).arg(length - expectedLength)));
}

void test_Vector3Double::lengthSquared_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<Vector3>("vector");
    QTest::addColumn<double>("expectedLengthSquared");

    QTest::newRow("zero") << Vector3() << 0.0;
    QTest::newRow("i") << Vector3(1, 0, 0) << 1.0;
    QTest::newRow("-i") << Vector3(-1, 0, 0) << 1.0;
    QTest::newRow("j") << Vector3(0, 1, 0) << 1.0;
    QTest::newRow("-j") << Vector3(0, -1, 0) << 1.0;
    QTest::newRow("k") << Vector3(0, 0, 1) << 1.0;
    QTest::newRow("-k") << Vector3(0, 0, -1) << 1.0;
    QTest::newRow("i+j") << Vector3(1, 1, 0) << 2.0;
    QTest::newRow("i+k") << Vector3(1, 0, 1) << 2.0;
    QTest::newRow("j+k") << Vector3(0, 1, 1) << 2.0;
    QTest::newRow("i+j+k") << Vector3(1, 1, 1) << 3.0;
    QTest::newRow("random #1") << Vector3(-1, -4, -5) << 42.0;
    QTest::newRow("random #2") << Vector3(1, 3, -5) << 35.0;
    QTest::newRow("random #3") << Vector3(-3, 0, 4) << 25.0;
}

void test_Vector3Double::lengthSquared_ShouldReturnExpectedValue()
{
    QFETCH(Vector3, vector);
    QFETCH(double, expectedLengthSquared);

    double lengthSquared = vector.lengthSquared();

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(lengthSquared, expectedLengthSquared),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(lengthSquared).arg(expectedLengthSquared)
                                                                          .arg(lengthSquared - expectedLengthSquared)));
}

void test_Vector3Double::normalize_ShouldNormalizeAndReturnVector_data()
{
    QTest::addColumn<Vector3>("vector");
    QTest::addColumn<Vector3>("expectedResult");

    QTest::newRow("i") << Vector3(1, 0, 0) << Vector3(1, 0, 0);
    QTest::newRow("-i") << Vector3(-1, 0, 0) << Vector3(-1, 0, 0);
    QTest::newRow("j") << Vector3(0, 1, 0) << Vector3(0, 1, 0);
    QTest::newRow("-j") << Vector3(0, -1, 0) << Vector3(0, -1, 0);
    QTest::newRow("k") << Vector3(0, 0, 1) << Vector3(0, 0, 1);
    QTest::newRow("-k") << Vector3(0, 0, -1) << Vector3(0, 0, -1);
    QTest::newRow("i+j") << Vector3(1, 1, 0) << Vector3(0.7071067811865475, 0.7071067811865475, 0);
    QTest::newRow("i+k") << Vector3(1, 0, 1) << Vector3(0.7071067811865475, 0, 0.7071067811865475);
    QTest::newRow("j+k") << Vector3(0, 1, 1) << Vector3(0, 0.7071067811865475, 0.7071067811865475);
    QTest::newRow("i+j+k") << Vector3(1, 1, 1) << Vector3(0.5773502691896258, 0.5773502691896258, 0.5773502691896258);
    QTest::newRow("random #1") << Vector3(-1, -4, -5) << Vector3(-0.1543033499620919, -0.6172133998483676, -0.7715167498104595);
    QTest::newRow("random #2") << Vector3(1, 3, -5) << Vector3(0.1690308509457033, 0.50709255283711, -0.8451542547285166);
    QTest::newRow("random #3") << Vector3(-3, 0, 4) << Vector3(-0.6, 0, 0.8);
}

void test_Vector3Double::normalize_ShouldNormalizeAndReturnVector()
{
    QFETCH(Vector3, vector);
    QFETCH(Vector3, expectedResult);

    Vector3 result = vector.normalize();

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(vector, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(vector).arg(expectedResult).arg((vector - expectedResult).toString())));
    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(result).arg(expectedResult).arg((result - expectedResult).toString())));
}

void test_Vector3Double::operatorEquals_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<Vector3>("vector1");
    QTest::addColumn<Vector3>("vector2");
    QTest::addColumn<bool>("expectedResult");

    QTest::newRow("equal #1") << Vector3(-1, -5, -3) << Vector3(-1, -5, -3) << true;
    QTest::newRow("equal #2") << Vector3(3, 1, 5) << Vector3(3, 1, 5) << true;
    QTest::newRow("equal #3") << Vector3(4, -5, -3) << Vector3(4, -5, -3) << true;
    QTest::newRow("different #1") << Vector3(3, -1, 4) << Vector3(3, -3, 2) << false;
    QTest::newRow("different #2") << Vector3(0, 5, -1) << Vector3(-4, 5, 0) << false;
    QTest::newRow("different #3") << Vector3(2, -1, -1) << Vector3(0, -1, -3) << false;
}

void test_Vector3Double::operatorEquals_ShouldReturnExpectedValue()
{
    QFETCH(Vector3, vector1);
    QFETCH(Vector3, vector2);
    QFETCH(bool, expectedResult);

    QCOMPARE(vector1 == vector2, expectedResult);
}

void test_Vector3Double::operatorNotEquals_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<Vector3>("vector1");
    QTest::addColumn<Vector3>("vector2");
    QTest::addColumn<bool>("expectedResult");

    QTest::newRow("equal #1") << Vector3(-1, -5, -3) << Vector3(-1, -5, -3) << false;
    QTest::newRow("equal #2") << Vector3(3, 1, 5) << Vector3(3, 1, 5) << false;
    QTest::newRow("equal #3") << Vector3(4, -5, -3) << Vector3(4, -5, -3) << false;
    QTest::newRow("different #1") << Vector3(3, -1, 4) << Vector3(3, -3, 2) << true;
    QTest::newRow("different #2") << Vector3(0, 5, -1) << Vector3(-4, 5, 0) << true;
    QTest::newRow("different #3") << Vector3(2, -1, -1) << Vector3(0, -1, -3) << true;
}

void test_Vector3Double::operatorNotEquals_ShouldReturnExpectedValue()
{
    QFETCH(Vector3, vector1);
    QFETCH(Vector3, vector2);
    QFETCH(bool, expectedResult);

    QCOMPARE(vector1 != vector2, expectedResult);
}

void test_Vector3Double::operatorPositive_ShouldReturnSameVector_data()
{
    QTest::addColumn<Vector3>("vector");

    QTest::newRow("zero") << Vector3();
    QTest::newRow("-zero") << Vector3(-0.0, -0.0, -0.0);
    QTest::newRow("i") << Vector3(1, 0, 0);
    QTest::newRow("-i") << Vector3(-1, 0, 0);
    QTest::newRow("j") << Vector3(0, 1, 0);
    QTest::newRow("-j") << Vector3(0, -1, 0);
    QTest::newRow("k") << Vector3(0, 0, 1);
    QTest::newRow("-k") << Vector3(0, 0, -1);
    QTest::newRow("random #1") << Vector3(5, -5, 4);
    QTest::newRow("random #2") << Vector3(-4, 2, -3);
    QTest::newRow("random #3") << Vector3(0, 4, 0);
}

void test_Vector3Double::operatorPositive_ShouldReturnSameVector()
{
    QFETCH(Vector3, vector);

    Vector3 positive = +vector;

    QVERIFY2(positive == vector, qPrintable(QString("actual: %1, expected %2, difference: %3").arg(positive).arg(vector).arg((positive - vector).toString())));
}

void test_Vector3Double::operatorNegative_ShouldReturnOpposite_data()
{
    QTest::addColumn<Vector3>("vector");
    QTest::addColumn<Vector3>("opposite");

    QTest::newRow("zero") << Vector3() << Vector3(-0.0, -0.0, -0.0);
    QTest::newRow("-zero") << Vector3(-0.0, -0.0, -0.0) << Vector3();
    QTest::newRow("i") << Vector3(1, 0, 0) << Vector3(-1, 0, 0);
    QTest::newRow("-i") << Vector3(-1, 0, 0) << Vector3(1, 0, 0);
    QTest::newRow("j") << Vector3(0, 1, 0) << Vector3(0, -1, 0);
    QTest::newRow("-j") << Vector3(0, -1, 0) << Vector3(0, 1, 0);
    QTest::newRow("k") << Vector3(0, 0, 1) << Vector3(0, 0, -1);
    QTest::newRow("-k") << Vector3(0, 0, -1) << Vector3(0, 0, 1);
    QTest::newRow("random #1") << Vector3(5, -5, 4) << Vector3(-5, 5, -4);
    QTest::newRow("random #2") << Vector3(-4, 2, -3) << Vector3(4, -2, 3);
    QTest::newRow("random #3") << Vector3(0, 4, 0) << Vector3(0, -4, 0);
}

void test_Vector3Double::operatorNegative_ShouldReturnOpposite()
{
    QFETCH(Vector3, vector);
    QFETCH(Vector3, opposite);

    Vector3 negative = -vector;

    QVERIFY2(negative == opposite,
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(negative).arg(opposite).arg((negative - opposite).toString())));
}

void test_Vector3Double::operatorPlus_ShouldReturnExpectedValue_data()
{
    setupAdditionData();
}

void test_Vector3Double::operatorPlus_ShouldReturnExpectedValue()
{
    QFETCH(Vector3, vector1);
    QFETCH(Vector3, vector2);
    QFETCH(Vector3, expectedResult);

    Vector3 result = vector1 + vector2;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(result).arg(expectedResult).arg((result - expectedResult).toString())));
}

void test_Vector3Double::operatorPlusEqual_ShouldModifyVectorAndReturnIt_data()
{
    setupAdditionData();
}

void test_Vector3Double::operatorPlusEqual_ShouldModifyVectorAndReturnIt()
{
    QFETCH(Vector3, vector1);
    QFETCH(Vector3, vector2);
    QFETCH(Vector3, expectedResult);

    Vector3 &result = vector1 += vector2;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(vector1, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(vector1).arg(expectedResult).arg((vector1 - expectedResult).toString())));
    QCOMPARE(&result, &vector1);
}

void test_Vector3Double::operatorMinus_ShouldReturnExpectedValue_data()
{
    setupSubtractionData();
}

void test_Vector3Double::operatorMinus_ShouldReturnExpectedValue()
{
    QFETCH(Vector3, vector1);
    QFETCH(Vector3, vector2);
    QFETCH(Vector3, expectedResult);

    Vector3 result = vector1 - vector2;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(result).arg(expectedResult).arg((result - expectedResult).toString())));
}

void test_Vector3Double::operatorMinusEqual_ShouldModifyVectorAndReturnIt_data()
{
    setupSubtractionData();
}

void test_Vector3Double::operatorMinusEqual_ShouldModifyVectorAndReturnIt()
{
    QFETCH(Vector3, vector1);
    QFETCH(Vector3, vector2);
    QFETCH(Vector3, expectedResult);

    Vector3 &result = vector1 -= vector2;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(vector1, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(vector1).arg(expectedResult).arg((vector1 - expectedResult).toString())));
    QCOMPARE(&result, &vector1);
}

void test_Vector3Double::operatorDotProduct_ShouldReturnExpectedValue_data()
{
    setupDotProductData();
}

void test_Vector3Double::operatorDotProduct_ShouldReturnExpectedValue()
{
    QFETCH(Vector3, vector1);
    QFETCH(Vector3, vector2);
    QFETCH(double, expectedResult);

    double result = vector1 * vector2;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(result).arg(expectedResult).arg(result - expectedResult)));
}

void test_Vector3Double::operatorCrossProduct_ShouldReturnExpectedValue_data()
{
    setupCrossProductData();
}

void test_Vector3Double::operatorCrossProduct_ShouldReturnExpectedValue()
{
    QFETCH(Vector3, vector1);
    QFETCH(Vector3, vector2);
    QFETCH(Vector3, expectedResult);

    Vector3 result = vector1 ^ vector2;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(result).arg(expectedResult).arg((result - expectedResult).toString())));
}

void test_Vector3Double::toString_ShoudReturnExpectedValue_data()
{
    setupToStringData();
}

void test_Vector3Double::toString_ShoudReturnExpectedValue()
{
    QFETCH(Vector3, vector);
    QFETCH(QString, expectedString);

    QCOMPARE(vector.toString(), expectedString);
}

void test_Vector3Double::operatorQString_ShouldReturnExpectedValue_data()
{
    setupToStringData();
}

void test_Vector3Double::operatorQString_ShouldReturnExpectedValue()
{
    QFETCH(Vector3, vector);
    QFETCH(QString, expectedString);

    QCOMPARE(QString(vector), expectedString);
}

void test_Vector3Double::operatorProductByScalarLeft_ShouldReturnExpectedValue_data()
{
    setupProductByScalarData();
}

void test_Vector3Double::operatorProductByScalarLeft_ShouldReturnExpectedValue()
{
    QFETCH(double, scalar);
    QFETCH(Vector3, vector);
    QFETCH(Vector3, expectedResult);

    Vector3 result = scalar * vector;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(result).arg(expectedResult).arg((result - expectedResult).toString())));
}

void test_Vector3Double::operatorProductByScalarRight_ShouldReturnExpectedValue_data()
{
    setupProductByScalarData();
}

void test_Vector3Double::operatorProductByScalarRight_ShouldReturnExpectedValue()
{
    QFETCH(Vector3, vector);
    QFETCH(double, scalar);
    QFETCH(Vector3, expectedResult);

    Vector3 result = vector * scalar;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(result).arg(expectedResult).arg((result - expectedResult).toString())));
}

void test_Vector3Double::operatorProductByScalarEqual_ShouldModifyVectorAndReturnIt_data()
{
    setupProductByScalarData();
}

void test_Vector3Double::operatorProductByScalarEqual_ShouldModifyVectorAndReturnIt()
{
    QFETCH(Vector3, vector);
    QFETCH(double, scalar);
    QFETCH(Vector3, expectedResult);

    Vector3 &result = vector *= scalar;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(vector, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(vector).arg(expectedResult).arg((vector - expectedResult).toString())));
    QCOMPARE(&result, &vector);
}

void test_Vector3Double::operatorDivisionByScalar_ShouldReturnExpectedValue_data()
{
    setupDivisionByScalarData();
}

void test_Vector3Double::operatorDivisionByScalar_ShouldReturnExpectedValue()
{
    QFETCH(Vector3, vector);
    QFETCH(double, scalar);
    QFETCH(Vector3, expectedResult);

    Vector3 result = vector / scalar;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(result).arg(expectedResult).arg((result - expectedResult).toString())));
}

void test_Vector3Double::operatorDivisionByScalarEqual_ShouldModifyVectorAndReturnIt_data()
{
    setupDivisionByScalarData();
}

void test_Vector3Double::operatorDivisionByScalarEqual_ShouldModifyVectorAndReturnIt()
{
    QFETCH(Vector3, vector);
    QFETCH(double, scalar);
    QFETCH(Vector3, expectedResult);

    Vector3 &result = vector /= scalar;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(vector, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(vector).arg(expectedResult).arg((vector - expectedResult).toString())));
    QCOMPARE(&result, &vector);
}

void test_Vector3Double::operatorQDataStreamIn_ShouldFillVectorAndReturnStream_data()
{
    // La primera serveix perquè no ens esborri el byte array
    QTest::addColumn<QSharedPointer<QByteArray>>("byteArray");
    QTest::addColumn<QSharedPointer<QDataStream>>("stream");
    QTest::addColumn<Vector3>("filledVector");

    {
        QSharedPointer<QByteArray> byteArray(new QByteArray());
        QSharedPointer<QDataStream> stream(new QDataStream(byteArray.data(), QIODevice::ReadWrite));
        Vector3 vector;
        (*stream) << vector.x << vector.y << vector.z;
        stream->device()->reset();
        QTest::newRow("zero") << byteArray << stream << vector;
    }

    {
        QSharedPointer<QByteArray> byteArray(new QByteArray());
        QSharedPointer<QDataStream> stream(new QDataStream(byteArray.data(), QIODevice::ReadWrite));
        Vector3 vector(-0, -0, -0);
        (*stream) << vector.x << vector.y << vector.z;
        stream->device()->reset();
        QTest::newRow("-zero") << byteArray << stream << vector;
    }

    {
        QSharedPointer<QByteArray> byteArray(new QByteArray());
        QSharedPointer<QDataStream> stream(new QDataStream(byteArray.data(), QIODevice::ReadWrite));
        Vector3 vector(4, 5, -4);
        (*stream) << vector.x << vector.y << vector.z;
        stream->device()->reset();
        QTest::newRow("random #1") << byteArray << stream << vector;
    }

    {
        QSharedPointer<QByteArray> byteArray(new QByteArray());
        QSharedPointer<QDataStream> stream(new QDataStream(byteArray.data(), QIODevice::ReadWrite));
        Vector3 vector(-4, -1, -3);
        (*stream) << vector.x << vector.y << vector.z;
        stream->device()->reset();
        QTest::newRow("random #2") << byteArray << stream << vector;
    }

    {
        QSharedPointer<QByteArray> byteArray(new QByteArray());
        QSharedPointer<QDataStream> stream(new QDataStream(byteArray.data(), QIODevice::ReadWrite));
        Vector3 vector(2, -1, -5);
        (*stream) << vector.x << vector.y << vector.z;
        stream->device()->reset();
        QTest::newRow("random #3") << byteArray << stream << vector;
    }
}

void test_Vector3Double::operatorQDataStreamIn_ShouldFillVectorAndReturnStream()
{
    QFETCH(QSharedPointer<QDataStream>, stream);
    QFETCH(Vector3, filledVector);

    Vector3 vector;
    QDataStream &returnedStream = (*stream) >> vector;
    
    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(vector, filledVector),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(vector).arg(filledVector).arg((vector - filledVector).toString())));
    QCOMPARE(&returnedStream, stream.data());
}

void test_Vector3Double::operatorQDataStreamOut_ShouldWriteToStreamAndReturnIt_data()
{
    QTest::addColumn<Vector3>("vector");
    // Aquesta serveix perquè no ens esborri el byte array
    QTest::addColumn<QSharedPointer<QByteArray>>("byteArray");
    QTest::addColumn<QSharedPointer<QDataStream>>("filledStream");

    {
        Vector3 vector;
        QSharedPointer<QByteArray> byteArray(new QByteArray());
        QSharedPointer<QDataStream> stream(new QDataStream(byteArray.data(), QIODevice::ReadWrite));
        (*stream) << vector.x << vector.y << vector.z;
        QTest::newRow("zero") << vector << byteArray << stream;
    }

    {
        Vector3 vector(-0, -0, -0);
        QSharedPointer<QByteArray> byteArray(new QByteArray());
        QSharedPointer<QDataStream> stream(new QDataStream(byteArray.data(), QIODevice::ReadWrite));
        (*stream) << vector.x << vector.y << vector.z;
        QTest::newRow("-zero") << vector << byteArray << stream;
    }

    {
        Vector3 vector(4, 5, -4);
        QSharedPointer<QByteArray> byteArray(new QByteArray());
        QSharedPointer<QDataStream> stream(new QDataStream(byteArray.data(), QIODevice::ReadWrite));
        (*stream) << vector.x << vector.y << vector.z;
        QTest::newRow("random #1") << vector << byteArray << stream;
    }

    {
        Vector3 vector(-4, -1, -3);
        QSharedPointer<QByteArray> byteArray(new QByteArray());
        QSharedPointer<QDataStream> stream(new QDataStream(byteArray.data(), QIODevice::ReadWrite));
        (*stream) << vector.x << vector.y << vector.z;
        QTest::newRow("random #2") << vector << byteArray << stream;
    }

    {
        Vector3 vector(2, -1, -5);
        QSharedPointer<QByteArray> byteArray(new QByteArray());
        QSharedPointer<QDataStream> stream(new QDataStream(byteArray.data(), QIODevice::ReadWrite));
        (*stream) << vector.x << vector.y << vector.z;
        QTest::newRow("random #3") << vector << byteArray << stream;
    }
}

void test_Vector3Double::operatorQDataStreamOut_ShouldWriteToStreamAndReturnIt()
{
    QFETCH(Vector3, vector);
    QFETCH(QSharedPointer<QDataStream>, filledStream);

    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::ReadWrite);
    QDataStream &returnedStream = stream << vector;

    stream.device()->reset();
    filledStream->device()->reset();

    QCOMPARE(stream.device()->bytesAvailable(), filledStream->device()->bytesAvailable());
    QCOMPARE(stream.device()->read(stream.device()->bytesAvailable()), filledStream->device()->read(filledStream->device()->bytesAvailable()));
    QCOMPARE(&returnedStream, &stream);
}

void test_Vector3Double::setupDotProductData()
{
    QTest::addColumn<Vector3>("vector1");
    QTest::addColumn<Vector3>("vector2");
    QTest::addColumn<double>("expectedResult");

    QTest::newRow("zero · zero") << Vector3() << Vector3() << 0.0;
    QTest::newRow("zero · non-zero") << Vector3() << Vector3(1, 1, 1) << 0.0;
    QTest::newRow("non-zero · zero") << Vector3(1, 1, 1) << Vector3() << 0.0;
    QTest::newRow("same vectors") << Vector3(-2, -3, -3) << Vector3(-2, -3, -3) << 22.0;
    QTest::newRow("parallel") << Vector3(-3, -5, -2) << Vector3(-1.5, -2.5, -1) << 19.0;
    QTest::newRow("perpendicular") << Vector3(1, 0, 0) << Vector3(0, 1, 0) << 0.0;
    QTest::newRow("opposite") << Vector3(-3, 4, 5) << Vector3(1.5, -2, -2.5) << -25.0;
    QTest::newRow("random #1") << Vector3(1, 3, -5) << Vector3(4, -2, -1) << 3.0;
    QTest::newRow("random #2") << Vector3(3, -4, -2) << Vector3(0, 1, -5) << 6.0;
    QTest::newRow("random #3") << Vector3(-2, -5, -3) << Vector3(-3, 0, 1) << 3.0;
}

void test_Vector3Double::setupCrossProductData()
{
    QTest::addColumn<Vector3>("vector1");
    QTest::addColumn<Vector3>("vector2");
    QTest::addColumn<Vector3>("expectedResult");

    QTest::newRow("zero × zero") << Vector3() << Vector3() << Vector3();
    QTest::newRow("zero × non-zero") << Vector3() << Vector3(1, 1, 1) << Vector3();
    QTest::newRow("non-zero × zero") << Vector3(1, 1, 1) << Vector3() << Vector3();
    QTest::newRow("same vectors") << Vector3(-2, -3, -3) << Vector3(-2, -3, -3) << Vector3();
    QTest::newRow("parallel") << Vector3(-3, -5, -2) << Vector3(-1.5, -2.5, -1) << Vector3();
    QTest::newRow("perpendicular") << Vector3(1, 0, 0) << Vector3(0, 1, 0) << Vector3(0, 0, 1);
    QTest::newRow("opposite") << Vector3(-3, 4, 5) << Vector3(1.5, -2, -2.5) << Vector3();
    QTest::newRow("random #1") << Vector3(1, 3, -5) << Vector3(4, -2, -1) << Vector3(-13, -19, -14);
    QTest::newRow("random #2") << Vector3(3, -4, -2) << Vector3(0, 1, -5) << Vector3(22, 15, 3);
    QTest::newRow("random #3") << Vector3(-2, -5, -3) << Vector3(-3, 0, 1) << Vector3(-5, 11, -15);
}

void test_Vector3Double::setupAdditionData()
{
    QTest::addColumn<Vector3>("vector1");
    QTest::addColumn<Vector3>("vector2");
    QTest::addColumn<Vector3>("expectedResult");

    QTest::newRow("zero + zero") << Vector3() << Vector3() << Vector3();
    QTest::newRow("zero + non-zero") << Vector3() << Vector3(1, 1, 1) << Vector3(1, 1, 1);
    QTest::newRow("non-zero + zero") << Vector3(1, 1, 1) << Vector3() << Vector3(1, 1, 1);
    QTest::newRow("+ i") << Vector3(-5, -4, 0) << Vector3(1, 0, 0) << Vector3(-4, -4, 0);
    QTest::newRow("+ (-i)") << Vector3(-5, -4, 0) << Vector3(-1, 0, 0) << Vector3(-6, -4, 0);
    QTest::newRow("+ j") << Vector3(-5, -4, 0) << Vector3(0, 1, 0) << Vector3(-5, -3, 0);
    QTest::newRow("+ (-j)") << Vector3(-5, -4, 0) << Vector3(0, -1, 0) << Vector3(-5, -5, 0);
    QTest::newRow("+ k") << Vector3(-5, -4, 0) << Vector3(0, 0, 1) << Vector3(-5, -4, 1);
    QTest::newRow("+ (-k)") << Vector3(-5, -4, 0) << Vector3(0, 0, -1) << Vector3(-5, -4, -1);
    QTest::newRow("random #1") << Vector3(1, 3, -5) << Vector3(4, -2, -1) << Vector3(5, 1, -6);
    QTest::newRow("random #2") << Vector3(3, -4, -2) << Vector3(0, 1, -5) << Vector3(3, -3, -7);
    QTest::newRow("random #3") << Vector3(-2, -5, -3) << Vector3(-3, 0, 1) << Vector3(-5, -5, -2);
}

void test_Vector3Double::setupSubtractionData()
{
    QTest::addColumn<Vector3>("vector1");
    QTest::addColumn<Vector3>("vector2");
    QTest::addColumn<Vector3>("expectedResult");

    QTest::newRow("zero - zero") << Vector3() << Vector3() << Vector3();
    QTest::newRow("zero - non-zero") << Vector3() << Vector3(1, 1, 1) << Vector3(-1, -1, -1);
    QTest::newRow("non-zero - zero") << Vector3(1, 1, 1) << Vector3() << Vector3(1, 1, 1);
    QTest::newRow("- i") << Vector3(-5, -4, 0) << Vector3(1, 0, 0) << Vector3(-6, -4, 0);
    QTest::newRow("- (-i)") << Vector3(-5, -4, 0) << Vector3(-1, 0, 0) << Vector3(-4, -4, 0);
    QTest::newRow("- j") << Vector3(-5, -4, 0) << Vector3(0, 1, 0) << Vector3(-5, -5, 0);
    QTest::newRow("- (-j)") << Vector3(-5, -4, 0) << Vector3(0, -1, 0) << Vector3(-5, -3, 0);
    QTest::newRow("- k") << Vector3(-5, -4, 0) << Vector3(0, 0, 1) << Vector3(-5, -4, -1);
    QTest::newRow("- (-k)") << Vector3(-5, -4, 0) << Vector3(0, 0, -1) << Vector3(-5, -4, 1);
    QTest::newRow("random #1") << Vector3(1, 3, -5) << Vector3(4, -2, -1) << Vector3(-3, 5, -4);
    QTest::newRow("random #2") << Vector3(3, -4, -2) << Vector3(0, 1, -5) << Vector3(3, -5, 3);
    QTest::newRow("random #3") << Vector3(-2, -5, -3) << Vector3(-3, 0, 1) << Vector3(1, -5, -4);
}

void test_Vector3Double::setupToStringData()
{
    QTest::addColumn<Vector3>("vector");
    QTest::addColumn<QString>("expectedString");

    QTest::newRow("zero") << Vector3() << "(0, 0, 0)";
    QTest::newRow("-zero") << Vector3(-0.0, -0.0, -0.0) << "(0, 0, 0)";
    QTest::newRow("random #1") << Vector3(1, -2, 0) << "(1, -2, 0)";
    QTest::newRow("random #2") << Vector3(2.5, -3, -3.4) << "(2.5, -3, -3.4)";
    QTest::newRow("random #3") << Vector3(3.9875905695, 0.4293503326, 5.5895753834) << "(3.98759, 0.42935, 5.58958)";
}

void test_Vector3Double::setupProductByScalarData()
{
    QTest::addColumn<double>("scalar");
    QTest::addColumn<Vector3>("vector");
    QTest::addColumn<Vector3>("expectedResult");

    QTest::newRow("0 * (0,0,0)") << 0.0 << Vector3() << Vector3();
    QTest::newRow("0 * (x,y,z)") << 0.0 << Vector3(1, 1, 1) << Vector3();
    QTest::newRow("a * (0,0,0)") << 1.0 << Vector3() << Vector3();
    QTest::newRow("random #1") << 2.0 << Vector3(-1, 1, -3) << Vector3(-2, 2, -6);
    QTest::newRow("random #2") << 4.0 << Vector3(1, -5, 3) << Vector3(4, -20, 12);
    QTest::newRow("random #3") << -5.0 << Vector3(1, -2, 0) << Vector3(-5, 10, 0);
}

void test_Vector3Double::setupDivisionByScalarData()
{
    QTest::addColumn<Vector3>("vector");
    QTest::addColumn<double>("scalar");
    QTest::addColumn<Vector3>("expectedResult");

    QTest::newRow("(0,0,0) / a") << Vector3() << 1.0 << Vector3();
    QTest::newRow("(x,y,z) / 1") << Vector3(0, -3, -5) << 1.0 << Vector3(0, -3, -5);
    QTest::newRow("random #1") << Vector3(4, 3, 5) << 3.0 << Vector3(1.3333333333333333, 1, 1.6666666666666667);
    QTest::newRow("random #2") << Vector3(2, 5, 1) << 3.0 << Vector3(0.6666666666666666, 1.6666666666666667, 0.3333333333333333);
    QTest::newRow("random #3") << Vector3(4, -3, -5) << 2.0 << Vector3(2, -1.5, -2.5);
}

DECLARE_TEST(test_Vector3Double)

#include "test_vector3double.moc"
