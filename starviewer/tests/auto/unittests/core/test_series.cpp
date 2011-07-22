#include "autotest.h"

#include "series.h"
#include "image.h"
#include "dicomsource.h"
#include "seriestesthelper.h"
#include "pacsdevicetesthelper.h"

using namespace udg;
using namespace testing;

class test_Series : public QObject {
Q_OBJECT

private slots:

    ///Test que comprova que es retorni el DICOMSource correctament
    void getDICOMSource_ShouldReturnMergedPACSDeviceList_data();
    void getDICOMSource_ShouldReturnMergedPACSDeviceList();
};

Q_DECLARE_METATYPE(DICOMSource)
Q_DECLARE_METATYPE(Series*)

void test_Series::getDICOMSource_ShouldReturnMergedPACSDeviceList_data()
{
    QTest::addColumn<Series*>("series");
    QTest::addColumn<DICOMSource>("result");

    DICOMSource DICOMSourceWithPACSIDOneAndTwo;
    DICOMSourceWithPACSIDOneAndTwo.addRetrievePACS(PACSDeviceTestHelper::createPACSDeviceByID("1"));
    DICOMSourceWithPACSIDOneAndTwo.addRetrievePACS(PACSDeviceTestHelper::createPACSDeviceByID("2"));

    DICOMSource DICOMSourceWithPACSIDOne;
    DICOMSourceWithPACSIDOne.addRetrievePACS(PACSDeviceTestHelper::createPACSDeviceByID("1"));

    DICOMSource DICOMSourceWithPACSIDTwo;
    DICOMSourceWithPACSIDTwo.addRetrievePACS(PACSDeviceTestHelper::createPACSDeviceByID("2"));

    Series *seriesWithoutDICOMSourceImagesWith = SeriesTestHelper::createSeriesByID("1", 2);
    seriesWithoutDICOMSourceImagesWith->getImageByIndex(0)->setDICOMSource(DICOMSourceWithPACSIDOne);
    seriesWithoutDICOMSourceImagesWith->getImageByIndex(1)->setDICOMSource(DICOMSourceWithPACSIDTwo);

    Series *seriesAndImagesWithDICOMSource = SeriesTestHelper::createSeriesByID("2", 2);
    seriesAndImagesWithDICOMSource->setDICOMSource(DICOMSourceWithPACSIDOne);
    seriesAndImagesWithDICOMSource->getImageByIndex(0)->setDICOMSource(DICOMSourceWithPACSIDOne);
    seriesAndImagesWithDICOMSource->getImageByIndex(1)->setDICOMSource(DICOMSourceWithPACSIDOne);

    Series *seriesWithDICOMSourceAndImagesNot = SeriesTestHelper::createSeriesByID("3", 2);
    seriesWithDICOMSourceAndImagesNot->setDICOMSource(DICOMSourceWithPACSIDTwo);

    Series *seriesAndImageWithOutDICOMSource = SeriesTestHelper::createSeriesByID("4", 1);

    QTest::newRow("Series without DICOMSource and image with DICOMSource") << seriesWithoutDICOMSourceImagesWith << DICOMSourceWithPACSIDOneAndTwo;
    QTest::newRow("Series and Images with DICOMSource") << seriesAndImagesWithDICOMSource  << DICOMSourceWithPACSIDOne;
    QTest::newRow("Series with DICOMSource and image without DICOMSource") << seriesWithDICOMSourceAndImagesNot << DICOMSourceWithPACSIDTwo;
    QTest::newRow("Series and Images without DICOMSource") << seriesAndImageWithOutDICOMSource  << DICOMSource();
}

void test_Series::getDICOMSource_ShouldReturnMergedPACSDeviceList()
{
    QFETCH(Series*, series);
    QFETCH(DICOMSource, result);

    QCOMPARE(series->getDICOMSource() == result, true);

    SeriesTestHelper::cleanUp(series);
}


DECLARE_TEST(test_Series)

#include "test_series.moc"

