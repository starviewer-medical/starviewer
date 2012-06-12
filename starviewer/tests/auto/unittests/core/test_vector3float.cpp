#include "autotest.h"
#include "vector3.h"

#include "fuzzycomparetesthelper.h"

using namespace udg;
using namespace testing;

class test_Vector3Float : public QObject {

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

Q_DECLARE_METATYPE(Vector3Float)
Q_DECLARE_METATYPE(QSharedPointer<QByteArray>)
Q_DECLARE_METATYPE(QSharedPointer<QDataStream>)

void test_Vector3Float::dot_ShouldReturnExpectedValue_data()
{
    setupDotProductData();
}

void test_Vector3Float::dot_ShouldReturnExpectedValue()
{
    QFETCH(Vector3Float, vector1);
    QFETCH(Vector3Float, vector2);
    QFETCH(float, expectedResult);

    float result = Vector3Float::dot(vector1, vector2);

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(result).arg(expectedResult).arg(result - expectedResult)));
}

void test_Vector3Float::cross_ShouldReturnExpectedValue_data()
{
    setupCrossProductData();
}

void test_Vector3Float::cross_ShouldReturnExpectedValue()
{
    QFETCH(Vector3Float, vector1);
    QFETCH(Vector3Float, vector2);
    QFETCH(Vector3Float, expectedResult);

    Vector3Float result = Vector3Float::cross(vector1, vector2);

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(result).arg(expectedResult).arg((result - expectedResult).toString())));
}

void test_Vector3Float::length_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<Vector3Float>("vector");
    QTest::addColumn<float>("expectedLength");

    QTest::newRow("zero") << Vector3Float() << 0.0f;
    QTest::newRow("i") << Vector3Float(1, 0, 0) << 1.0f;
    QTest::newRow("-i") << Vector3Float(-1, 0, 0) << 1.0f;
    QTest::newRow("j") << Vector3Float(0, 1, 0) << 1.0f;
    QTest::newRow("-j") << Vector3Float(0, -1, 0) << 1.0f;
    QTest::newRow("k") << Vector3Float(0, 0, 1) << 1.0f;
    QTest::newRow("-k") << Vector3Float(0, 0, -1) << 1.0f;
    QTest::newRow("i+j") << Vector3Float(1, 1, 0) << 1.4142135623730951f;
    QTest::newRow("i+k") << Vector3Float(1, 0, 1) << 1.4142135623730951f;
    QTest::newRow("j+k") << Vector3Float(0, 1, 1) << 1.4142135623730951f;
    QTest::newRow("i+j+k") << Vector3Float(1, 1, 1) << 1.7320508075688772f;
    QTest::newRow("random #1") << Vector3Float(-1, -4, -5) << 6.48074069840786f;
    QTest::newRow("random #2") << Vector3Float(1, 3, -5) << 5.916079783099616f;
    QTest::newRow("random #3") << Vector3Float(-3, 0, 4) << 5.0f;
}

void test_Vector3Float::length_ShouldReturnExpectedValue()
{
    QFETCH(Vector3Float, vector);
    QFETCH(float, expectedLength);

    float length = vector.length();

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(length, expectedLength),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(length).arg(expectedLength).arg(length - expectedLength)));
}

void test_Vector3Float::lengthSquared_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<Vector3Float>("vector");
    QTest::addColumn<float>("expectedLengthSquared");

    QTest::newRow("zero") << Vector3Float() << 0.0f;
    QTest::newRow("i") << Vector3Float(1, 0, 0) << 1.0f;
    QTest::newRow("-i") << Vector3Float(-1, 0, 0) << 1.0f;
    QTest::newRow("j") << Vector3Float(0, 1, 0) << 1.0f;
    QTest::newRow("-j") << Vector3Float(0, -1, 0) << 1.0f;
    QTest::newRow("k") << Vector3Float(0, 0, 1) << 1.0f;
    QTest::newRow("-k") << Vector3Float(0, 0, -1) << 1.0f;
    QTest::newRow("i+j") << Vector3Float(1, 1, 0) << 2.0f;
    QTest::newRow("i+k") << Vector3Float(1, 0, 1) << 2.0f;
    QTest::newRow("j+k") << Vector3Float(0, 1, 1) << 2.0f;
    QTest::newRow("i+j+k") << Vector3Float(1, 1, 1) << 3.0f;
    QTest::newRow("random #1") << Vector3Float(-1, -4, -5) << 42.0f;
    QTest::newRow("random #2") << Vector3Float(1, 3, -5) << 35.0f;
    QTest::newRow("random #3") << Vector3Float(-3, 0, 4) << 25.0f;
}

