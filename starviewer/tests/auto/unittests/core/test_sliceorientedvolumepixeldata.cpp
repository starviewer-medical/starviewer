#include "autotest.h"
#include "sliceorientedvolumepixeldata.h"

#include "fuzzycomparetesthelper.h"
#include "volumepixeldata.h"
#include "volumepixeldatatesthelper.h"
#include "voxel.h"
#include "voxelindex.h"

#include <vtkMatrix4x4.h>

using namespace testing;
using namespace udg;

namespace udg {

char* toString(const VoxelIndex &index)
{
    return QTest::toString(QString("VoxelIndex(%1, %2, %3)").arg(index.x()).arg(index.y()).arg(index.z()));
}

}

class test_SliceOrientedVolumePixelData : public QObject {

    Q_OBJECT

private slots:
    void constructor_InitializesAsExpected();

    void setDataToWorldMatrix_SetsBothMatrices_data();
    void setDataToWorldMatrix_SetsBothMatrices();

    void getExtent_PermutesAxesCorrectly_data();
    void getExtent_PermutesAxesCorrectly();

    void getSpacing_PermutesAxesCorrectly_data();
    void getSpacing_PermutesAxesCorrectly();

    void getVoxelIndex_ReturnsExpectedIndex_data();
    void getVoxelIndex_ReturnsExpectedIndex();

    void getVoxelValue_PermutesAxesCorrectly_data();
    void getVoxelValue_PermutesAxesCorrectly();

    void getWorldCoordinate_ReturnsExpectedCoordinate_data();
    void getWorldCoordinate_ReturnsExpectedCoordinate();

};

typedef std::array<int, 6> Extent;

Q_DECLARE_METATYPE(Extent)
Q_DECLARE_METATYPE(OrthogonalPlane)
Q_DECLARE_METATYPE(Vector3)
Q_DECLARE_METATYPE(Voxel)
Q_DECLARE_METATYPE(VoxelIndex)
Q_DECLARE_METATYPE(vtkSmartPointer<vtkMatrix4x4>)

void test_SliceOrientedVolumePixelData::constructor_InitializesAsExpected()
{
    SliceOrientedVolumePixelData data;

    QCOMPARE(data.getOrthogonalPlane(), OrthogonalPlane(OrthogonalPlane::XYPlane));

    auto identity = vtkSmartPointer<vtkMatrix4x4>::New();
    identity->Identity();

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            QCOMPARE(data.getDataToWorldMatrix()->GetElement(i, j), identity->GetElement(i, j));
            QCOMPARE(data.getWorldToDataMatrix()->GetElement(i, j), identity->GetElement(i, j));
        }
    }
}

void test_SliceOrientedVolumePixelData::setDataToWorldMatrix_SetsBothMatrices_data()
{
    QTest::addColumn<vtkSmartPointer<vtkMatrix4x4>>("dataToWorldMatrix");
    QTest::addColumn<vtkSmartPointer<vtkMatrix4x4>>("worldToDataMatrix");

    double dtw[16] = {  0.808, -0.115,  0.577, 8,
                        0.437,  0.437,  0.786, 0,
                       -0.513,  0.085, -0.854, 0,
                        0    ,  0    ,  0    , 1  };
    auto dataToWorld = vtkSmartPointer<vtkMatrix4x4>::New();
    dataToWorld->DeepCopy(dtw);

    double wtd[16] = {  2.186, 0.244,  1.702, -17.488,
                        0.149, 1.958,  1.902,  -1.193,
                       -1.298, 0.048, -2.004,  10.386,
                        0    , 0    ,  0    ,   1      };
    auto worldToData = vtkSmartPointer<vtkMatrix4x4>::New();
    worldToData->DeepCopy(wtd);

    QTest::newRow("") << dataToWorld << worldToData;
}

