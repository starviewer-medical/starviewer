#include "autotest.h"
#include "volumepixeldataiterator.h"

#include "volumepixeldata.h"

#include <vtkImageData.h>

using namespace udg;

namespace {

// Relying on QObject trees to destroy the VolumePixelData (i.e. creating it with VolumePixelData(this) and forgetting about it) produces random crashes in Mac,
// so we create this auxiliar struct to keep it with the iterator and destroy both at the same time; this struct will be hold by a QSharedPointer.
struct DataIterator {
    VolumePixelData *data = nullptr;
    VolumePixelDataIterator *iterator = nullptr;
    ~DataIterator() {
        delete data;
        delete iterator;
    }
};

}

class test_VolumePixelDataIterator : public QObject {

    Q_OBJECT

private slots:

    void isNull_ShouldReturnExpectedValue_data();
    void isNull_ShouldReturnExpectedValue();

    void get_ShouldReturnExpectedValue_data();
    void get_ShouldReturnExpectedValue();

    void set_ShouldSetExpectedValue_data();
    void set_ShouldSetExpectedValue();

    void operatorAdd_ShouldReturnExpectedValue_data();
    void operatorAdd_ShouldReturnExpectedValue();

    void operatorSubtract_ShouldReturnExpectedValue_data();
    void operatorSubtract_ShouldReturnExpectedValue();

    void operatorPreIncrement_ShouldIncrementAndReturnNewIterator_data();
    void operatorPreIncrement_ShouldIncrementAndReturnNewIterator();

    void operatorPostIncrement_ShouldIncrementAndReturnOldIterator_data();
    void operatorPostIncrement_ShouldIncrementAndReturnOldIterator();

    void operatorPreDecrement_ShouldDecrementAndReturnNewIterator_data();
    void operatorPreDecrement_ShouldDecrementAndReturnNewIterator();

    void operatorPostDecrement_ShouldDecrementAndReturnOldIterator_data();
    void operatorPostDecrement_ShouldDecrementAndReturnOldIterator();

    void operatorEquals_ShouldReturnExpectedValue_data();
    void operatorEquals_ShouldReturnExpectedValue();

    void operatorDifferent_ShouldReturnExpectedValue_data();
    void operatorDifferent_ShouldReturnExpectedValue();

    void operatorGreaterThan_ShouldReturnExpectedValue_data();
    void operatorGreaterThan_ShouldReturnExpectedValue();

    void operatorLessThan_ShouldReturnExpectedValue_data();
    void operatorLessThan_ShouldReturnExpectedValue();

    void operatorGreaterThanOrEqual_ShouldReturnExpectedValue_data();
    void operatorGreaterThanOrEqual_ShouldReturnExpectedValue();

    void operatorLessThanOrEqual_ShouldReturnExpectedValue_data();
    void operatorLessThanOrEqual_ShouldReturnExpectedValue();

    void operatorAddAndAssign_ShouldAddAndReturnExpectedValue_data();
    void operatorAddAndAssign_ShouldAddAndReturnExpectedValue();

    void operatorSubtractAndAssign_ShouldSubtractAndReturnExpectedValue_data();
    void operatorSubtractAndAssign_ShouldSubtractAndReturnExpectedValue();

private:
    QSharedPointer<DataIterator> createDataIterator(int x, int y, int z);
    QSharedPointer<VolumePixelDataIterator> createIterator(int x, int y, int z);
    void setupTestDataForAddOperators();
    void setupTestDataForSubtractOperators();
    void setupTestDataForComparisonOperators();

};

Q_DECLARE_METATYPE(QSharedPointer<DataIterator>)
Q_DECLARE_METATYPE(QSharedPointer<VolumePixelDataIterator>)

void test_VolumePixelDataIterator::isNull_ShouldReturnExpectedValue_data()
{
    QTest::addColumn< QSharedPointer<VolumePixelDataIterator> >("iterator");
    QTest::addColumn<bool>("isNull");

    VolumePixelData emptyVolumePixelData;
    QTest::newRow("null") << QSharedPointer<VolumePixelDataIterator>(new VolumePixelDataIterator(&emptyVolumePixelData)) << true;

    QTest::newRow("not null") << createIterator(0, 0, 0) << false;
}