void test_Vector3Float::lengthSquared_ShouldReturnExpectedValue()
{
    QFETCH(Vector3Float, vector);
    QFETCH(float, expectedLengthSquared);

    float lengthSquared = vector.lengthSquared();

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(lengthSquared, expectedLengthSquared),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(lengthSquared).arg(expectedLengthSquared)
                                                                          .arg(lengthSquared - expectedLengthSquared)));
}

void test_Vector3Float::normalize_ShouldNormalizeAndReturnVector_data()
{
    QTest::addColumn<Vector3Float>("vector");
    QTest::addColumn<Vector3Float>("expectedResult");

    QTest::newRow("i") << Vector3Float(1, 0, 0) << Vector3Float(1, 0, 0);
    QTest::newRow("-i") << Vector3Float(-1, 0, 0) << Vector3Float(-1, 0, 0);
    QTest::newRow("j") << Vector3Float(0, 1, 0) << Vector3Float(0, 1, 0);
    QTest::newRow("-j") << Vector3Float(0, -1, 0) << Vector3Float(0, -1, 0);
    QTest::newRow("k") << Vector3Float(0, 0, 1) << Vector3Float(0, 0, 1);
    QTest::newRow("-k") << Vector3Float(0, 0, -1) << Vector3Float(0, 0, -1);
    QTest::newRow("i+j") << Vector3Float(1, 1, 0) << Vector3Float(0.7071067811865475f, 0.7071067811865475f, 0);
    QTest::newRow("i+k") << Vector3Float(1, 0, 1) << Vector3Float(0.7071067811865475f, 0, 0.7071067811865475f);
    QTest::newRow("j+k") << Vector3Float(0, 1, 1) << Vector3Float(0, 0.7071067811865475f, 0.7071067811865475f);
    QTest::newRow("i+j+k") << Vector3Float(1, 1, 1) << Vector3Float(0.5773502691896258f, 0.5773502691896258f, 0.5773502691896258f);
    QTest::newRow("random #1") << Vector3Float(-1, -4, -5) << Vector3Float(-0.1543033499620919f, -0.6172133998483676f, -0.7715167498104595f);
    QTest::newRow("random #2") << Vector3Float(1, 3, -5) << Vector3Float(0.1690308509457033f, 0.50709255283711f, -0.8451542547285166f);
    QTest::newRow("random #3") << Vector3Float(-3, 0, 4) << Vector3Float(-0.6f, 0, 0.8f);
}

void test_Vector3Float::normalize_ShouldNormalizeAndReturnVector()
{
    QFETCH(Vector3Float, vector);
    QFETCH(Vector3Float, expectedResult);

    Vector3Float result = vector.normalize();

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(vector, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(vector).arg(expectedResult).arg((vector - expectedResult).toString())));
    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(result).arg(expectedResult).arg((result - expectedResult).toString())));
}

void test_Vector3Float::operatorEquals_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<Vector3Float>("vector1");
    QTest::addColumn<Vector3Float>("vector2");
    QTest::addColumn<bool>("expectedResult");

    QTest::newRow("equal #1") << Vector3Float(-1, -5, -3) << Vector3Float(-1, -5, -3) << true;
    QTest::newRow("equal #2") << Vector3Float(3, 1, 5) << Vector3Float(3, 1, 5) << true;
    QTest::newRow("equal #3") << Vector3Float(4, -5, -3) << Vector3Float(4, -5, -3) << true;
    QTest::newRow("different #1") << Vector3Float(3, -1, 4) << Vector3Float(3, -3, 2) << false;
    QTest::newRow("different #2") << Vector3Float(0, 5, -1) << Vector3Float(-4, 5, 0) << false;
    QTest::newRow("different #3") << Vector3Float(2, -1, -1) << Vector3Float(0, -1, -3) << false;
}

void test_Vector3Float::operatorEquals_ShouldReturnExpectedValue()
{
    QFETCH(Vector3Float, vector1);
    QFETCH(Vector3Float, vector2);
    QFETCH(bool, expectedResult);

    QCOMPARE(vector1 == vector2, expectedResult);
}

