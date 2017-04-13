#include "autotest.h"
#include "imageorientation.h"

using namespace udg;

class test_ImageOrientation : public QObject {
Q_OBJECT

private slots:
    // Casos en que passem un string formatat correctament
    void setDICOMFormattedImageOrientation_ShouldModifyVectorValuesAndReturnTrue_data();
    void setDICOMFormattedImageOrientation_ShouldModifyVectorValuesAndReturnTrue();
    
    // Casos en que passem un string formatat incorrectament 
    void setDICOMFormattedImageOrientation_ShouldResetVectorsToDefaultValuesAndReturnFalse_data();
    void setDICOMFormattedImageOrientation_ShouldResetVectorsToDefaultValuesAndReturnFalse();

    // Validar constructors
    void ImageOrientation_constructorInitialization_data();
    void ImageOrientation_constructorInitialization();

    // Validar setRownAndColumnVectors
    void setRowAndColumnVectors_UpdateVectorValues_data();
    void setRowAndColumnVectors_UpdateVectorValues();

    // Validar operador igualtat retorna cert/fals
    void operatorEquality_ShouldReturnTrue_data();
    void operatorEquality_ShouldReturnTrue();
    void operatorEquality_ShouldReturnFalse_data();
    void operatorEquality_ShouldReturnFalse();

private:
    void setupVectorsData();
};

Q_DECLARE_METATYPE(Vector3)

void test_ImageOrientation::setDICOMFormattedImageOrientation_ShouldModifyVectorValuesAndReturnTrue_data()
{
    QTest::addColumn<QString>("dicomFormattedOrientation");
    QTest::addColumn<Vector3>("rowVector");
    QTest::addColumn<Vector3>("columnVector");
    QTest::addColumn<Vector3>("normalVector");

    Vector3 defaultVectorValues(.0, .0, .0);

    QTest::newRow("six random number values correctly formatted") << "1\\2\\3\\0\\0\\0" << Vector3(1.0, 2.0, 3.0) << Vector3(.0, .0, .0) << Vector3(.0, .0, .0);
    
    QTest::newRow("six random floating point number values correctly formatted") << "1.0\\2.1\\3.\\.0\\.0\\3.5" << Vector3(1.0, 2.1, 3.0)
                                                                                 << Vector3(.0, .0, 3.5)
                                                                                 << Vector3::cross(Vector3(1.0, 2.1, 3.0), Vector3(.0, .0, 3.5));
    
    QTest::newRow("empty string") << "" << defaultVectorValues << defaultVectorValues << defaultVectorValues;
}

void test_ImageOrientation::setDICOMFormattedImageOrientation_ShouldModifyVectorValuesAndReturnTrue()
{
    QFETCH(QString, dicomFormattedOrientation);
    QFETCH(Vector3, rowVector);
    QFETCH(Vector3, columnVector);
    QFETCH(Vector3, normalVector);

    ImageOrientation imageOrientation;
    QCOMPARE(imageOrientation.setDICOMFormattedImageOrientation(dicomFormattedOrientation), true);

    QCOMPARE(imageOrientation.getRowVector(), rowVector);
    QCOMPARE(imageOrientation.getColumnVector(), columnVector);
    QCOMPARE(imageOrientation.getNormalVector(), normalVector);
}

void test_ImageOrientation::setDICOMFormattedImageOrientation_ShouldResetVectorsToDefaultValuesAndReturnFalse_data()
{
    QTest::addColumn<QString>("dicomFormattedOrientation");
    QTest::addColumn<Vector3>("rowVector");
    QTest::addColumn<Vector3>("columnVector");
    QTest::addColumn<Vector3>("normalVector");

    Vector3 defaultVectorValues(.0, .0, .0);

    QTest::newRow("six random string values") << "aaa\\bbbb\\ccc\\dddd\\eeee\\ffff" << defaultVectorValues << defaultVectorValues << defaultVectorValues;
    QTest::newRow("six elements with mixed alphanumeric and numeric values") << "aaa\\1\\bbbb\\.2\\cccc\\3.2" << defaultVectorValues << defaultVectorValues 
        << defaultVectorValues;
    
    QTest::newRow("six elements with mixed alphanumeric, numeric and empty values") << "aaa\\1\\\\.2\\cccc\\3.2" << defaultVectorValues << defaultVectorValues 
        << defaultVectorValues;
    
    QTest::newRow("six elements with numeric and empty elements") << "3.0\\1\\\\.2\\4.2\\3.2" << defaultVectorValues << defaultVectorValues 
        << defaultVectorValues;
    
    QTest::newRow("less than 6 numeric values") << "1.0\\2.1\\3." << defaultVectorValues << defaultVectorValues << defaultVectorValues;
    QTest::newRow("more than 6 numeric values") << "1.0\\1.0\\0.0\\0.0\\1.0\\0.2\\1.2\\1.8" << defaultVectorValues << defaultVectorValues << defaultVectorValues;
}

void test_ImageOrientation::setDICOMFormattedImageOrientation_ShouldResetVectorsToDefaultValuesAndReturnFalse()
{
    QFETCH(QString, dicomFormattedOrientation);
    QFETCH(Vector3, rowVector);
    QFETCH(Vector3, columnVector);
    QFETCH(Vector3, normalVector);

    ImageOrientation imageOrientation;
    QCOMPARE(imageOrientation.setDICOMFormattedImageOrientation(dicomFormattedOrientation), false);

    QCOMPARE(imageOrientation.getRowVector(), rowVector);
    QCOMPARE(imageOrientation.getColumnVector(), columnVector);
    QCOMPARE(imageOrientation.getNormalVector(), normalVector);
}