void test_VolumePixelDataIterator::isNull_ShouldReturnExpectedValue()
{
    QFETCH(QSharedPointer<VolumePixelDataIterator>, iterator);
    QFETCH(bool, isNull);

    QCOMPARE(iterator->isNull(), isNull);
}

void test_VolumePixelDataIterator::get_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<QSharedPointer<DataIterator>>("dataIterator");
    QTest::addColumn<qint8>("expectedValueInt8");
    QTest::addColumn<quint8>("expectedValueUint8");
    QTest::addColumn<qint16>("expectedValueInt16");
    QTest::addColumn<quint16>("expectedValueUint16");
    QTest::addColumn<qint32>("expectedValueInt32");
    QTest::addColumn<quint32>("expectedValueUint32");
    QTest::addColumn<qint64>("expectedValueInt64");
    QTest::addColumn<quint64>("expectedValueUint64");
    QTest::addColumn<float>("expectedValueFloat");
    QTest::addColumn<double>("expectedValueDouble");

    QTest::newRow("get(3, 0, 1)") << createDataIterator(3, 0, 1) << qint8(245) << quint8(245) << qint16(1781) << quint16(1781)
                                                                 << qint32(819267317) << quint32(819267317) << qint64(819267317) << quint64(819267317)
                                                                 << 819267317.0f << 819267317.0;
    QTest::newRow("get(3, 1, 2)") << createDataIterator(3, 1, 2) << qint8(111) << quint8(111) << qint16(39279) << quint16(39279)
                                                                 << qint32(2333039) << quint32(2333039) << qint64(2333039) << quint64(2333039)
                                                                 << 2333039.0f << 2333039.0;
    QTest::newRow("get(1, 0, 1)") << createDataIterator(1, 0, 1) << qint8(17) << quint8(17) << qint16(49169) << quint16(49169)
                                                                 << qint32(408141841) << quint32(408141841) << qint64(408141841) << quint64(408141841)
                                                                 << 408141841.0f << 408141841.0;
}

void test_VolumePixelDataIterator::get_ShouldReturnExpectedValue()
{
    QFETCH(QSharedPointer<DataIterator>, dataIterator);
    QFETCH(qint8, expectedValueInt8);
    QFETCH(quint8, expectedValueUint8);
    QFETCH(qint16, expectedValueInt16);
    QFETCH(quint16, expectedValueUint16);
    QFETCH(qint32, expectedValueInt32);
    QFETCH(quint32, expectedValueUint32);
    QFETCH(qint64, expectedValueInt64);
    QFETCH(quint64, expectedValueUint64);
    QFETCH(float, expectedValueFloat);
    QFETCH(double, expectedValueDouble);

    QCOMPARE(dataIterator->iterator->get<qint8>(), expectedValueInt8);
    QCOMPARE(dataIterator->iterator->get<quint8>(), expectedValueUint8);
    QCOMPARE(dataIterator->iterator->get<qint16>(), expectedValueInt16);
    QCOMPARE(dataIterator->iterator->get<quint16>(), expectedValueUint16);
    QCOMPARE(dataIterator->iterator->get<qint32>(), expectedValueInt32);
    QCOMPARE(dataIterator->iterator->get<quint32>(), expectedValueUint32);
    QCOMPARE(dataIterator->iterator->get<qint64>(), expectedValueInt64);
    QCOMPARE(dataIterator->iterator->get<quint64>(), expectedValueUint64);
    QCOMPARE(dataIterator->iterator->get<float>(), expectedValueFloat);
    QCOMPARE(dataIterator->iterator->get<double>(), expectedValueDouble);
}

