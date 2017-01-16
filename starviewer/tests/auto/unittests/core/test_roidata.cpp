#include "autotest.h"
#include "roidata.h"

#include "fuzzycomparetesthelper.h"
#include "voxel.h"

using namespace udg;
using namespace testing;

class test_ROIData : public QObject {
Q_OBJECT

private slots:
    void clear_ShouldInitializeData_data();
    void clear_ShouldInitializeData();

    void addVoxel_EmptyVoxelIsNotAdded_data();
    void addVoxel_EmptyVoxelIsNotAdded();
    
    void getMean_ReturnsExpectedData_data();
    void getMean_ReturnsExpectedData();

    void getStandardDeviation_ReturnsExpectedData_data();
    void getStandardDeviation_ReturnsExpectedData();
    
    void getMaximum_ReturnsExpectedData_data();
    void getMaximum_ReturnsExpectedData();

private:
    ROIData generateROIData();
};

Q_DECLARE_METATYPE(ROIData)

void test_ROIData::clear_ShouldInitializeData_data()
{
    QTest::addColumn<ROIData>("roiData");

    QTest::newRow("Random single valued voxels (clear)") << generateROIData();
}

void test_ROIData::clear_ShouldInitializeData()
{
    QFETCH(ROIData, roiData);

    roiData.clear();
    
    QCOMPARE(roiData.getMaximum(), 0.0);
    QCOMPARE(roiData.getMean(), 0.0);
    QCOMPARE(roiData.getStandardDeviation(), 0.0);
    QCOMPARE(roiData.getUnits(), QString());
    QCOMPARE(roiData.getModality(), QString());
}

void test_ROIData::addVoxel_EmptyVoxelIsNotAdded_data()
{
    QTest::addColumn<ROIData>("roiData");

    QTest::newRow("Random single valued voxels (add empty voxel)") << generateROIData();
}

void test_ROIData::addVoxel_EmptyVoxelIsNotAdded()
{
    QFETCH(ROIData, roiData);
    
    double meanBeforeAdd = roiData.getMean();
    
    Voxel emptyVoxel;
    roiData.addVoxel(emptyVoxel);
    
    QCOMPARE(meanBeforeAdd, roiData.getMean());
}

void test_ROIData::getMean_ReturnsExpectedData_data()
{
    QTest::addColumn<ROIData>("roiData");
    QTest::addColumn<double>("expectedMean");

    QTest::newRow("Random single valued voxels (mean)") << generateROIData() << 5.5;
}

void test_ROIData::getMean_ReturnsExpectedData()
{
    QFETCH(ROIData, roiData);
    QFETCH(double, expectedMean);

    QCOMPARE(roiData.getMean(), expectedMean);
}

void test_ROIData::getStandardDeviation_ReturnsExpectedData_data()
{
    QTest::addColumn<ROIData>("roiData");
    QTest::addColumn<double>("expectedStandardDeviation");

    QTest::newRow("Random single valued voxels (std dev)") << generateROIData() << 2.872;
}

void test_ROIData::getStandardDeviation_ReturnsExpectedData()
{
    QFETCH(ROIData, roiData);
    QFETCH(double, expectedStandardDeviation);

    QVERIFY(FuzzyCompareTestHelper::fuzzyCompare(roiData.getStandardDeviation(), expectedStandardDeviation, 1.0e-3));
}

void test_ROIData::getMaximum_ReturnsExpectedData_data()
{
    QTest::addColumn<ROIData>("roiData");
    QTest::addColumn<double>("expectedMaximum");
    
    QTest::newRow("Random single valued voxels (max)") << generateROIData() << 10.0;
}

void test_ROIData::getMaximum_ReturnsExpectedData()
{
    QFETCH(ROIData, roiData);
    QFETCH(double, expectedMaximum);

    QCOMPARE(roiData.getMaximum(), expectedMaximum);
}

ROIData test_ROIData::generateROIData()
{
    ROIData roiData;

    double value = 1.0;
    for (int i = 0; i < 10; ++i)
    {
        Voxel voxel;
        voxel.addComponent(value);

        roiData.addVoxel(voxel);
        
        value += 1.0;
    }

    return roiData;
}

DECLARE_TEST(test_ROIData)

#include "test_roidata.moc"
