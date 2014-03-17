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

    void operatorAddition_ReturnsExpectedValue_data();
    void operatorAddition_ReturnsExpectedValue();
    
    void operatorAdditionAssignment_ReturnsExpectedValue_data();
    void operatorAdditionAssignment_ReturnsExpectedValue();

    void operatorSubstraction_ReturnsExpectedValue_data();
    void operatorSubstraction_ReturnsExpectedValue();

    void operatorSubstractionAssignment_ReturnsExpectedValue_data();
    void operatorSubstractionAssignment_ReturnsExpectedValue();
    
    void operatorDivision_ReturnsExpectedValue_data();
    void operatorDivision_ReturnsExpectedValue();

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

    int aboveRangeIndex = MathTools::randomInt(voxel.getNumberOfComponents(), std::numeric_limits<int>::max());
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

    Voxel voxel = generateVoxelWithNComponents(1);
    QTest::newRow("Single component voxel") << voxel << QString("%1").arg(voxel.getComponent(0));
    
    voxel = generateVoxelWithNComponents(MathTools::randomInt(2, 10));
    QString voxelString = QString("%1").arg(voxel.getComponent(0));
    for (int i = 1; i < voxel.getNumberOfComponents(); ++i)
    {
        voxelString += QString(", %1").arg(voxel.getComponent(i));
    }
    
    QTest::newRow("n-components (n > 1) voxel") << voxel << "(" + voxelString + ")";
}

void test_Voxel::getAsQString_ReturnsExpectedValues()
{
    QFETCH(Voxel, voxel);
    QFETCH(QString, expectedString);
    
    QCOMPARE(voxel.getAsQString(), expectedString);
}

void test_Voxel::operatorAddition_ReturnsExpectedValue_data()
{
    QTest::addColumn<Voxel>("additionVoxel1");
    QTest::addColumn<Voxel>("additionVoxel2");
    QTest::addColumn<Voxel>("expectedResult");

    QTest::newRow("Empty voxels") << Voxel() << Voxel() << Voxel();

    int nComponents = MathTools::randomInt(1, 8);
    Voxel nComponentsVoxel1 = generateVoxelWithNComponents(nComponents);
    Voxel nComponentsVoxel2 = generateVoxelWithNComponents(nComponents);

    Voxel result;
    for (int i = 0; i < nComponentsVoxel1.getNumberOfComponents(); ++i)
    {
        result.addComponent(nComponentsVoxel1.getComponent(i) + nComponentsVoxel2.getComponent(i));
    }
    QTest::newRow("same number of components") << nComponentsVoxel1 << nComponentsVoxel2 << result;

    Voxel between3_5ComponentsVoxel = generateVoxelWithNComponents(MathTools::randomInt(3, 5));
    Voxel between1_2ComponentsVoxel = generateVoxelWithNComponents(MathTools::randomInt(1, 2));

    result.reset();
    for (int i = 0; i < between1_2ComponentsVoxel.getNumberOfComponents(); ++i)
    {
        result.addComponent(between1_2ComponentsVoxel.getComponent(i) + between3_5ComponentsVoxel.getComponent(i));
    }
    for (int i = between1_2ComponentsVoxel.getNumberOfComponents(); i < between3_5ComponentsVoxel.getNumberOfComponents(); ++i)
    {
        result.addComponent(between3_5ComponentsVoxel.getComponent(i));
    }
    QTest::newRow("First voxel with less number of components") << between1_2ComponentsVoxel << between3_5ComponentsVoxel << result;

    result.reset();
    for (int i = 0; i < between1_2ComponentsVoxel.getNumberOfComponents(); ++i)
    {
        result.addComponent(between3_5ComponentsVoxel.getComponent(i) + between1_2ComponentsVoxel.getComponent(i));
    }
    for (int i = between1_2ComponentsVoxel.getNumberOfComponents(); i < between3_5ComponentsVoxel.getNumberOfComponents(); ++i)
    {
        result.addComponent(between3_5ComponentsVoxel.getComponent(i));
    }
    QTest::newRow("First voxel with more number of components") << between3_5ComponentsVoxel << between1_2ComponentsVoxel << result;
}

void test_Voxel::operatorAddition_ReturnsExpectedValue()
{
    QFETCH(Voxel, additionVoxel1);
    QFETCH(Voxel, additionVoxel2);
    QFETCH(Voxel, expectedResult);

    QCOMPARE(additionVoxel1 + additionVoxel2, expectedResult);
}