void test_VolumePixelDataIterator::set_ShouldSetExpectedValue_data()
{
    QTest::addColumn<QSharedPointer<DataIterator>>("dataIterator");
    QTest::addColumn<qint8>("setValueInt8");
    QTest::addColumn<unsigned int>("expectedValueInt8");
    QTest::addColumn<quint8>("setValueUint8");
    QTest::addColumn<unsigned int>("expectedValueUint8");
    QTest::addColumn<qint16>("setValueInt16");
    QTest::addColumn<unsigned int>("expectedValueInt16");
    QTest::addColumn<quint16>("setValueUint16");
    QTest::addColumn<unsigned int>("expectedValueUint16");
    QTest::addColumn<qint32>("setValueInt32");
    QTest::addColumn<unsigned int>("expectedValueInt32");
    QTest::addColumn<quint32>("setValueUint32");
    QTest::addColumn<unsigned int>("expectedValueUint32");
    QTest::addColumn<qint64>("setValueInt64");
    QTest::addColumn<unsigned int>("expectedValueInt64");
    QTest::addColumn<quint64>("setValueUint64");
    QTest::addColumn<unsigned int>("expectedValueUint64");
    QTest::addColumn<float>("setValueFloat");
    QTest::addColumn<unsigned int>("expectedValueFloat");
    QTest::addColumn<double>("setValueDouble");
    QTest::addColumn<unsigned int>("expectedValueDouble");

    QTest::newRow("set") << createDataIterator(0, 0, 0) << qint8(-55) << 4294967241u << quint8(168) << 168u
                                                        << qint16(-14160) << 4294953136u << quint16(26213) << 26213u
                                                        << qint32(-21140511) << 4273826785u << quint32(754190776) << 754190776u
                                                        << qint64(-193498463462335766) << 884890346u << quint64(10509583164190741238u) << 2802045686u
                                                        << 255565.98f << 255565u << 7.3271E85 << 0u;
}

void test_VolumePixelDataIterator::set_ShouldSetExpectedValue()
{
    QFETCH(QSharedPointer<DataIterator>, dataIterator);
    QFETCH(qint8, setValueInt8);
    QFETCH(unsigned int, expectedValueInt8);
    QFETCH(quint8, setValueUint8);
    QFETCH(unsigned int, expectedValueUint8);
    QFETCH(qint16, setValueInt16);
    QFETCH(unsigned int, expectedValueInt16);
    QFETCH(quint16, setValueUint16);
    QFETCH(unsigned int, expectedValueUint16);
    QFETCH(qint32, setValueInt32);
    QFETCH(unsigned int, expectedValueInt32);
    QFETCH(quint32, setValueUint32);
    QFETCH(unsigned int, expectedValueUint32);
    QFETCH(qint64, setValueInt64);
    QFETCH(unsigned int, expectedValueInt64);
    QFETCH(quint64, setValueUint64);
    QFETCH(unsigned int, expectedValueUint64);
    QFETCH(float, setValueFloat);
    QFETCH(unsigned int, expectedValueFloat);
    QFETCH(double, setValueDouble);
    QFETCH(unsigned int, expectedValueDouble);

    dataIterator->iterator->set(setValueInt8);
    QCOMPARE(dataIterator->iterator->get<unsigned int>(), expectedValueInt8);
    dataIterator->iterator->set(setValueUint8);
    QCOMPARE(dataIterator->iterator->get<unsigned int>(), expectedValueUint8);
    dataIterator->iterator->set(setValueInt16);
    QCOMPARE(dataIterator->iterator->get<unsigned int>(), expectedValueInt16);
    dataIterator->iterator->set(setValueUint16);
    QCOMPARE(dataIterator->iterator->get<unsigned int>(), expectedValueUint16);
    dataIterator->iterator->set(setValueInt32);
    QCOMPARE(dataIterator->iterator->get<unsigned int>(), expectedValueInt32);
    dataIterator->iterator->set(setValueUint32);
    QCOMPARE(dataIterator->iterator->get<unsigned int>(), expectedValueUint32);
    dataIterator->iterator->set(setValueInt64);
    QCOMPARE(dataIterator->iterator->get<unsigned int>(), expectedValueInt64);
    dataIterator->iterator->set(setValueUint64);
    QCOMPARE(dataIterator->iterator->get<unsigned int>(), expectedValueUint64);
    dataIterator->iterator->set(setValueFloat);
    QCOMPARE(dataIterator->iterator->get<unsigned int>(), expectedValueFloat);
    dataIterator->iterator->set(setValueDouble);
    QCOMPARE(dataIterator->iterator->get<unsigned int>(), expectedValueDouble);
}

