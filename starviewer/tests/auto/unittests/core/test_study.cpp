#include "autotest.h"

#include "study.h"
#include "series.h"
#include "dicomsource.h"
#include "studytesthelper.h"
#include "pacsdevicetesthelper.h"


using namespace udg;

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

    DICOMSource DICOMSourceWithPACSIDOneAndTwo;
    DICOMSourceWithPACSIDOneAndTwo.addRetrievePACS(PACSDeviceTestHelper::createPACSDeviceByID("1"));
    DICOMSourceWithPACSIDOneAndTwo.addRetrievePACS(PACSDeviceTestHelper::createPACSDeviceByID("2"));

    DICOMSource DICOMSourceWithPACSIDOne;
    DICOMSourceWithPACSIDOne.addRetrievePACS(PACSDeviceTestHelper::createPACSDeviceByID("1"));

    DICOMSource DICOMSourceWithPACSIDTwo;
    DICOMSourceWithPACSIDTwo.addRetrievePACS(PACSDeviceTestHelper::createPACSDeviceByID("2"));

    Study *studyWithoutDICOMSourceSeriesWith = StudyTestHelper::createStudyByID("1", 2);
    studyWithoutDICOMSourceSeriesWith->getSeries().at(0)->setDICOMSource(DICOMSourceWithPACSIDOne);
    studyWithoutDICOMSourceSeriesWith->getSeries().at(1)->setDICOMSource(DICOMSourceWithPACSIDTwo);

    Study *studyAndSeriesWithDICOMSource = StudyTestHelper::createStudyByID("2", 2);
    studyAndSeriesWithDICOMSource->setDICOMSource(DICOMSourceWithPACSIDOne);
    studyAndSeriesWithDICOMSource->getSeries().at(0)->setDICOMSource(DICOMSourceWithPACSIDOne);
    studyAndSeriesWithDICOMSource->getSeries().at(1)->setDICOMSource(DICOMSourceWithPACSIDOne);

    Study *studyWithDICOMSourceAndSeriesNot = StudyTestHelper::createStudyByID("3", 2);
    studyWithDICOMSourceAndSeriesNot->setDICOMSource(DICOMSourceWithPACSIDTwo);

    Study *studyAndSeriesWithoutDICOMSource = StudyTestHelper::createStudyByID("4", 1);

    QTest::newRow("Study without DICOMSource and series with DICOMSource") << studyWithoutDICOMSourceSeriesWith << DICOMSourceWithPACSIDOneAndTwo;
    QTest::newRow("Study and series with DICOMSource") << studyAndSeriesWithDICOMSource  << DICOMSourceWithPACSIDOne;
    QTest::newRow("Study with DICOMSource and series without DICOMSource") << studyWithDICOMSourceAndSeriesNot << DICOMSourceWithPACSIDTwo;
    QTest::newRow("Study and series without DICOMSource") << studyAndSeriesWithoutDICOMSource  << DICOMSource();

    //TODO: Al fer delete peta! esbrinar perquè
   // delete studyWithoutDICOMSourceSeriesWith;
    //delete studyAndSeriesWithDICOMSource;
    //delete studyWithDICOMSourceAndSeriesNot;
    //delete studyAndSeriesWithoutDICOMSource;
}

void test_Study::getDICOMSource_ShouldReturnMergedPACSDeviceList()
{
    QFETCH(Study*, study);
    QFETCH(DICOMSource, result);

    QCOMPARE(study->getDICOMSource() == result, true);
}


DECLARE_TEST(test_Study)

#include "test_study.moc"