void test_Voxel::operatorAdditionAssignment_ReturnsExpectedValue_data()
{
    QTest::addColumn<Voxel>("voxelBeingAdded");
    QTest::addColumn<Voxel>("voxelToAdd");
    QTest::addColumn<Voxel>("expectedResult");

    QTest::newRow("Empty voxel + empty voxel") << Voxel() << Voxel() << Voxel();

    Voxel singleComponentVoxel = generateVoxelWithNComponents(1);

    Voxel resultingVoxel;
    resultingVoxel.addComponent(singleComponentVoxel.getComponent(0) + singleComponentVoxel.getComponent(0));
    QTest::newRow("Same number of components (single)") << singleComponentVoxel << singleComponentVoxel << resultingVoxel;

    int nComponents = MathTools::randomInt(2, 8);
    Voxel multipleComponentsVoxel1 = generateVoxelWithNComponents(nComponents);
    Voxel multipleComponentsVoxel2 = generateVoxelWithNComponents(nComponents);

    resultingVoxel.reset();
    for (int i = 0; i < multipleComponentsVoxel1.getNumberOfComponents(); ++i)
    {
        resultingVoxel.addComponent(multipleComponentsVoxel1.getComponent(i) + multipleComponentsVoxel2.getComponent(i));
    }
    
    QTest::newRow("Same number of components (multiple)") << multipleComponentsVoxel1 << multipleComponentsVoxel2 << resultingVoxel;

    resultingVoxel.reset();
    resultingVoxel.addComponent(multipleComponentsVoxel1.getComponent(0) + singleComponentVoxel.getComponent(0));
    for (int i = 1; i < multipleComponentsVoxel1.getNumberOfComponents(); ++i)
    {
        resultingVoxel.addComponent(multipleComponentsVoxel1.getComponent(i));
    }
    QTest::newRow("Different number of components (multiple + single)") << multipleComponentsVoxel1 << singleComponentVoxel << resultingVoxel;

    resultingVoxel.reset();
    resultingVoxel.addComponent(singleComponentVoxel.getComponent(0) + multipleComponentsVoxel1.getComponent(0));
    for (int i = 1; i < multipleComponentsVoxel1.getNumberOfComponents(); ++i)
    {
        resultingVoxel.addComponent(multipleComponentsVoxel1.getComponent(i));
    }
    QTest::newRow("Different number of components (single + multiple)") << singleComponentVoxel << multipleComponentsVoxel1 << resultingVoxel;
}

void test_Voxel::operatorAdditionAssignment_ReturnsExpectedValue()
{
    QFETCH(Voxel, voxelBeingAdded);
    QFETCH(Voxel, voxelToAdd);
    QFETCH(Voxel, expectedResult);

    voxelBeingAdded += voxelToAdd;

    QCOMPARE(voxelBeingAdded, expectedResult);
}

void test_Voxel::operatorSubstraction_ReturnsExpectedValue_data()
{
    QTest::addColumn<Voxel>("substractVoxel1");
    QTest::addColumn<Voxel>("substractVoxel2");
    QTest::addColumn<Voxel>("expectedResult");

    QTest::newRow("Empty voxels") << Voxel() << Voxel() << Voxel();

    int nComponents = MathTools::randomInt(1, 8);
    Voxel nComponentsVoxel1 = generateVoxelWithNComponents(nComponents);
    Voxel nComponentsVoxel2 = generateVoxelWithNComponents(nComponents);

    Voxel result;
    for (int i = 0; i < nComponentsVoxel1.getNumberOfComponents(); ++i)
    {
        result.addComponent(nComponentsVoxel1.getComponent(i) - nComponentsVoxel2.getComponent(i));
    }
    QTest::newRow("same number of components") << nComponentsVoxel1 << nComponentsVoxel2 << result;

    Voxel between3_5ComponentsVoxel = generateVoxelWithNComponents(MathTools::randomInt(3, 5));
    Voxel between1_2ComponentsVoxel = generateVoxelWithNComponents(MathTools::randomInt(1, 2));

    result.reset();
    for (int i = 0; i < between1_2ComponentsVoxel.getNumberOfComponents(); ++i)
    {
        result.addComponent(between1_2ComponentsVoxel.getComponent(i) - between3_5ComponentsVoxel.getComponent(i));
    }
    for (int i = between1_2ComponentsVoxel.getNumberOfComponents(); i < between3_5ComponentsVoxel.getNumberOfComponents(); ++i)
    {
        result.addComponent(-between3_5ComponentsVoxel.getComponent(i));
    }
    QTest::newRow("First voxel with less number of components") << between1_2ComponentsVoxel << between3_5ComponentsVoxel << result;

    result.reset();
    for (int i = 0; i < between1_2ComponentsVoxel.getNumberOfComponents(); ++i)
    {
        result.addComponent(between3_5ComponentsVoxel.getComponent(i) - between1_2ComponentsVoxel.getComponent(i));
    }
    for (int i = between1_2ComponentsVoxel.getNumberOfComponents(); i < between3_5ComponentsVoxel.getNumberOfComponents(); ++i)
    {
        result.addComponent(between3_5ComponentsVoxel.getComponent(i));
    }
    QTest::newRow("First voxel with more number of components") << between3_5ComponentsVoxel << between1_2ComponentsVoxel << result;
}