void test_VolumePixelDataIterator::operatorAdd_ShouldReturnExpectedValue_data()
{
    setupTestDataForAddOperators();
}

void test_VolumePixelDataIterator::operatorAdd_ShouldReturnExpectedValue()
{
    QFETCH(QSharedPointer<DataIterator>, dataIterator);
    QFETCH(int, delta);
    QFETCH(unsigned int, expectedVoxelValue);

    QCOMPARE((*dataIterator->iterator + delta).get<unsigned int>(), expectedVoxelValue);
}

void test_VolumePixelDataIterator::operatorSubtract_ShouldReturnExpectedValue_data()
{
    setupTestDataForSubtractOperators();
}

void test_VolumePixelDataIterator::operatorSubtract_ShouldReturnExpectedValue()
{
    QFETCH(QSharedPointer<DataIterator>, dataIterator);
    QFETCH(int, delta);
    QFETCH(unsigned int, expectedVoxelValue);

    QCOMPARE((*dataIterator->iterator - delta).get<unsigned int>(), expectedVoxelValue);
}

void test_VolumePixelDataIterator::operatorPreIncrement_ShouldIncrementAndReturnNewIterator_data()
{
    QTest::addColumn<QSharedPointer<DataIterator>>("dataIterator");
    QTest::addColumn<unsigned int>("newVoxelValue");

    QTest::newRow("++it") << createDataIterator(0, 3, 1) << 584932269u;
}

void test_VolumePixelDataIterator::operatorPreIncrement_ShouldIncrementAndReturnNewIterator()
{
    QFETCH(QSharedPointer<DataIterator>, dataIterator);
    QFETCH(unsigned int, newVoxelValue);

    VolumePixelDataIterator incremented = ++(*dataIterator->iterator);

    QCOMPARE(dataIterator->iterator->get<unsigned int>(), newVoxelValue);
    QCOMPARE(incremented.get<unsigned int>(), newVoxelValue);
}

void test_VolumePixelDataIterator::operatorPostIncrement_ShouldIncrementAndReturnOldIterator_data()
{
    QTest::addColumn<QSharedPointer<DataIterator>>("dataIterator");
    QTest::addColumn<unsigned int>("newVoxelValue");
    QTest::addColumn<unsigned int>("oldVoxelValue");

    QTest::newRow("it++") << createDataIterator(3, 1, 2) << 626138607u << 2333039u;
}

void test_VolumePixelDataIterator::operatorPostIncrement_ShouldIncrementAndReturnOldIterator()
{
    QFETCH(QSharedPointer<DataIterator>, dataIterator);
    QFETCH(unsigned int, newVoxelValue);
    QFETCH(unsigned int, oldVoxelValue);

    VolumePixelDataIterator notIncremented = (*dataIterator->iterator)++;

    QCOMPARE(dataIterator->iterator->get<unsigned int>(), newVoxelValue);
    QCOMPARE(notIncremented.get<unsigned int>(), oldVoxelValue);
}

void test_VolumePixelDataIterator::operatorPreDecrement_ShouldDecrementAndReturnNewIterator_data()
{
    QTest::addColumn<QSharedPointer<DataIterator>>("dataIterator");
    QTest::addColumn<unsigned int>("newVoxelValue");

    QTest::newRow("--it") << createDataIterator(2, 1, 0) << 451505799u;
}