void test_Vector3Float::operatorNotEquals_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<Vector3Float>("vector1");
    QTest::addColumn<Vector3Float>("vector2");
    QTest::addColumn<bool>("expectedResult");

    QTest::newRow("equal #1") << Vector3Float(-1, -5, -3) << Vector3Float(-1, -5, -3) << false;
    QTest::newRow("equal #2") << Vector3Float(3, 1, 5) << Vector3Float(3, 1, 5) << false;
    QTest::newRow("equal #3") << Vector3Float(4, -5, -3) << Vector3Float(4, -5, -3) << false;
    QTest::newRow("different #1") << Vector3Float(3, -1, 4) << Vector3Float(3, -3, 2) << true;
    QTest::newRow("different #2") << Vector3Float(0, 5, -1) << Vector3Float(-4, 5, 0) << true;
    QTest::newRow("different #3") << Vector3Float(2, -1, -1) << Vector3Float(0, -1, -3) << true;
}

void test_Vector3Float::operatorNotEquals_ShouldReturnExpectedValue()
{
    QFETCH(Vector3Float, vector1);
    QFETCH(Vector3Float, vector2);
    QFETCH(bool, expectedResult);

    QCOMPARE(vector1 != vector2, expectedResult);
}

void test_Vector3Float::operatorPositive_ShouldReturnSameVector_data()
{
    QTest::addColumn<Vector3Float>("vector");

    QTest::newRow("zero") << Vector3Float();
    QTest::newRow("-zero") << Vector3Float(-0.0, -0.0, -0.0);
    QTest::newRow("i") << Vector3Float(1, 0, 0);
    QTest::newRow("-i") << Vector3Float(-1, 0, 0);
    QTest::newRow("j") << Vector3Float(0, 1, 0);
    QTest::newRow("-j") << Vector3Float(0, -1, 0);
    QTest::newRow("k") << Vector3Float(0, 0, 1);
    QTest::newRow("-k") << Vector3Float(0, 0, -1);
    QTest::newRow("random #1") << Vector3Float(5, -5, 4);
    QTest::newRow("random #2") << Vector3Float(-4, 2, -3);
    QTest::newRow("random #3") << Vector3Float(0, 4, 0);
}

void test_Vector3Float::operatorPositive_ShouldReturnSameVector()
{
    QFETCH(Vector3Float, vector);

    Vector3Float positive = +vector;

    QVERIFY2(positive == vector, qPrintable(QString("actual: %1, expected %2, difference: %3").arg(positive).arg(vector).arg((positive - vector).toString())));
}

void test_Vector3Float::operatorNegative_ShouldReturnOpposite_data()
{
    QTest::addColumn<Vector3Float>("vector");
    QTest::addColumn<Vector3Float>("opposite");

    QTest::newRow("zero") << Vector3Float() << Vector3Float(-0.0, -0.0, -0.0);
    QTest::newRow("-zero") << Vector3Float(-0.0, -0.0, -0.0) << Vector3Float();
    QTest::newRow("i") << Vector3Float(1, 0, 0) << Vector3Float(-1, 0, 0);
    QTest::newRow("-i") << Vector3Float(-1, 0, 0) << Vector3Float(1, 0, 0);
    QTest::newRow("j") << Vector3Float(0, 1, 0) << Vector3Float(0, -1, 0);
    QTest::newRow("-j") << Vector3Float(0, -1, 0) << Vector3Float(0, 1, 0);
    QTest::newRow("k") << Vector3Float(0, 0, 1) << Vector3Float(0, 0, -1);
    QTest::newRow("-k") << Vector3Float(0, 0, -1) << Vector3Float(0, 0, 1);
    QTest::newRow("random #1") << Vector3Float(5, -5, 4) << Vector3Float(-5, 5, -4);
    QTest::newRow("random #2") << Vector3Float(-4, 2, -3) << Vector3Float(4, -2, 3);
    QTest::newRow("random #3") << Vector3Float(0, 4, 0) << Vector3Float(0, -4, 0);
}

void test_Vector3Float::operatorNegative_ShouldReturnOpposite()
{
    QFETCH(Vector3Float, vector);
    QFETCH(Vector3Float, opposite);

    Vector3Float negative = -vector;

    QVERIFY2(negative == opposite,
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(negative).arg(opposite).arg((negative - opposite).toString())));
}

void test_Vector3Float::operatorPlus_ShouldReturnExpectedValue_data()
{
    setupAdditionData();
}