void test_Voxel::operatorSubstraction_ReturnsExpectedValue()
{
    QFETCH(Voxel, substractVoxel1);
    QFETCH(Voxel, substractVoxel2);
    QFETCH(Voxel, expectedResult);

    QCOMPARE(substractVoxel1 - substractVoxel2, expectedResult);
}

void test_Voxel::operatorSubstractionAssignment_ReturnsExpectedValue_data()
{
    QTest::addColumn<Voxel>("voxelBeingSubstracted");
    QTest::addColumn<Voxel>("voxelToSubstract");
    QTest::addColumn<Voxel>("expectedResult");

    QTest::newRow("Empty voxel + empty voxel") << Voxel() << Voxel() << Voxel();

    Voxel singleComponentVoxel = generateVoxelWithNComponents(1);

    Voxel resultingVoxel;
    resultingVoxel.addComponent(singleComponentVoxel.getComponent(0) - singleComponentVoxel.getComponent(0));
    QTest::newRow("Same number of components (single)") << singleComponentVoxel << singleComponentVoxel << resultingVoxel;

    int nComponents = MathTools::randomInt(2, 8);
    Voxel multipleComponentsVoxel1 = generateVoxelWithNComponents(nComponents);
    Voxel multipleComponentsVoxel2 = generateVoxelWithNComponents(nComponents);

    resultingVoxel.reset();
    for (int i = 0; i < multipleComponentsVoxel1.getNumberOfComponents(); ++i)
    {
        resultingVoxel.addComponent(multipleComponentsVoxel1.getComponent(i) - multipleComponentsVoxel2.getComponent(i));
    }
    
    QTest::newRow("Same number of components (multiple)") << multipleComponentsVoxel1 << multipleComponentsVoxel2 << resultingVoxel;

    resultingVoxel.reset();
    resultingVoxel.addComponent(multipleComponentsVoxel1.getComponent(0) - singleComponentVoxel.getComponent(0));
    for (int i = 1; i < multipleComponentsVoxel1.getNumberOfComponents(); ++i)
    {
        resultingVoxel.addComponent(multipleComponentsVoxel1.getComponent(i));
    }
    QTest::newRow("Different number of components (multiple + single)") << multipleComponentsVoxel1 << singleComponentVoxel << resultingVoxel;

    resultingVoxel.reset();
    resultingVoxel.addComponent(singleComponentVoxel.getComponent(0) - multipleComponentsVoxel1.getComponent(0));
    for (int i = 1; i < multipleComponentsVoxel1.getNumberOfComponents(); ++i)
    {
        resultingVoxel.addComponent(-multipleComponentsVoxel1.getComponent(i));
    }
    QTest::newRow("Different number of components (single + multiple)") << singleComponentVoxel << multipleComponentsVoxel1 << resultingVoxel;
}

void test_Voxel::operatorSubstractionAssignment_ReturnsExpectedValue()
{
    QFETCH(Voxel, voxelBeingSubstracted);
    QFETCH(Voxel, voxelToSubstract);
    QFETCH(Voxel, expectedResult);

    voxelBeingSubstracted -= voxelToSubstract;

    QCOMPARE(voxelBeingSubstracted, expectedResult);
}

void test_Voxel::operatorDivision_ReturnsExpectedValue_data()
{
    QTest::addColumn<Voxel>("voxelToDivide");
    QTest::addColumn<double>("divisor");
    QTest::addColumn<Voxel>("expectedResult");

    double randomDivisor = MathTools::randomDouble(-100, 1000);
    
    QTest::newRow("Empty Voxel") << Voxel() << randomDivisor << Voxel();

    Voxel singleComponentVoxel;
    singleComponentVoxel.addComponent(5.0);

    Voxel dividedVoxel;
    dividedVoxel.addComponent(singleComponentVoxel.getComponent(0) / randomDivisor);
    QTest::newRow("Single component voxel") << singleComponentVoxel << randomDivisor << dividedVoxel;

    Voxel multipleComponentsVoxel = generateVoxelWithNComponents(MathTools::randomInt(2, 8));

    dividedVoxel.reset();
    for (int i = 0; i < multipleComponentsVoxel.getNumberOfComponents(); ++i)
    {
        dividedVoxel.addComponent(multipleComponentsVoxel.getComponent(i) / randomDivisor);
    }
    QTest::newRow("Multiple component voxel") << multipleComponentsVoxel << randomDivisor << dividedVoxel;
}

void test_Voxel::operatorDivision_ReturnsExpectedValue()
{
    QFETCH(Voxel, voxelToDivide);
    QFETCH(double, divisor);
    QFETCH(Voxel, expectedResult);
    
    QCOMPARE(voxelToDivide / divisor, expectedResult);
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
