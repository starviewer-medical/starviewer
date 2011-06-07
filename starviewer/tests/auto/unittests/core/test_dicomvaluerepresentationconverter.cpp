#include "autotest.h"
#include "dicomvaluerepresentationconverter.h"

using namespace udg;

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

DECLARE_TEST(test_DICOMValueRepresentationConverter)

#include "test_dicomvaluerepresentationconverter.moc"