void test_VolumePixelDataIterator::operatorPreDecrement_ShouldDecrementAndReturnNewIterator()
{
    QFETCH(QSharedPointer<DataIterator>, dataIterator);
    QFETCH(unsigned int, newVoxelValue);

    VolumePixelDataIterator decremented = --(*dataIterator->iterator);

    QCOMPARE(dataIterator->iterator->get<unsigned int>(), newVoxelValue);
    QCOMPARE(decremented.get<unsigned int>(), newVoxelValue);
}

void test_VolumePixelDataIterator::operatorPostDecrement_ShouldDecrementAndReturnOldIterator_data()
{
    QTest::addColumn<QSharedPointer<DataIterator>>("dataIterator");
    QTest::addColumn<unsigned int>("newVoxelValue");
    QTest::addColumn<unsigned int>("oldVoxelValue");

    QTest::newRow("it--") << createDataIterator(3, 1, 0) << 99830523u << 680556899u;
}

void test_VolumePixelDataIterator::operatorPostDecrement_ShouldDecrementAndReturnOldIterator()
{
    QFETCH(QSharedPointer<DataIterator>, dataIterator);
    QFETCH(unsigned int, newVoxelValue);
    QFETCH(unsigned int, oldVoxelValue);

    VolumePixelDataIterator notDecremented = (*dataIterator->iterator)--;

    QCOMPARE(dataIterator->iterator->get<unsigned int>(), newVoxelValue);
    QCOMPARE(notDecremented.get<unsigned int>(), oldVoxelValue);
}

void test_VolumePixelDataIterator::operatorEquals_ShouldReturnExpectedValue_data()
{
    setupTestDataForComparisonOperators();
}

void test_VolumePixelDataIterator::operatorEquals_ShouldReturnExpectedValue()
{
    QFETCH(QSharedPointer<VolumePixelDataIterator>, iterator1);
    QFETCH(QSharedPointer<VolumePixelDataIterator>, iterator2);
    QFETCH(bool, equal);

    QCOMPARE(*iterator1 == *iterator2, equal);
}

void test_VolumePixelDataIterator::operatorDifferent_ShouldReturnExpectedValue_data()
{
    setupTestDataForComparisonOperators();
}

void test_VolumePixelDataIterator::operatorDifferent_ShouldReturnExpectedValue()
{
    QFETCH(QSharedPointer<VolumePixelDataIterator>, iterator1);
    QFETCH(QSharedPointer<VolumePixelDataIterator>, iterator2);
    QFETCH(bool, different);

    QCOMPARE(*iterator1 != *iterator2, different);
}

void test_VolumePixelDataIterator::operatorGreaterThan_ShouldReturnExpectedValue_data()
{
    setupTestDataForComparisonOperators();
}

void test_VolumePixelDataIterator::operatorGreaterThan_ShouldReturnExpectedValue()
{
    QFETCH(QSharedPointer<VolumePixelDataIterator>, iterator1);
    QFETCH(QSharedPointer<VolumePixelDataIterator>, iterator2);
    QFETCH(bool, greaterThan);

    QCOMPARE(*iterator1 > *iterator2, greaterThan);
}

void test_VolumePixelDataIterator::operatorLessThan_ShouldReturnExpectedValue_data()
{
    setupTestDataForComparisonOperators();
}

void test_VolumePixelDataIterator::operatorLessThan_ShouldReturnExpectedValue()
{
    QFETCH(QSharedPointer<VolumePixelDataIterator>, iterator1);
    QFETCH(QSharedPointer<VolumePixelDataIterator>, iterator2);
    QFETCH(bool, lessThan);

    QCOMPARE(*iterator1 < *iterator2, lessThan);
}

void test_VolumePixelDataIterator::operatorGreaterThanOrEqual_ShouldReturnExpectedValue_data()
{
    setupTestDataForComparisonOperators();
}

void test_VolumePixelDataIterator::operatorGreaterThanOrEqual_ShouldReturnExpectedValue()
{
    QFETCH(QSharedPointer<VolumePixelDataIterator>, iterator1);
    QFETCH(QSharedPointer<VolumePixelDataIterator>, iterator2);
    QFETCH(bool, greaterThanOrEqual);

    QCOMPARE(*iterator1 >= *iterator2, greaterThanOrEqual);
}