void test_Vector3Float::operatorPlus_ShouldReturnExpectedValue()
{
    QFETCH(Vector3Float, vector1);
    QFETCH(Vector3Float, vector2);
    QFETCH(Vector3Float, expectedResult);

    Vector3Float result = vector1 + vector2;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(result).arg(expectedResult).arg((result - expectedResult).toString())));
}

void test_Vector3Float::operatorPlusEqual_ShouldModifyVectorAndReturnIt_data()
{
    setupAdditionData();
}

void test_Vector3Float::operatorPlusEqual_ShouldModifyVectorAndReturnIt()
{
    QFETCH(Vector3Float, vector1);
    QFETCH(Vector3Float, vector2);
    QFETCH(Vector3Float, expectedResult);

    Vector3Float &result = vector1 += vector2;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(vector1, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(vector1).arg(expectedResult).arg((vector1 - expectedResult).toString())));
    QCOMPARE(&result, &vector1);
}

void test_Vector3Float::operatorMinus_ShouldReturnExpectedValue_data()
{
    setupSubtractionData();
}

void test_Vector3Float::operatorMinus_ShouldReturnExpectedValue()
{
    QFETCH(Vector3Float, vector1);
    QFETCH(Vector3Float, vector2);
    QFETCH(Vector3Float, expectedResult);

    Vector3Float result = vector1 - vector2;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(result).arg(expectedResult).arg((result - expectedResult).toString())));
}

void test_Vector3Float::operatorMinusEqual_ShouldModifyVectorAndReturnIt_data()
{
    setupSubtractionData();
}

void test_Vector3Float::operatorMinusEqual_ShouldModifyVectorAndReturnIt()
{
    QFETCH(Vector3Float, vector1);
    QFETCH(Vector3Float, vector2);
    QFETCH(Vector3Float, expectedResult);

    Vector3Float &result = vector1 -= vector2;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(vector1, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(vector1).arg(expectedResult).arg((vector1 - expectedResult).toString())));
    QCOMPARE(&result, &vector1);
}

void test_Vector3Float::operatorDotProduct_ShouldReturnExpectedValue_data()
{
    setupDotProductData();
}

void test_Vector3Float::operatorDotProduct_ShouldReturnExpectedValue()
{
    QFETCH(Vector3Float, vector1);
    QFETCH(Vector3Float, vector2);
    QFETCH(float, expectedResult);

    float result = vector1 * vector2;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(result).arg(expectedResult).arg(result - expectedResult)));
}

void test_Vector3Float::operatorCrossProduct_ShouldReturnExpectedValue_data()
{
    setupCrossProductData();
}

void test_Vector3Float::operatorCrossProduct_ShouldReturnExpectedValue()
{
    QFETCH(Vector3Float, vector1);
    QFETCH(Vector3Float, vector2);
    QFETCH(Vector3Float, expectedResult);

    Vector3Float result = vector1 ^ vector2;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(result).arg(expectedResult).arg((result - expectedResult).toString())));
}

void test_Vector3Float::toString_ShoudReturnExpectedValue_data()
{
    setupToStringData();
}

void test_Vector3Float::toString_ShoudReturnExpectedValue()
{
    QFETCH(Vector3Float, vector);
    QFETCH(QString, expectedString);

    QCOMPARE(vector.toString(), expectedString);
}

void test_Vector3Float::operatorQString_ShouldReturnExpectedValue_data()
{
    setupToStringData();
}

void test_Vector3Float::operatorQString_ShouldReturnExpectedValue()
{
    QFETCH(Vector3Float, vector);
    QFETCH(QString, expectedString);

    QCOMPARE(QString(vector), expectedString);
}

void test_Vector3Float::operatorProductByScalarLeft_ShouldReturnExpectedValue_data()
{
    setupProductByScalarData();
}

void test_Vector3Float::operatorProductByScalarLeft_ShouldReturnExpectedValue()
{
    QFETCH(float, scalar);
    QFETCH(Vector3Float, vector);
    QFETCH(Vector3Float, expectedResult);

    Vector3Float result = scalar * vector;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(result).arg(expectedResult).arg((result - expectedResult).toString())));
}

void test_Vector3Float::operatorProductByScalarRight_ShouldReturnExpectedValue_data()
{
    setupProductByScalarData();
}