void test_SliceOrientedVolumePixelData::setDataToWorldMatrix_SetsBothMatrices()
{
    QFETCH(vtkSmartPointer<vtkMatrix4x4>, dataToWorldMatrix);
    QFETCH(vtkSmartPointer<vtkMatrix4x4>, worldToDataMatrix);

    SliceOrientedVolumePixelData data;
    data.setDataToWorldMatrix(dataToWorldMatrix);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(data.getDataToWorldMatrix()->GetElement(i, j), dataToWorldMatrix->GetElement(i, j), 0.001),
                     qPrintable(QString("%1 != %2").arg(data.getDataToWorldMatrix()->GetElement(i, j)).arg(dataToWorldMatrix->GetElement(i, j))));
            QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(data.getWorldToDataMatrix()->GetElement(i, j), worldToDataMatrix->GetElement(i, j), 0.001),
                     qPrintable(QString("%1 != %2").arg(data.getWorldToDataMatrix()->GetElement(i, j)).arg(worldToDataMatrix->GetElement(i, j))));
        }
    }
}

void test_SliceOrientedVolumePixelData::getExtent_PermutesAxesCorrectly_data()
{
    QTest::addColumn<VolumePixelData*>("volumePixelData");
    QTest::addColumn<OrthogonalPlane>("orthogonalPlane");
    QTest::addColumn<Extent>("expectedExtent");

    int dimensions[3] = { 25, 100, 18 };
    int extent[6] = { 0, 24, 0, 99, 0, 17 };
    double spacing[3] = { 0.1, 0.2, 0.3 };
    double origin[3] = { 10, 20, 30 };

    QTest::newRow("xy") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin)
                        << OrthogonalPlane(OrthogonalPlane::XYPlane) << Extent{{0, 24, 0, 99, 0, 17}};
    QTest::newRow("xz") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin)
                        << OrthogonalPlane(OrthogonalPlane::XZPlane) << Extent{{0, 24, 0, 17, 0, 99}};
    QTest::newRow("yz") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin)
                        << OrthogonalPlane(OrthogonalPlane::YZPlane) << Extent{{0, 99, 0, 17, 0, 24}};
}

void test_SliceOrientedVolumePixelData::getExtent_PermutesAxesCorrectly()
{
    QFETCH(VolumePixelData*, volumePixelData);
    QFETCH(OrthogonalPlane, orthogonalPlane);
    QFETCH(Extent, expectedExtent);

    SliceOrientedVolumePixelData data;
    data.setVolumePixelData(volumePixelData).setOrthogonalPlane(orthogonalPlane);

    QCOMPARE(data.getExtent(), expectedExtent);

    delete volumePixelData;
}

void test_SliceOrientedVolumePixelData::getSpacing_PermutesAxesCorrectly_data()
{
    QTest::addColumn<VolumePixelData*>("volumePixelData");
    QTest::addColumn<OrthogonalPlane>("orthogonalPlane");
    QTest::addColumn<Vector3>("expectedSpacing");

    int dimensions[3] = { 25, 100, 18 };
    int extent[6] = { 0, 24, 0, 99, 0, 17 };
    double spacing[3] = { 0.1, 0.2, 0.3 };
    double origin[3] = { 10, 20, 30 };

    QTest::newRow("xy") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin)
                        << OrthogonalPlane(OrthogonalPlane::XYPlane) << Vector3{0.1, 0.2, 0.3};
    QTest::newRow("xz") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin)
                        << OrthogonalPlane(OrthogonalPlane::XZPlane) << Vector3{0.1, 0.3, 0.2};
    QTest::newRow("yz") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin)
                        << OrthogonalPlane(OrthogonalPlane::YZPlane) << Vector3{0.2, 0.3, 0.1};
}

void test_SliceOrientedVolumePixelData::getSpacing_PermutesAxesCorrectly()
{
    QFETCH(VolumePixelData*, volumePixelData);
    QFETCH(OrthogonalPlane, orthogonalPlane);
    QFETCH(Vector3, expectedSpacing);

    SliceOrientedVolumePixelData data;
    data.setVolumePixelData(volumePixelData).setOrthogonalPlane(orthogonalPlane);

    QCOMPARE(data.getSpacing(), expectedSpacing);

    delete volumePixelData;
}