void test_VolumePixelDataIterator::operatorLessThanOrEqual_ShouldReturnExpectedValue_data()
{
    setupTestDataForComparisonOperators();
}

void test_VolumePixelDataIterator::operatorLessThanOrEqual_ShouldReturnExpectedValue()
{
    QFETCH(QSharedPointer<VolumePixelDataIterator>, iterator1);
    QFETCH(QSharedPointer<VolumePixelDataIterator>, iterator2);
    QFETCH(bool, lessThanOrEqual);

    QCOMPARE(*iterator1 <= *iterator2, lessThanOrEqual);
}

void test_VolumePixelDataIterator::operatorAddAndAssign_ShouldAddAndReturnExpectedValue_data()
{
    setupTestDataForAddOperators();
}

void test_VolumePixelDataIterator::operatorAddAndAssign_ShouldAddAndReturnExpectedValue()
{
    QFETCH(QSharedPointer<DataIterator>, dataIterator);
    QFETCH(int, delta);
    QFETCH(unsigned int, expectedVoxelValue);

    VolumePixelDataIterator returned = *dataIterator->iterator += delta;

    QCOMPARE(dataIterator->iterator->get<unsigned int>(), expectedVoxelValue);
    QCOMPARE(returned.get<unsigned int>(), expectedVoxelValue);
}

void test_VolumePixelDataIterator::operatorSubtractAndAssign_ShouldSubtractAndReturnExpectedValue_data()
{
    setupTestDataForSubtractOperators();
}

void test_VolumePixelDataIterator::operatorSubtractAndAssign_ShouldSubtractAndReturnExpectedValue()
{
    QFETCH(QSharedPointer<DataIterator>, dataIterator);
    QFETCH(int, delta);
    QFETCH(unsigned int, expectedVoxelValue);

    VolumePixelDataIterator returned = *dataIterator->iterator -= delta;

    QCOMPARE(dataIterator->iterator->get<unsigned int>(), expectedVoxelValue);
    QCOMPARE(returned.get<unsigned int>(), expectedVoxelValue);
}

QSharedPointer<DataIterator> test_VolumePixelDataIterator::createDataIterator(int x, int y, int z)
{
    unsigned int data[64] =
    {
        791872639, 811525129, 134066898, 541558453,
        365975978, 451505799,  99830523, 680556899,
        453536183, 729275940, 423252438, 933141035,
        792028861, 301263477, 413448016, 141612028,

        298660870, 408141841, 425391003, 819267317,
        770975287, 102760916, 237324047, 749122890,
        654762131, 638808862, 770376181, 457760798,
        234022016, 584932269, 924368040, 244143906,

        111159420, 924042719, 302509952, 479721801,
        567990921, 828006687, 821115067,   2333039,
        626138607, 413986677, 303847715,   1163443,
        470926816, 652404063, 632051296, 764950774,

        571332928, 330461370, 565992699, 606430337,
        994223883, 958556707, 149245908, 939432988,
        275036285, 756408718, 373272869, 112184599,
        163296192, 752516014,  55445813, 360082395
    };

    vtkImageData *imageData = vtkImageData::New();
    imageData->SetExtent(0, 3, 0, 3, 0, 3);
    imageData->AllocateScalars(VTK_UNSIGNED_INT, 1);
    memcpy(imageData->GetScalarPointer(), data, sizeof(data));
    QSharedPointer<DataIterator> dataIterator(new DataIterator());
    dataIterator->data = new VolumePixelData();
    dataIterator->data->setData(imageData);
    dataIterator->iterator = new VolumePixelDataIterator(dataIterator->data, x, y, z);
    imageData->Delete();
    return dataIterator;
}

