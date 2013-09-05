#include "autotest.h"
#include "voxel.h"

#include "mathtools.h"

using namespace udg;

class test_Voxel : public QObject {
Q_OBJECT

private slots:
    void addComponent_ShouldIncreaseNumberOfComponentsByOne_data();
    void addComponent_ShouldIncreaseNumberOfComponentsByOne();

    void getComponent_ReturnsNaN_data();
    void getComponent_ReturnsNaN();
    
    void isEmpty_ReturnsExpectedValue_data();
    void isEmpty_ReturnsExpectedValue();
    
    void reset_SetsVoxelWithNoComponents_data();
    void reset_SetsVoxelWithNoComponents();
    
    void getAsQString_ReturnsExpectedValues_data();
    void getAsQString_ReturnsExpectedValues();

private:
    /// Data for isEmpty() and reset() tests
    void setupEmptyAndRandomNumberOfComponentsVoxelsData();

    /// Returns a Voxel with the specified number of components. The value of each component is random.
    Voxel generateVoxelWithNComponents(unsigned int n);
};

Q_DECLARE_METATYPE(Voxel)

void test_Voxel::addComponent_ShouldIncreaseNumberOfComponentsByOne_data()
{
    QTest::addColumn<Voxel>("voxel");
    QTest::addColumn<int>("expectedNumberOfComponents");

    Voxel voxel = generateVoxelWithNComponents(MathTools::randomInt(0, 10));

    QTest::newRow(qPrintable(QString("Voxel with %1 components").arg(voxel.getNumberOfComponents()))) << voxel << voxel.getNumberOfComponents() + 1;
}

void test_Voxel::addComponent_ShouldIncreaseNumberOfComponentsByOne()
{
    QFETCH(Voxel, voxel);
    QFETCH(int, expectedNumberOfComponents);
    
    voxel.addComponent(MathTools::randomDouble(std::numeric_limits<double>::min(), std::numeric_limits<double>::max()));
    
    QCOMPARE(voxel.getNumberOfComponents(), expectedNumberOfComponents);
}

void test_Voxel::getComponent_ReturnsNaN_data()
{
    QTest::addColumn<Voxel>("voxel");
    QTest::addColumn<int>("i");

    Voxel voxel = generateVoxelWithNComponents(MathTools::randomInt(0, 10));
    
    int belowRangeIndex = MathTools::randomInt(std::numeric_limits<int>::min(), -1);
    QTest::newRow("Component index below number of components") << voxel << belowRangeIndex;

    int aboveRangeIndex = MathTools::randomInt(voxel.getNumberOfComponents() + 1, std::numeric_limits<int>::max());
    QTest::newRow("Component index above number of components") << voxel << aboveRangeIndex;
}

void test_Voxel::getComponent_ReturnsNaN()
{
    QFETCH(Voxel, voxel);
    QFETCH(int, i);
    
    QVERIFY(MathTools::isNaN(voxel.getComponent(i)));
}

void test_Voxel::isEmpty_ReturnsExpectedValue_data()
{
    setupEmptyAndRandomNumberOfComponentsVoxelsData();
}

void test_Voxel::isEmpty_ReturnsExpectedValue()
{
    QFETCH(Voxel, voxel);
    QFETCH(bool, expectedEmptyValue);
    
    QCOMPARE(voxel.isEmpty(), expectedEmptyValue);
}

void test_Voxel::reset_SetsVoxelWithNoComponents_data()
{
    setupEmptyAndRandomNumberOfComponentsVoxelsData();
}

void test_Voxel::reset_SetsVoxelWithNoComponents()
{
    QFETCH(Voxel, voxel);

    voxel.reset();
    QCOMPARE(voxel.getNumberOfComponents(), 0);
}

void test_Voxel::getAsQString_ReturnsExpectedValues_data()
{
    QTest::addColumn<Voxel>("voxel");
    QTest::addColumn<QString>("expectedString");

    QTest::newRow("Empty voxel") << Voxel() << tr("N/A");

    Voxel voxel = generateVoxelWithNComponents(MathTools::randomInt(1, 10));
    QString voxelString;
    voxelString = QString("%1").arg(voxel.getComponent(0));
    for (int i = 1; i < voxel.getNumberOfComponents(); ++i)
    {
        voxelString += QString(", %1").arg(voxel.getComponent(i));
    }
    QTest::newRow("Not empty voxel") << voxel << "(" + voxelString + ")";
}

void test_Voxel::getAsQString_ReturnsExpectedValues()
{
    QFETCH(Voxel, voxel);
    QFETCH(QString, expectedString);
    
    QCOMPARE(voxel.getAsQString(), expectedString);
}

void test_Voxel::setupEmptyAndRandomNumberOfComponentsVoxelsData()
{
    QTest::addColumn<Voxel>("voxel");
    QTest::addColumn<bool>("expectedEmptyValue");

    QTest::newRow("Just created voxel") << Voxel() << true;

    Voxel voxel = generateVoxelWithNComponents(MathTools::randomInt(1, 10));
    QTest::newRow("Voxel with at least one component") << voxel << false;
}

Voxel test_Voxel::generateVoxelWithNComponents(unsigned int n)
{
    Voxel voxel;
    for (unsigned int i = 0; i < n; ++i)
    {
        voxel.addComponent(MathTools::randomDouble(std::numeric_limits<double>::min(), std::numeric_limits<double>::max()));
    }

    return voxel;
}

DECLARE_TEST(test_Voxel)

#include "test_voxel.moc"