void test_SliceOrientedVolumePixelData::getVoxelIndex_ReturnsExpectedIndex_data()
{
    QTest::addColumn<VolumePixelData*>("volumePixelData");
    QTest::addColumn<OrthogonalPlane>("orthogonalPlane");
    QTest::addColumn<vtkSmartPointer<vtkMatrix4x4>>("dataToWorldMatrix");
    QTest::addColumn<Vector3>("worldCoordinate");
    QTest::addColumn<VoxelIndex>("expectedVoxelIndex");

    int dimensions[3] = { 25, 100, 18 };
    int extent[6] = { 0, 24, 0, 99, 0, 17 };
    double spacing[3] = { 0.1, 0.2, 0.3 };
    double origin[3] = { 10, 20, 30 };
    auto identity = vtkSmartPointer<vtkMatrix4x4>::New();
    identity->Identity();

    QTest::newRow("no data") << new VolumePixelData() << OrthogonalPlane() << identity << Vector3() << VoxelIndex();

    QTest::newRow("outside") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin)
                             << OrthogonalPlane(OrthogonalPlane::XYPlane) << identity << Vector3{-1, -100, 250} << VoxelIndex();

    QTest::newRow("xy") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin)
                        << OrthogonalPlane(OrthogonalPlane::XYPlane) << identity << Vector3{10, 24, 33} << VoxelIndex(0, 20, 10);
    QTest::newRow("xz") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin)
                        << OrthogonalPlane(OrthogonalPlane::XZPlane) << identity << Vector3{10, 24, 33} << VoxelIndex(0, 10, 20);
    QTest::newRow("yz") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin)
                        << OrthogonalPlane(OrthogonalPlane::YZPlane) << identity << Vector3{10, 24, 33} << VoxelIndex(20, 10, 0);

    double dtw[16] = {  0.808, -0.115,  0.577, 8,
                        0.437,  0.437,  0.786, 0,
                       -0.513,  0.085, -0.854, 0,
                        0    ,  0    ,  0    , 1  };
    auto dataToWorld = vtkSmartPointer<vtkMatrix4x4>::New();
    dataToWorld->DeepCopy(dtw);

    QTest::newRow("matrix") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin)
                            << OrthogonalPlane(OrthogonalPlane::XYPlane) << dataToWorld << Vector3{32, 40, -30} << VoxelIndex(12, 24, 3);
}

void test_SliceOrientedVolumePixelData::getVoxelIndex_ReturnsExpectedIndex()
{
    QFETCH(VolumePixelData*, volumePixelData);
    QFETCH(OrthogonalPlane, orthogonalPlane);
    QFETCH(vtkSmartPointer<vtkMatrix4x4>, dataToWorldMatrix);
    QFETCH(Vector3, worldCoordinate);
    QFETCH(VoxelIndex, expectedVoxelIndex);

    SliceOrientedVolumePixelData data;
    data.setVolumePixelData(volumePixelData).setOrthogonalPlane(orthogonalPlane).setDataToWorldMatrix(dataToWorldMatrix);

    QCOMPARE(data.getVoxelIndex(worldCoordinate), expectedVoxelIndex);

    delete volumePixelData;
}

void test_SliceOrientedVolumePixelData::getVoxelValue_PermutesAxesCorrectly_data()
{
    QTest::addColumn<VolumePixelData*>("volumePixelData");
    QTest::addColumn<OrthogonalPlane>("orthogonalPlane");
    QTest::addColumn<VoxelIndex>("voxelIndex");
    QTest::addColumn<Voxel>("expectedValue");

    int dimensions[3] = { 25, 100, 18 };
    int extent[6] = { 0, 24, 0, 99, 0, 17 };
    double spacing[3] = { 0.1, 0.2, 0.3 };
    double origin[3] = { 10, 20, 30 };
    Voxel voxel;
    voxel.addComponent(static_cast<short>(34756));

    QTest::newRow("xy") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin)
                        << OrthogonalPlane(OrthogonalPlane::XYPlane) << VoxelIndex(6, 90, 13) << voxel;
    QTest::newRow("xz") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin)
                        << OrthogonalPlane(OrthogonalPlane::XZPlane) << VoxelIndex(6, 13, 90) << voxel;
    QTest::newRow("yz") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin)
                        << OrthogonalPlane(OrthogonalPlane::YZPlane) << VoxelIndex(90, 13, 6) << voxel;
}

