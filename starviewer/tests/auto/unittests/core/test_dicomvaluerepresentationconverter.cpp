#include "autotest.h"

#include <QVector2D>

#include "dicomvaluerepresentationconverter.h"
#include "fuzzycomparetesthelper.h"
#include "mathtools.h"

using namespace udg;
using namespace testing;

class test_DICOMValueRepresentationConverter : public QObject {
Q_OBJECT

private slots:
    // Cas en que els valors i/o el format no siguin vàlids
    void decimalStringToDoubleVector_ShouldReturnEmptyQVectorAndOkIsFalse_data();
    void decimalStringToDoubleVector_ShouldReturnEmptyQVectorAndOkIsFalse();

    // Cas en que l'string que passem sigui buit
    void decimalStringToDoubleVector_ShouldReturnEmptyQVectorAndOkIsTrue_data();
    void decimalStringToDoubleVector_ShouldReturnEmptyQVectorAndOkIsTrue();

    // Cas en que l'string és correcte
    void decimalStringToDoubleVector_ShouldReturnNonEmptyQVectorAndOkIsTrue_data();
    void decimalStringToDoubleVector_ShouldReturnNonEmptyQVectorAndOkIsTrue();

    void decimalStringTo2DDoubleVector_ShouldReturnExpectedValues_data();
    void decimalStringTo2DDoubleVector_ShouldReturnExpectedValues();

    void timeToQTime_ShouldReturnExpectedValues_data();
    void timeToQTime_ShouldReturnExpectedValues();
};

void test_DICOMValueRepresentationConverter::decimalStringToDoubleVector_ShouldReturnEmptyQVectorAndOkIsFalse_data()
{
    QTest::addColumn<QString>("string");

    QString Separator = "\\";
    QString integerString = "123";
    QString doubleString = "1.3";
    QString wrongDoubleString = ".3e 10";
    QString alphanumericString = "Hello 33!";

    QTest::newRow("Alphanumeric String") << alphanumericString;
    QTest::newRow("Single number bad formatted") << wrongDoubleString;
    QTest::newRow("Multiple numbers bad formatted") << wrongDoubleString + Separator + "3a.3";
    QTest::newRow("Mixed values (numbers,strings,etc) bad & well formatted") << doubleString + Separator + wrongDoubleString + Separator + alphanumericString;
    QTest::newRow("Extra separator at the end") << "1.0" + Separator;
    QTest::newRow("Extra separator at the beggining") << Separator + "1.0";
    QTest::newRow("Extra separators at the end and the beggining") << Separator + "1.0" + Separator;
    QTest::newRow("More than one contiguous separator") << "1.0" + Separator + Separator + "2.0";
    QTest::newRow("Separator with no values") << Separator;
}

void test_DICOMValueRepresentationConverter::decimalStringToDoubleVector_ShouldReturnEmptyQVectorAndOkIsFalse()
{
    QFETCH(QString, string);

    bool ok;
    QVector<double> convertedString = DICOMValueRepresentationConverter::decimalStringToDoubleVector(string, &ok);
    QCOMPARE(convertedString.isEmpty(), QVector<double>().isEmpty());
    QCOMPARE(ok, false);
}

void test_DICOMValueRepresentationConverter::decimalStringToDoubleVector_ShouldReturnEmptyQVectorAndOkIsTrue_data()
{
    QTest::addColumn<QString>("string");

    QTest::newRow("Empty String") << "";
}

void test_DICOMValueRepresentationConverter::decimalStringToDoubleVector_ShouldReturnEmptyQVectorAndOkIsTrue()
{
    QFETCH(QString, string);

    bool ok;
    QVector<double> convertedString = DICOMValueRepresentationConverter::decimalStringToDoubleVector(string, &ok);
    QCOMPARE(convertedString.isEmpty(), QVector<double>().isEmpty());
    QCOMPARE(ok, true);
}

void test_DICOMValueRepresentationConverter::decimalStringToDoubleVector_ShouldReturnNonEmptyQVectorAndOkIsTrue_data()
{
    QTest::addColumn<QString>("string");

    QString Separator = "\\";
    
    QTest::newRow("Single-valued decimal string") << "1.3";
    QTest::newRow("Multivalued decimal string (2)") << "1" + Separator + "0.3e+15";
    QTest::newRow("Multivalued decimal string (3)") << "0.0" + Separator + "-0.3e-15" + Separator + "-3";
    QTest::newRow("Multivalued decimal string (4)") << "-0.3e15" + Separator + "-0.3e-15" + Separator + "-.3e+15" + Separator + ".3e15";
}

void test_DICOMValueRepresentationConverter::decimalStringToDoubleVector_ShouldReturnNonEmptyQVectorAndOkIsTrue()
{
    QFETCH(QString, string);

    bool ok;
    QVector<double> convertedString = DICOMValueRepresentationConverter::decimalStringToDoubleVector(string, &ok);
    QCOMPARE(convertedString.isEmpty(), false);
    QCOMPARE(ok, true);
}

void test_DICOMValueRepresentationConverter::decimalStringTo2DDoubleVector_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QVector2D>("expectedVector2D");
    QTest::addColumn<bool>("expectedOk");

    QString separator = "\\";
    QString integerString = "123";
    QString doubleString = "1.3";
    QString wrongDoubleString = ".3e 10";
    QString alphanumericString = "Hello 33!";
    
    QTest::newRow("Empty string") << QString() << QVector2D(0, 0) << false;
    QTest::newRow("Single-valued decimal string") << doubleString << QVector2D(0, 0) << false;
    QTest::newRow("3-valued decimal string") << doubleString + separator +  doubleString + separator + doubleString << QVector2D(0, 0) << false;
    QTest::newRow("2-valued decimal string") << doubleString + separator + integerString << QVector2D(1.3, 123) << true;
    QTest::newRow("2-valued decimal string - with invalid values") << doubleString + separator + alphanumericString << QVector2D(0, 0) << false;
}