void test_Vector3Float::operatorProductByScalarRight_ShouldReturnExpectedValue()
{
    QFETCH(Vector3Float, vector);
    QFETCH(float, scalar);
    QFETCH(Vector3Float, expectedResult);

    Vector3Float result = vector * scalar;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(result).arg(expectedResult).arg((result - expectedResult).toString())));
}

void test_Vector3Float::operatorProductByScalarEqual_ShouldModifyVectorAndReturnIt_data()
{
    setupProductByScalarData();
}

void test_Vector3Float::operatorProductByScalarEqual_ShouldModifyVectorAndReturnIt()
{
    QFETCH(Vector3Float, vector);
    QFETCH(float, scalar);
    QFETCH(Vector3Float, expectedResult);

    Vector3Float &result = vector *= scalar;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(vector, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(vector).arg(expectedResult).arg((vector - expectedResult).toString())));
    QCOMPARE(&result, &vector);
}

void test_Vector3Float::operatorDivisionByScalar_ShouldReturnExpectedValue_data()
{
    setupDivisionByScalarData();
}

void test_Vector3Float::operatorDivisionByScalar_ShouldReturnExpectedValue()
{
    QFETCH(Vector3Float, vector);
    QFETCH(float, scalar);
    QFETCH(Vector3Float, expectedResult);

    Vector3Float result = vector / scalar;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(result, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(result).arg(expectedResult).arg((result - expectedResult).toString())));
}

void test_Vector3Float::operatorDivisionByScalarEqual_ShouldModifyVectorAndReturnIt_data()
{
    setupDivisionByScalarData();
}

void test_Vector3Float::operatorDivisionByScalarEqual_ShouldModifyVectorAndReturnIt()
{
    QFETCH(Vector3Float, vector);
    QFETCH(float, scalar);
    QFETCH(Vector3Float, expectedResult);

    Vector3Float &result = vector /= scalar;

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(vector, expectedResult),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(vector).arg(expectedResult).arg((vector - expectedResult).toString())));
    QCOMPARE(&result, &vector);
}

void test_Vector3Float::operatorQDataStreamIn_ShouldFillVectorAndReturnStream_data()
{
    // La primera serveix perquè no ens esborri el byte array
    QTest::addColumn<QSharedPointer<QByteArray>>("byteArray");
    QTest::addColumn<QSharedPointer<QDataStream>>("stream");
    QTest::addColumn<Vector3Float>("filledVector");

    {
        QSharedPointer<QByteArray> byteArray(new QByteArray());
        QSharedPointer<QDataStream> stream(new QDataStream(byteArray.data(), QIODevice::ReadWrite));
        Vector3Float vector;
        (*stream) << vector.x << vector.y << vector.z;
        stream->device()->reset();
        QTest::newRow("zero") << byteArray << stream << vector;
    }

    {
        QSharedPointer<QByteArray> byteArray(new QByteArray());
        QSharedPointer<QDataStream> stream(new QDataStream(byteArray.data(), QIODevice::ReadWrite));
        Vector3Float vector(-0, -0, -0);
        (*stream) << vector.x << vector.y << vector.z;
        stream->device()->reset();
        QTest::newRow("-zero") << byteArray << stream << vector;
    }

    {
        QSharedPointer<QByteArray> byteArray(new QByteArray());
        QSharedPointer<QDataStream> stream(new QDataStream(byteArray.data(), QIODevice::ReadWrite));
        Vector3Float vector(4, 5, -4);
        (*stream) << vector.x << vector.y << vector.z;
        stream->device()->reset();
        QTest::newRow("random #1") << byteArray << stream << vector;
    }

    {
        QSharedPointer<QByteArray> byteArray(new QByteArray());
        QSharedPointer<QDataStream> stream(new QDataStream(byteArray.data(), QIODevice::ReadWrite));
        Vector3Float vector(-4, -1, -3);
        (*stream) << vector.x << vector.y << vector.z;
        stream->device()->reset();
        QTest::newRow("random #2") << byteArray << stream << vector;
    }

    {
        QSharedPointer<QByteArray> byteArray(new QByteArray());
        QSharedPointer<QDataStream> stream(new QDataStream(byteArray.data(), QIODevice::ReadWrite));
        Vector3Float vector(2, -1, -5);
        (*stream) << vector.x << vector.y << vector.z;
        stream->device()->reset();
        QTest::newRow("random #3") << byteArray << stream << vector;
    }
}

