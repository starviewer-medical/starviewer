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

    DICOMSource DICOMSourceWithPACSIDOne;
    DICOMSourceWithPACSIDOne.addRetrievePACS(PACSDeviceTestHelper::createPACSDeviceByID("1"));

    DICOMSource DICOMSourceWithPACSIDTwo;
    DICOMSourceWithPACSIDTwo.addRetrievePACS(PACSDeviceTestHelper::createPACSDeviceByID("2"));

    DICOMSource DICOMSourceResults;
    DICOMSourceResults.addPACSDeviceFromDICOMSource(DICOMSourceWithPACSIDOne);
    DICOMSourceResults.addPACSDeviceFromDICOMSource(DICOMSourceWithPACSIDTwo);

    Series *series = SeriesTestHelper::createSeries(2);
    series->setDICOMSource(DICOMSourceWithPACSIDOne);
    series->getImageByIndex(0)->setDICOMSource(DICOMSourceWithPACSIDOne);
    series->getImageByIndex(1)->setDICOMSource(DICOMSourceWithPACSIDTwo);

    QTest::newRow("Series and Images with DICOMSource") << series  << DICOMSourceResults;
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