void test_DICOMValueRepresentationConverter::decimalStringTo2DDoubleVector_ShouldReturnExpectedValues()
{
    QFETCH(QString, string);
    QFETCH(QVector2D, expectedVector2D);
    QFETCH(bool, expectedOk);

    bool ok;
    QVector2D resultVector = DICOMValueRepresentationConverter::decimalStringTo2DDoubleVector(string, &ok);
    
    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(resultVector.x(), expectedVector2D.x(), 0.001));
    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(resultVector.y(), expectedVector2D.y(), 0.001));
    QCOMPARE(ok, expectedOk);
}

void test_DICOMValueRepresentationConverter::timeToQTime_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<QString>("timeString");
    QTest::addColumn<QTime>("expectedTime");

    QTest::newRow("Empty string") << QString() << QTime() ;

    int randomHour = MathTools::randomInt(0, 23);
    int randomMinute = MathTools::randomInt(0, 59);
    int randomSecond = MathTools::randomInt(0, 60);
    int randomMicroSecond = MathTools::randomInt(0, 999999);

    QString randomHourString = QString("%1").arg(randomHour, 2, 10, QChar('0'));
    QString randomMinuteString = QString("%1").arg(randomMinute, 2, 10, QChar('0'));
    QString randomSecondString = QString("%1").arg(randomSecond, 2, 10, QChar('0'));
    QString randomMicroSecondString = QString("%1").arg(randomMicroSecond, 6, 10, QChar('0'));
    
    QTest::newRow("2 digits (HH)") << randomHourString << QTime(randomHour, 0);
    QTest::newRow("4 digits (HHMM") << randomHourString + randomMinuteString << QTime(randomHour, randomMinute);
    QTest::newRow("6 digits (HHMMSS)") << randomHourString + randomMinuteString + randomSecondString << QTime(randomHour, randomMinute, randomSecond);
    
    QString precisionString = "F";
    for (int i = 1; i < 7; ++i)
    {
        int leftChars = (i > 3) ? 3 : i;
        QString microToMiliSecondString = randomMicroSecondString.left(leftChars);

        QTest::newRow(qPrintable(QString("%1 digits (HHMMSS.%2)").arg(i + 6).arg(precisionString))) 
            << randomHourString + randomMinuteString + randomSecondString + "." + randomMicroSecondString.left(i) 
            << QTime(randomHour, randomMinute, randomSecond, microToMiliSecondString.toInt());
        
        precisionString += "F";
    }

    int randomInvalidHour = MathTools::randomInt(24, 99);
    int randomInvalidMinute = MathTools::randomInt(60, 99);
    int randomInvalidSecond = MathTools::randomInt(61, 99);

    QString randomInvalidHourString = QString("%1").arg(randomInvalidHour, 2, 10, QChar('0'));
    QString randomInvalidMinuteString = QString("%1").arg(randomInvalidMinute, 2, 10, QChar('0'));
    QString randomInvalidSecondString = QString("%1").arg(randomInvalidSecond, 2, 10, QChar('0'));
    
    QTest::newRow("2 digits (HH) - hour invalid range") << randomInvalidHourString << QTime();
    QTest::newRow("4 digits (HHMM) - invalid hour & invalid minute") << randomInvalidHourString + randomInvalidMinuteString << QTime();
    QTest::newRow("6 digits (HHMMSS) - invalid hour & invalid minute & invalid second") << randomInvalidHourString + randomInvalidMinuteString + randomInvalidSecondString << QTime();
    QTest::newRow("4 digits (HHMM) - hour ok & invalid minute") << randomHourString + randomInvalidMinuteString << QTime();
    QTest::newRow("4 digits (HHMM) - invalid hour & minute ok") << randomInvalidHourString + randomMinuteString << QTime();
    QTest::newRow("6 digits (HHMMSS) - hour ok & invalid (minute & second)") << randomHourString + randomInvalidMinuteString + randomInvalidSecondString << QTime();
    QTest::newRow("6 digits (HHMMSS) - hour ok & minute ok & invalid second") << randomHourString + randomMinuteString + randomInvalidSecondString << QTime();
    QTest::newRow("6 digits (HHMMSS) - hour ok & invalid minute & second ok") << randomHourString + randomInvalidMinuteString + randomSecondString << QTime();
    QTest::newRow("6 digits (HHMMSS) - invalid hour & minute ok && invalid second") << randomInvalidHourString + randomMinuteString + randomInvalidSecondString << QTime();
    QTest::newRow("6 digits (HHMMSS) - invalid hour & invalid minute && second ok") << randomInvalidHourString + randomInvalidMinuteString + randomSecondString << QTime();
}

void test_DICOMValueRepresentationConverter::timeToQTime_ShouldReturnExpectedValues()
{
    QFETCH(QString, timeString);
    QFETCH(QTime, expectedTime);
    
    QCOMPARE(expectedTime, DICOMValueRepresentationConverter::timeToQTime(timeString));
}

DECLARE_TEST(test_DICOMValueRepresentationConverter)

#include "test_dicomvaluerepresentationconverter.moc"