void test_Vector3Float::operatorQDataStreamIn_ShouldFillVectorAndReturnStream()
{
    QFETCH(QSharedPointer<QDataStream>, stream);
    QFETCH(Vector3Float, filledVector);

    Vector3Float vector;
    QDataStream &returnedStream = (*stream) >> vector;
    
    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(vector, filledVector),
             qPrintable(QString("actual: %1, expected %2, difference: %3").arg(vector).arg(filledVector).arg((vector - filledVector).toString())));
    QCOMPARE(&returnedStream, stream.data());
}

void test_Vector3Float::operatorQDataStreamOut_ShouldWriteToStreamAndReturnIt_data()
{
    QTest::addColumn<Vector3Float>("vector");
    // Aquesta serveix perquè no ens esborri el byte array
    QTest::addColumn<QSharedPointer<QByteArray>>("byteArray");
    QTest::addColumn<QSharedPointer<QDataStream>>("filledStream");

    {
        Vector3Float vector;
        QSharedPointer<QByteArray> byteArray(new QByteArray());
        QSharedPointer<QDataStream> stream(new QDataStream(byteArray.data(), QIODevice::ReadWrite));
        (*stream) << vector.x << vector.y << vector.z;
        QTest::newRow("zero") << vector << byteArray << stream;
    }

    {
        Vector3Float vector(-0, -0, -0);
        QSharedPointer<QByteArray> byteArray(new QByteArray());
        QSharedPointer<QDataStream> stream(new QDataStream(byteArray.data(), QIODevice::ReadWrite));
        (*stream) << vector.x << vector.y << vector.z;
        QTest::newRow("-zero") << vector << byteArray << stream;
    }

    {
        Vector3Float vector(4, 5, -4);
        QSharedPointer<QByteArray> byteArray(new QByteArray());
        QSharedPointer<QDataStream> stream(new QDataStream(byteArray.data(), QIODevice::ReadWrite));
        (*stream) << vector.x << vector.y << vector.z;
        QTest::newRow("random #1") << vector << byteArray << stream;
    }

    {
        Vector3Float vector(-4, -1, -3);
        QSharedPointer<QByteArray> byteArray(new QByteArray());
        QSharedPointer<QDataStream> stream(new QDataStream(byteArray.data(), QIODevice::ReadWrite));
        (*stream) << vector.x << vector.y << vector.z;
        QTest::newRow("random #2") << vector << byteArray << stream;
    }

    {
        Vector3Float vector(2, -1, -5);
        QSharedPointer<QByteArray> byteArray(new QByteArray());
        QSharedPointer<QDataStream> stream(new QDataStream(byteArray.data(), QIODevice::ReadWrite));
        (*stream) << vector.x << vector.y << vector.z;
        QTest::newRow("random #3") << vector << byteArray << stream;
    }
}

