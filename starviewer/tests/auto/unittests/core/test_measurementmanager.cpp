#include "autotest.h"
#include "measurementmanager.h"

#include "image.h"

using namespace udg;

class test_MeasurementManager : public QObject {
Q_OBJECT

private:
    void setupGetMeasurementUnitsData();

private slots:
    void getMeasurementUnits_ReturnsExpectedValues_data();
    void getMeasurementUnits_ReturnsExpectedValues();
    
    void getMeasurementUnitsAsQString_ReturnsExpectedValues_data();
    void getMeasurementUnitsAsQString_ReturnsExpectedValues();
};

Q_DECLARE_METATYPE(Image*)
Q_DECLARE_METATYPE(MeasurementManager::MeasurementUnitsType)

void test_MeasurementManager::setupGetMeasurementUnitsData()
{
    QTest::addColumn<Image*>("image");
    QTest::addColumn<MeasurementManager::MeasurementUnitsType>("expectedResult");
    QTest::addColumn<QString>("expectedString");

    Image *image = 0;
    QTest::newRow("null image") << image << MeasurementManager::NotAvailable << tr("N/A");

    image = new Image(this);
    QTest::newRow("default pixel spacing values") << image << MeasurementManager::Pixels << tr("px");

    image = new Image(this);
    image->setPixelSpacing(0.0, 0.0);
    QTest::newRow("pixel spacing = 0.0, 0.0") << image << MeasurementManager::Pixels << tr("px");

    image = new Image(this);
    image->setPixelSpacing(1.0, 3.0);
    QTest::newRow("pixel spacing = 1.0, 3.0") << image << MeasurementManager::Millimetres << tr("mm");
}

void test_MeasurementManager::getMeasurementUnits_ReturnsExpectedValues_data()
{
    setupGetMeasurementUnitsData();
}

void test_MeasurementManager::getMeasurementUnits_ReturnsExpectedValues()
{
    QFETCH(Image*, image);
    QFETCH(MeasurementManager::MeasurementUnitsType, expectedResult);

    QCOMPARE(MeasurementManager::getMeasurementUnits(image), expectedResult);
}

void test_MeasurementManager::getMeasurementUnitsAsQString_ReturnsExpectedValues_data()
{
    setupGetMeasurementUnitsData();
}

void test_MeasurementManager::getMeasurementUnitsAsQString_ReturnsExpectedValues()
{
    QFETCH(Image*, image);
    QFETCH(QString, expectedString);

    QCOMPARE(MeasurementManager::getMeasurementUnitsAsQString(image), expectedString);
}

DECLARE_TEST(test_MeasurementManager)

#include "test_measurementmanager.moc"