void test_ImageOrientation::ImageOrientation_constructorInitialization_data()
{
    this->setupVectorsData();
}

void test_ImageOrientation::ImageOrientation_constructorInitialization()
{
    QFETCH(Vector3, inputRowVector);
    QFETCH(Vector3, inputColumnVector);
    QFETCH(Vector3, memberRowVector);
    QFETCH(Vector3, memberColumnVector);

    ImageOrientation imageOrientation(inputRowVector, inputColumnVector);

    QCOMPARE(imageOrientation.getRowVector(), memberRowVector);
    QCOMPARE(imageOrientation.getColumnVector(), memberColumnVector);

    // Constructor sense paràmetres
    Vector3 zeroValuedVector(0.0,0.0, 0.0);
    ImageOrientation imageOrientationDefault;
    
    QCOMPARE(imageOrientationDefault.getRowVector(), zeroValuedVector);
    QCOMPARE(imageOrientationDefault.getColumnVector(), zeroValuedVector);
    QCOMPARE(imageOrientationDefault.getNormalVector(), zeroValuedVector);
}

void test_ImageOrientation::setRowAndColumnVectors_UpdateVectorValues_data()
{
    this->setupVectorsData();
}

void test_ImageOrientation::setRowAndColumnVectors_UpdateVectorValues()
{
    QFETCH(Vector3, inputRowVector);
    QFETCH(Vector3, inputColumnVector);
    QFETCH(Vector3, memberRowVector);
    QFETCH(Vector3, memberColumnVector);

    ImageOrientation imageOrientation;
    imageOrientation.setRowAndColumnVectors(inputRowVector, inputColumnVector);

    QCOMPARE(imageOrientation.getRowVector(), memberRowVector);
    QCOMPARE(imageOrientation.getColumnVector(), memberColumnVector);
}

void test_ImageOrientation::operatorEquality_ShouldReturnTrue_data()
{
    QTest::addColumn<Vector3>("rowVector");
    QTest::addColumn<Vector3>("columnVector");
    QTest::addColumn<QString>("orientationString");
    
    Vector3 row(2.0, 1.0, 3);
    Vector3 column(5.3, 2e-12, 33);

    QString orientationString("2\\1\\3\\5.3\\2e-12\\33");

    QTest::newRow("same image orientation contents") << row << column << orientationString;
}

void test_ImageOrientation::operatorEquality_ShouldReturnTrue()
{
    QFETCH(Vector3, rowVector);
    QFETCH(Vector3, columnVector);
    QFETCH(QString, orientationString);

    ImageOrientation imageOrientationConstructorInitialized(rowVector, columnVector);
    ImageOrientation imageOrientationInitializedBySetVectors;
    imageOrientationInitializedBySetVectors.setRowAndColumnVectors(rowVector, columnVector);
    ImageOrientation imageOrientationInitializedByString;
    imageOrientationInitializedByString.setDICOMFormattedImageOrientation(orientationString);

    QCOMPARE(imageOrientationConstructorInitialized == imageOrientationInitializedBySetVectors, true);
    QCOMPARE(imageOrientationConstructorInitialized == imageOrientationInitializedByString, true);
    QCOMPARE(imageOrientationInitializedBySetVectors == imageOrientationInitializedByString, true);
}

void test_ImageOrientation::operatorEquality_ShouldReturnFalse_data()
{
    QTest::addColumn<Vector3>("rowVector");
    QTest::addColumn<Vector3>("columnVector");
    QTest::addColumn<QString>("orientationString");
    
    Vector3 row(2.0, 1.0, 3);
    Vector3 column(5.3, 2e-12, 33);

    QString orientationString("0\\1\\2\\-6\\49\\0.3");

    QTest::newRow("different image orientation contents") << row << column << orientationString;
}

void test_ImageOrientation::operatorEquality_ShouldReturnFalse()
{
    QFETCH(Vector3, rowVector);
    QFETCH(Vector3, columnVector);
    QFETCH(QString, orientationString);

    ImageOrientation imageOrientationConstructorInitialized(rowVector, columnVector);
    ImageOrientation imageOrientationInitializedBySetVectors;
    imageOrientationInitializedBySetVectors.setRowAndColumnVectors(columnVector, rowVector);
    ImageOrientation imageOrientationInitializedByString;
    imageOrientationInitializedByString.setDICOMFormattedImageOrientation(orientationString);

    QCOMPARE(imageOrientationConstructorInitialized == imageOrientationInitializedBySetVectors, false);
    QCOMPARE(imageOrientationConstructorInitialized == imageOrientationInitializedByString, false);
    QCOMPARE(imageOrientationInitializedBySetVectors == imageOrientationInitializedByString, false);
}

void test_ImageOrientation::setupVectorsData()
{
    QTest::addColumn<Vector3>("inputRowVector");
    QTest::addColumn<Vector3>("inputColumnVector");
    QTest::addColumn<Vector3>("memberRowVector");
    QTest::addColumn<Vector3>("memberColumnVector");
    
    Vector3 defaultValueVector;
    Vector3 zeroVector(0.0, 0.0, 0.0);
    Vector3 randomValuedVector(0.1, 2.1, 0.3e15);
    Vector3 randomValuedVector2(-3.875, .1e-3, 450.23);
    
    QTest::newRow("initialize with empty vectors") << defaultValueVector << defaultValueVector << defaultValueVector << defaultValueVector;
    QTest::newRow("initialize with zero valued vectors") << zeroVector << zeroVector << zeroVector << zeroVector;
    QTest::newRow("initialize with different valued vectors") << randomValuedVector << randomValuedVector2 << randomValuedVector << randomValuedVector2;
}

DECLARE_TEST(test_ImageOrientation)

#include "test_imageorientation.moc"