void test_Vector3Float::operatorQDataStreamOut_ShouldWriteToStreamAndReturnIt()
{
    QFETCH(Vector3Float, vector);
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

void test_Vector3Float::setupDotProductData()
{
    QTest::addColumn<Vector3Float>("vector1");
    QTest::addColumn<Vector3Float>("vector2");
    QTest::addColumn<float>("expectedResult");

    QTest::newRow("zero · zero") << Vector3Float() << Vector3Float() << 0.0f;
    QTest::newRow("zero · non-zero") << Vector3Float() << Vector3Float(1, 1, 1) << 0.0f;
    QTest::newRow("non-zero · zero") << Vector3Float(1, 1, 1) << Vector3Float() << 0.0f;
    QTest::newRow("same vectors") << Vector3Float(-2, -3, -3) << Vector3Float(-2, -3, -3) << 22.0f;
    QTest::newRow("parallel") << Vector3Float(-3, -5, -2) << Vector3Float(-1.5, -2.5, -1) << 19.0f;
    QTest::newRow("perpendicular") << Vector3Float(1, 0, 0) << Vector3Float(0, 1, 0) << 0.0f;
    QTest::newRow("opposite") << Vector3Float(-3, 4, 5) << Vector3Float(1.5, -2, -2.5) << -25.0f;
    QTest::newRow("random #1") << Vector3Float(1, 3, -5) << Vector3Float(4, -2, -1) << 3.0f;
    QTest::newRow("random #2") << Vector3Float(3, -4, -2) << Vector3Float(0, 1, -5) << 6.0f;
    QTest::newRow("random #3") << Vector3Float(-2, -5, -3) << Vector3Float(-3, 0, 1) << 3.0f;
}

void test_Vector3Float::setupCrossProductData()
{
    QTest::addColumn<Vector3Float>("vector1");
    QTest::addColumn<Vector3Float>("vector2");
    QTest::addColumn<Vector3Float>("expectedResult");

    QTest::newRow("zero × zero") << Vector3Float() << Vector3Float() << Vector3Float();
    QTest::newRow("zero × non-zero") << Vector3Float() << Vector3Float(1, 1, 1) << Vector3Float();
    QTest::newRow("non-zero × zero") << Vector3Float(1, 1, 1) << Vector3Float() << Vector3Float();
    QTest::newRow("same vectors") << Vector3Float(-2, -3, -3) << Vector3Float(-2, -3, -3) << Vector3Float();
    QTest::newRow("parallel") << Vector3Float(-3, -5, -2) << Vector3Float(-1.5, -2.5, -1) << Vector3Float();
    QTest::newRow("perpendicular") << Vector3Float(1, 0, 0) << Vector3Float(0, 1, 0) << Vector3Float(0, 0, 1);
    QTest::newRow("opposite") << Vector3Float(-3, 4, 5) << Vector3Float(1.5, -2, -2.5) << Vector3Float();
    QTest::newRow("random #1") << Vector3Float(1, 3, -5) << Vector3Float(4, -2, -1) << Vector3Float(-13, -19, -14);
    QTest::newRow("random #2") << Vector3Float(3, -4, -2) << Vector3Float(0, 1, -5) << Vector3Float(22, 15, 3);
    QTest::newRow("random #3") << Vector3Float(-2, -5, -3) << Vector3Float(-3, 0, 1) << Vector3Float(-5, 11, -15);
}

void test_Vector3Float::setupAdditionData()
{
    QTest::addColumn<Vector3Float>("vector1");
    QTest::addColumn<Vector3Float>("vector2");
    QTest::addColumn<Vector3Float>("expectedResult");

    QTest::newRow("zero + zero") << Vector3Float() << Vector3Float() << Vector3Float();
    QTest::newRow("zero + non-zero") << Vector3Float() << Vector3Float(1, 1, 1) << Vector3Float(1, 1, 1);
    QTest::newRow("non-zero + zero") << Vector3Float(1, 1, 1) << Vector3Float() << Vector3Float(1, 1, 1);
    QTest::newRow("+ i") << Vector3Float(-5, -4, 0) << Vector3Float(1, 0, 0) << Vector3Float(-4, -4, 0);
    QTest::newRow("+ (-i)") << Vector3Float(-5, -4, 0) << Vector3Float(-1, 0, 0) << Vector3Float(-6, -4, 0);
    QTest::newRow("+ j") << Vector3Float(-5, -4, 0) << Vector3Float(0, 1, 0) << Vector3Float(-5, -3, 0);
    QTest::newRow("+ (-j)") << Vector3Float(-5, -4, 0) << Vector3Float(0, -1, 0) << Vector3Float(-5, -5, 0);
    QTest::newRow("+ k") << Vector3Float(-5, -4, 0) << Vector3Float(0, 0, 1) << Vector3Float(-5, -4, 1);
    QTest::newRow("+ (-k)") << Vector3Float(-5, -4, 0) << Vector3Float(0, 0, -1) << Vector3Float(-5, -4, -1);
    QTest::newRow("random #1") << Vector3Float(1, 3, -5) << Vector3Float(4, -2, -1) << Vector3Float(5, 1, -6);
    QTest::newRow("random #2") << Vector3Float(3, -4, -2) << Vector3Float(0, 1, -5) << Vector3Float(3, -3, -7);
    QTest::newRow("random #3") << Vector3Float(-2, -5, -3) << Vector3Float(-3, 0, 1) << Vector3Float(-5, -5, -2);
}

void test_Vector3Float::setupSubtractionData()
{
    QTest::addColumn<Vector3Float>("vector1");
    QTest::addColumn<Vector3Float>("vector2");
    QTest::addColumn<Vector3Float>("expectedResult");

    QTest::newRow("zero - zero") << Vector3Float() << Vector3Float() << Vector3Float();
    QTest::newRow("zero - non-zero") << Vector3Float() << Vector3Float(1, 1, 1) << Vector3Float(-1, -1, -1);
    QTest::newRow("non-zero - zero") << Vector3Float(1, 1, 1) << Vector3Float() << Vector3Float(1, 1, 1);
    QTest::newRow("- i") << Vector3Float(-5, -4, 0) << Vector3Float(1, 0, 0) << Vector3Float(-6, -4, 0);
    QTest::newRow("- (-i)") << Vector3Float(-5, -4, 0) << Vector3Float(-1, 0, 0) << Vector3Float(-4, -4, 0);
    QTest::newRow("- j") << Vector3Float(-5, -4, 0) << Vector3Float(0, 1, 0) << Vector3Float(-5, -5, 0);
    QTest::newRow("- (-j)") << Vector3Float(-5, -4, 0) << Vector3Float(0, -1, 0) << Vector3Float(-5, -3, 0);
    QTest::newRow("- k") << Vector3Float(-5, -4, 0) << Vector3Float(0, 0, 1) << Vector3Float(-5, -4, -1);
    QTest::newRow("- (-k)") << Vector3Float(-5, -4, 0) << Vector3Float(0, 0, -1) << Vector3Float(-5, -4, 1);
    QTest::newRow("random #1") << Vector3Float(1, 3, -5) << Vector3Float(4, -2, -1) << Vector3Float(-3, 5, -4);
    QTest::newRow("random #2") << Vector3Float(3, -4, -2) << Vector3Float(0, 1, -5) << Vector3Float(3, -5, 3);
    QTest::newRow("random #3") << Vector3Float(-2, -5, -3) << Vector3Float(-3, 0, 1) << Vector3Float(1, -5, -4);
}

void test_Vector3Float::setupToStringData()
{
    QTest::addColumn<Vector3Float>("vector");
    QTest::addColumn<QString>("expectedString");

    QTest::newRow("zero") << Vector3Float() << "(0, 0, 0)";
    QTest::newRow("-zero") << Vector3Float(-0.0, -0.0, -0.0) << "(0, 0, 0)";
    QTest::newRow("random #1") << Vector3Float(1, -2, 0) << "(1, -2, 0)";
    QTest::newRow("random #2") << Vector3Float(2.5, -3, -3.4f) << "(2.5, -3, -3.4)";
    QTest::newRow("random #3") << Vector3Float(3.9875905695f, 0.4293503326f, 5.5895753834f) << "(3.98759, 0.42935, 5.58958)";
}

void test_Vector3Float::setupProductByScalarData()
{
    QTest::addColumn<float>("scalar");
    QTest::addColumn<Vector3Float>("vector");
    QTest::addColumn<Vector3Float>("expectedResult");

    QTest::newRow("0 * (0,0,0)") << 0.0f << Vector3Float() << Vector3Float();
    QTest::newRow("0 * (x,y,z)") << 0.0f << Vector3Float(1, 1, 1) << Vector3Float();
    QTest::newRow("a * (0,0,0)") << 1.0f << Vector3Float() << Vector3Float();
    QTest::newRow("random #1") << 2.0f << Vector3Float(-1, 1, -3) << Vector3Float(-2, 2, -6);
    QTest::newRow("random #2") << 4.0f << Vector3Float(1, -5, 3) << Vector3Float(4, -20, 12);
    QTest::newRow("random #3") << -5.0f << Vector3Float(1, -2, 0) << Vector3Float(-5, 10, 0);
}

void test_Vector3Float::setupDivisionByScalarData()
{
    QTest::addColumn<Vector3Float>("vector");
    QTest::addColumn<float>("scalar");
    QTest::addColumn<Vector3Float>("expectedResult");

    QTest::newRow("(0,0,0) / a") << Vector3Float() << 1.0f << Vector3Float();
    QTest::newRow("(x,y,z) / 1") << Vector3Float(0, -3, -5) << 1.0f << Vector3Float(0, -3, -5);
    QTest::newRow("random #1") << Vector3Float(4, 3, 5) << 3.0f << Vector3Float(1.3333333333333333f, 1, 1.6666666666666667f);
    QTest::newRow("random #2") << Vector3Float(2, 5, 1) << 3.0f << Vector3Float(0.6666666666666666f, 1.6666666666666667f, 0.3333333333333333f);
    QTest::newRow("random #3") << Vector3Float(4, -3, -5) << 2.0f << Vector3Float(2, -1.5, -2.5);
}

DECLARE_TEST(test_Vector3Float)

#include "test_vector3float.moc"
