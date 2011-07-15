#include "autotest.h"

#include "series.h"
#include "image.h"
#include "dicomsource.h"
#include "seriestestutils.h"
#include "pacsdevicetestutils.h"

using namespace udg;

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
    DICOMSourceWithPACSIDOneAndTwo.addRetrievePACS(PACSDeviceTestUtils::getTestPACSDevice("1"));
    DICOMSourceWithPACSIDOneAndTwo.addRetrievePACS(PACSDeviceTestUtils::getTestPACSDevice("2"));

    DICOMSource DICOMSourceWithPACSIDOne;
    DICOMSourceWithPACSIDOne.addRetrievePACS(PACSDeviceTestUtils::getTestPACSDevice("1"));

    DICOMSource DICOMSourceWithPACSIDTwo;
    DICOMSourceWithPACSIDTwo.addRetrievePACS(PACSDeviceTestUtils::getTestPACSDevice("2"));

    Series *seriesWithoutDICOMSourceImagesWith = SeriesTestUtils::getTestSeries("1", "1", 2);
    seriesWithoutDICOMSourceImagesWith->getImageByIndex(0)->setDICOMSource(DICOMSourceWithPACSIDOne);
    seriesWithoutDICOMSourceImagesWith->getImageByIndex(1)->setDICOMSource(DICOMSourceWithPACSIDTwo);

    Series *seriesAndImagesWithDICOMSource = SeriesTestUtils::getTestSeries("2", "2", 2);
    seriesAndImagesWithDICOMSource->setDICOMSource(DICOMSourceWithPACSIDOne);
    seriesAndImagesWithDICOMSource->getImageByIndex(0)->setDICOMSource(DICOMSourceWithPACSIDOne);
    seriesAndImagesWithDICOMSource->getImageByIndex(1)->setDICOMSource(DICOMSourceWithPACSIDOne);

    Series *seriesWithDICOMSourceAndImagesNot = SeriesTestUtils::getTestSeries("3", "3", 2);
    seriesWithDICOMSourceAndImagesNot->setDICOMSource(DICOMSourceWithPACSIDTwo);

    Series *seriesAndImageWithOutDICOMSource = SeriesTestUtils::getTestSeries("4", "4", 1);

    QTest::newRow("Series without DICOMSource and image with DICOMSource") << seriesWithoutDICOMSourceImagesWith << DICOMSourceWithPACSIDOneAndTwo;
    QTest::newRow("Series and Images with DICOMSource") << seriesAndImagesWithDICOMSource  << DICOMSourceWithPACSIDOne;
    QTest::newRow("Series with DICOMSource and image without DICOMSource") << seriesWithDICOMSourceAndImagesNot << DICOMSourceWithPACSIDTwo;
    QTest::newRow("Series and Images without DICOMSource") << seriesAndImageWithOutDICOMSource  << DICOMSource();

    //TODO: Al fer delete peta! esbrinar perquè
    //delete seriesNoDICOMSourceImagesWith;
    //delete seriesAndImagesWithDICOMSource;
    //delete seriesWithDICOMSourceAndImagesNot;
    //delete seriesAndImageWithOutDICOMSource;
}

void test_Series::getDICOMSource_ShouldReturnMergedPACSDeviceList()
{
    QFETCH(Series*, series);
    QFETCH(DICOMSource, result);

    QCOMPARE(series->getDICOMSource() == result, true);
}


DECLARE_TEST(test_Series)

#include "test_series.moc"

