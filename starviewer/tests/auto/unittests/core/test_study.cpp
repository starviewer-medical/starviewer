#include "autotest.h"

#include "study.h"
#include "series.h"
#include "image.h"
#include "dicomsource.h"
#include "studytesthelper.h"
#include "pacsdevicetesthelper.h"

using namespace udg;
using namespace testing;

class test_Study : public QObject {
Q_OBJECT

private slots:

    ///Test que comprova que es retorni el DICOMSource correctament
    void getDICOMSource_ShouldReturnMergedPACSDeviceList_data();
    void getDICOMSource_ShouldReturnMergedPACSDeviceList();
};

Q_DECLARE_METATYPE(DICOMSource)
Q_DECLARE_METATYPE(Study*)

void test_Study::getDICOMSource_ShouldReturnMergedPACSDeviceList_data()
{
    QTest::addColumn<Study*>("study");
    QTest::addColumn<DICOMSource>("result");

    DICOMSource DICOMSourceWithPACSIDOne;
    DICOMSourceWithPACSIDOne.addRetrievePACS(PACSDeviceTestHelper::createPACSDeviceByID("1"));
    DICOMSource DICOMSourceWithPACSIDTwo;
    DICOMSourceWithPACSIDTwo.addRetrievePACS(PACSDeviceTestHelper::createPACSDeviceByID("2"));
    DICOMSource DICOMSourceWithPACSIDThree;
    DICOMSourceWithPACSIDTwo.addRetrievePACS(PACSDeviceTestHelper::createPACSDeviceByID("3"));

    DICOMSource DICOMSourceResult;
    DICOMSourceResult.addPACSDeviceFromDICOMSource(DICOMSourceWithPACSIDOne);
    DICOMSourceResult.addPACSDeviceFromDICOMSource(DICOMSourceWithPACSIDTwo);
    DICOMSourceResult.addPACSDeviceFromDICOMSource(DICOMSourceWithPACSIDThree);

    Study *study = StudyTestHelper::createStudy(2, 1);
    study->setDICOMSource(DICOMSourceWithPACSIDOne);
    study->getSeries().at(0)->setDICOMSource(DICOMSourceWithPACSIDOne);
    study->getSeries().at(0)->getImageByIndex(0)->setDICOMSource(DICOMSourceWithPACSIDThree);
    study->getSeries().at(1)->setDICOMSource(DICOMSourceWithPACSIDTwo);
    study->getSeries().at(1)->getImageByIndex(0)->setDICOMSource(DICOMSourceWithPACSIDTwo);

    QTest::newRow("Study/series/images with DICOMSource") << study  << DICOMSourceResult;
}

void test_Study::getDICOMSource_ShouldReturnMergedPACSDeviceList()
{
    QFETCH(Study*, study);
    QFETCH(DICOMSource, result);

    QCOMPARE(study->getDICOMSource() == result, true);

    StudyTestHelper::cleanUp(study);
}


DECLARE_TEST(test_Study)

#include "test_study.moc"