QSharedPointer<VolumePixelDataIterator> test_VolumePixelDataIterator::createIterator(int x, int y, int z)
{
    // Create a DataIterator and keep only the iterator
    auto dataIterator = createDataIterator(x, y, z);
    QSharedPointer<VolumePixelDataIterator> iterator(dataIterator->iterator);
    dataIterator->iterator = nullptr;
    return iterator;
}

void test_VolumePixelDataIterator::setupTestDataForAddOperators()
{
    QTest::addColumn<QSharedPointer<DataIterator>>("dataIterator");
    QTest::addColumn<int>("delta");
    QTest::addColumn<unsigned int>("expectedVoxelValue");

    QTest::newRow("positive add") << createDataIterator(2, 2, 1) << 14 << 626138607u;
    QTest::newRow("negative add") << createDataIterator(0, 1, 3) << -24 << 234022016u;
    QTest::newRow("zero add") << createDataIterator(2, 3, 3) << 0 << 55445813u;
}

void test_VolumePixelDataIterator::setupTestDataForSubtractOperators()
{
    QTest::addColumn<QSharedPointer<DataIterator>>("dataIterator");
    QTest::addColumn<int>("delta");
    QTest::addColumn<unsigned int>("expectedVoxelValue");

    QTest::newRow("positive subtract") << createDataIterator(2, 0, 1) << 5 << 301263477u;
    QTest::newRow("negative subtract") << createDataIterator(2, 1, 2) << -1 << 2333039u;
    QTest::newRow("zero subtract") << createDataIterator(1, 0, 0) << 0 << 811525129u;
}

void test_VolumePixelDataIterator::setupTestDataForComparisonOperators()
{
    QTest::addColumn< QSharedPointer<VolumePixelDataIterator> >("iterator1");
    QTest::addColumn< QSharedPointer<VolumePixelDataIterator> >("iterator2");
    QTest::addColumn<bool>("equal");
    QTest::addColumn<bool>("different");
    QTest::addColumn<bool>("greaterThan");
    QTest::addColumn<bool>("lessThan");
    QTest::addColumn<bool>("greaterThanOrEqual");
    QTest::addColumn<bool>("lessThanOrEqual");

    VolumePixelData volumePixelData;
    volumePixelData.getVtkData()->AllocateScalars(VTK_CHAR, 1);
    QSharedPointer<VolumePixelDataIterator> iteratorNullChar(new VolumePixelDataIterator(&volumePixelData));
    volumePixelData.getVtkData()->AllocateScalars(VTK_DOUBLE, 1);
    QSharedPointer<VolumePixelDataIterator> iteratorNullDouble(new VolumePixelDataIterator(&volumePixelData));
    volumePixelData.getVtkData()->SetExtent(0, 0, 0, 0, 0, 0);
    volumePixelData.getVtkData()->AllocateScalars(VTK_DOUBLE, 1);
    QSharedPointer<VolumePixelDataIterator> iterator000Double(new VolumePixelDataIterator(&volumePixelData));

    QTest::newRow("same pointer & different scalar type") << iteratorNullChar << iteratorNullDouble << false << true << false << false << false << false;
    QTest::newRow("greater pointer & different scalar type") << iterator000Double << iteratorNullChar << false << true << false << false << false << false;
    QTest::newRow("lesser pointer & different scalar type") << iteratorNullChar << iterator000Double << false << true << false << false << false << false;

    QSharedPointer<VolumePixelDataIterator> iterator122 = createIterator(1, 2, 2);
    QSharedPointer<VolumePixelDataIterator> iterator123(new VolumePixelDataIterator(*iterator122 + 16));

    QTest::newRow("same pointer & same scalar type") << iterator122 << iterator122 << true << false << false << false << true << true;
    QTest::newRow("greater pointer & same scalar type") << iterator123 << iterator122 << false << true << true << false << true << false;
    QTest::newRow("lesser pointer & same scalar type") << iterator122 << iterator123 << false << true << false << true << false << true;
}

DECLARE_TEST(test_VolumePixelDataIterator)

#include "test_volumepixeldataiterator.moc"