void test_SliceOrientedVolumePixelData::getVoxelValue_PermutesAxesCorrectly()
{
    QFETCH(VolumePixelData*, volumePixelData);
    QFETCH(OrthogonalPlane, orthogonalPlane);
    QFETCH(VoxelIndex, voxelIndex);
    QFETCH(Voxel, expectedValue);

    SliceOrientedVolumePixelData data;
    data.setVolumePixelData(volumePixelData).setOrthogonalPlane(orthogonalPlane);

    QCOMPARE(data.getVoxelValue(voxelIndex), expectedValue);

    delete volumePixelData;
}

void test_SliceOrientedVolumePixelData::getWorldCoordinate_ReturnsExpectedCoordinate_data()
{
    QTest::addColumn<VolumePixelData*>("volumePixelData");
    QTest::addColumn<OrthogonalPlane>("orthogonalPlane");
    QTest::addColumn<vtkSmartPointer<vtkMatrix4x4>>("dataToWorldMatrix");
    QTest::addColumn<VoxelIndex>("voxelIndex");
    QTest::addColumn<Vector3>("expectedWorldCoordinate");

    int dimensions[3] = { 25, 100, 18 };
    int extent[6] = { 0, 24, 0, 99, 0, 17 };
    double spacing[3] = { 0.1, 0.2, 0.3 };
    double origin[3] = { 10, 20, 30 };
    auto identity = vtkSmartPointer<vtkMatrix4x4>::New();
    identity->Identity();

    QTest::newRow("no data") << new VolumePixelData() << OrthogonalPlane() << identity << VoxelIndex() << Vector3() ;

    QTest::newRow("invalid index") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin)
                                   << OrthogonalPlane(OrthogonalPlane::XYPlane) << identity << VoxelIndex() << Vector3();

    QTest::newRow("outside") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin)
                             << OrthogonalPlane(OrthogonalPlane::XYPlane) << identity << VoxelIndex(100, 100, 100) << Vector3();

    QTest::newRow("xy") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin)
                        << OrthogonalPlane(OrthogonalPlane::XYPlane) << identity << VoxelIndex(0, 20, 10) << Vector3{10, 24, 33} ;
    QTest::newRow("xz") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin)
                        << OrthogonalPlane(OrthogonalPlane::XZPlane) << identity << VoxelIndex(0, 10, 20) << Vector3{10, 24, 33};
    QTest::newRow("yz") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin)
                        << OrthogonalPlane(OrthogonalPlane::YZPlane) << identity << VoxelIndex(20, 10, 0) << Vector3{10, 24, 33};

    double dtw[16] = {  0.808, -0.115,  0.577, 8,
                        0.437,  0.437,  0.786, 0,
                       -0.513,  0.085, -0.854, 0,
                        0    ,  0    ,  0    , 1  };
    auto dataToWorld = vtkSmartPointer<vtkMatrix4x4>::New();
    dataToWorld->DeepCopy(dtw);

    QTest::newRow("matrix") << VolumePixelDataTestHelper::createVolumePixelData(dimensions, extent, spacing, origin)
                            << OrthogonalPlane(OrthogonalPlane::XYPlane) << dataToWorld << VoxelIndex(12, 24, 3) << Vector3{32.0269, 40.0194, -30.0262};
}

void test_SliceOrientedVolumePixelData::getWorldCoordinate_ReturnsExpectedCoordinate()
{
    QFETCH(VolumePixelData*, volumePixelData);
    QFETCH(OrthogonalPlane, orthogonalPlane);
    QFETCH(vtkSmartPointer<vtkMatrix4x4>, dataToWorldMatrix);
    QFETCH(VoxelIndex, voxelIndex);
    QFETCH(Vector3, expectedWorldCoordinate);

    SliceOrientedVolumePixelData data;
    data.setVolumePixelData(volumePixelData).setOrthogonalPlane(orthogonalPlane).setDataToWorldMatrix(dataToWorldMatrix);

    QVERIFY2(FuzzyCompareTestHelper::fuzzyCompare(data.getWorldCoordinate(voxelIndex), expectedWorldCoordinate, 0.0001),
             qPrintable(QString("actual %1, expected %2").arg(data.getWorldCoordinate(voxelIndex).toString()).arg(expectedWorldCoordinate.toString())));

    delete volumePixelData;
}


DECLARE_TEST(test_SliceOrientedVolumePixelData)

#include "test_sliceorientedvolumepixeldata.moc"
