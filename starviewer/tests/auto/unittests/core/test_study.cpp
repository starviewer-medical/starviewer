#include "autotest.h"

#include "study.h"
#include "series.h"
#include "image.h"
#include "dicomsource.h"
#include "studytesthelper.h"
#include "seriestesthelper.h"
#include "pacsdevicetesthelper.h"

using namespace udg;
using namespace testing;

class test_Study : public QObject {
Q_OBJECT

private slots:

    /// Test que comprova que es retorni el DICOMSource correctament
    void getDICOMSource_ShouldReturnMergedPACSDeviceList_data();
    void getDICOMSource_ShouldReturnMergedPACSDeviceList();

    /// Test que comprova si la data introduida és vàlida o no
    void setDate_ShouldValidatePassedDate_data();
    void setDate_ShouldValidatePassedDate();

    /// Test que comprova si la data passada com a String és vàlida o no
    void setDate_ShouldValidatePassedDateAsString_data();
    void setDate_ShouldValidatePassedDateAsString();

    /// Test que comprova si el temps introduit és vàlid o no
    void setTime_ShouldValidatePassedTime_data();
    void setTime_ShouldValidatePassedTime();

    /// Test que comprova si el temps introduit com a String és vàlid o no
    void setTime_ShouldValidatePassedTimeAsString_data();
    void setTime_ShouldValidatePassedTimeAsString();

    /// Test que comprova si existeix una serie
    void seriesExists_ShouldReturnCorrectAnswer_data();
    void seriesExists_ShouldReturnCorrectAnswer();

    /// Test que comprova si s'afegeix una serie correctament
    void addSeries_ShouldReturnCorrectAnswer_data();
    void addSeries_ShouldReturnCorrectAnswer();

    /// Test que comprova si retorna la llista de series visibles correctament
    void getViewableSeries_ShouldReturnCorrectList_data();
    void getViewableSeries_ShouldReturnCorrectList();

    /// Test que comprova si retorna la llista de series seleccionades correctament
    void getSelectedSeries_ShouldReturnCorrectList_data();
    void getSelectedSeries_ShouldReturnCorrectList();

    /// Test que comprova si retorna la llista de series correctament
    void getSeries_ShouldReturnCorrectSeriesPassedByUID_data();
    void getSeries_ShouldReturnCorrectSeriesPassedByUID();
};

Q_DECLARE_METATYPE(DICOMSource)
Q_DECLARE_METATYPE(Study*)
Q_DECLARE_METATYPE(Series*)
Q_DECLARE_METATYPE(QList<Series*>)

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

void test_Study::setDate_ShouldValidatePassedDate_data()
{
    QTest::addColumn<QDate>("date");
    QTest::addColumn<bool>("result");

    // Una data nula es una data invalida segons Qt
    QDate emptyDate;
    QDate invalidDate(1000, 24, 32);
    QDate validPastDate(2011, 1, 1);
    QDate validFutureDate(2012, 12, 24);

    QTest::newRow("set a null Date") << emptyDate << true;
    QTest::newRow("set a invalid Date") << invalidDate << true;
    QTest::newRow("set a valid past Date") << validPastDate << true;
    QTest::newRow("set a valid future Date") << validFutureDate << true;
}

void test_Study::setDate_ShouldValidatePassedDate()
{
    QFETCH(QDate, date);
    QFETCH(bool, result);
    Study study;

    QCOMPARE(study.setDate(date), result);
}

void test_Study::setDate_ShouldValidatePassedDateAsString_data()
{
    QTest::addColumn<QString>("date");
    QTest::addColumn<bool>("result");

    // Una data nula es una data invalida segons Qt
    QString emptyDate;
    QString invalidFormatDate("trolling.Date");
    QString invalidDate("20120032");
    QString validPointDate("2012.01.31");
    QString validDate("20120131");

    QTest::newRow("set a null Date") << emptyDate << true;
    QTest::newRow("set a invalid Date") << invalidFormatDate << true;
    QTest::newRow("set a invalid Date") << invalidDate << true;
    QTest::newRow("set a valid Date with points") << validPointDate << true;
    QTest::newRow("set a valid Date without points") << validDate << true;
}

void test_Study::setDate_ShouldValidatePassedDateAsString()
{
    QFETCH(QString, date);
    QFETCH(bool, result);
    Study study;

    QCOMPARE(study.setDate(date), result);
}

void test_Study::setTime_ShouldValidatePassedTime_data()
{
    QTest::addColumn<QTime>("time");
    QTest::addColumn<bool>("result");

    // Un time nul es un time invalid segons Qt
    QTime emptyTime;
    QTime invalidTime(24, 61, 61, 1000);
    QTime validPastTime(0, 0, 0, 0);
    QTime validFutureTime(12, 30, 30, 500);

    Series series;
    QTest::newRow("set a null Time") << emptyTime << true;
    QTest::newRow("set a invalid Time") << invalidTime << true;
    QTest::newRow("set a valid past Time") << validPastTime << true;
    QTest::newRow("set a valid future Time") << validFutureTime << true;
}

void test_Study::setTime_ShouldValidatePassedTime()
{
    QFETCH(QTime, time);
    QFETCH(bool, result);
    Study study;

    QCOMPARE(study.setTime(time), result);
}

void test_Study::setTime_ShouldValidatePassedTimeAsString_data()
{
    QTest::addColumn<QString>("time");
    QTest::addColumn<bool>("result");

    // Un time nul es un time invalid segons Qt
    QString emptyTime;
    QString invalidPointsTime("22:5:62.1000");
    QString invalidTime("220562");
    QString validPointsTime("21:10:30.509");
    QString validTime("211030");

    QTest::newRow("set a null Time") << emptyTime << true;
    QTest::newRow("set a invalid Time with :") << invalidPointsTime << true;
    QTest::newRow("set a invalid Time without :") << invalidTime << true;
    QTest::newRow("set a valid past Time with :") << validPointsTime << true;
    QTest::newRow("set a valid future Time without :") << validTime << true;
}

void test_Study::setTime_ShouldValidatePassedTimeAsString()
{
    QFETCH(QString, time);
    QFETCH(bool, result);
    Study study;

    QCOMPARE(study.setTime(time), result);
}

void test_Study::seriesExists_ShouldReturnCorrectAnswer_data()
{
    QTest::addColumn<Study*>("study");
    QTest::addColumn<QString>("identifier");
    QTest::addColumn<bool>("result");

    Study *emptyStudy = StudyTestHelper::createStudyByUID("empty", 0, 0);
    QTest::newRow("Correct identifier in a empty Study") << emptyStudy << "0" << false;

    Study *oneSeriesStudy1 = StudyTestHelper::createStudyByUID("one1", 1, 1);
    QTest::newRow("Unexisting identifier in a one Series Study") << oneSeriesStudy1 << "1" << false;

    Study *oneSeriesStudy2 = StudyTestHelper::createStudyByUID("one2", 1, 1);
    QTest::newRow("Existing identifier in a one Series Study") << oneSeriesStudy2 << "0" << true;

    Study *manySeriesStudy1 = StudyTestHelper::createStudyByUID("many1", 10);
    QTest::newRow("Unexisting identifier in a many Series Study") << manySeriesStudy1 << "10" << false;

    Study *manySeriesStudy2 = StudyTestHelper::createStudyByUID("many2", 10);
    QTest::newRow("Existing identifier in a many Series Study") << manySeriesStudy2 << "9" << true;
}

void test_Study::seriesExists_ShouldReturnCorrectAnswer()
{
    QFETCH(Study*, study);
    QFETCH(QString, identifier);
    QFETCH(bool, result);

    QCOMPARE(study->seriesExists(identifier), result);

    StudyTestHelper::cleanUp(study);
}

void test_Study::addSeries_ShouldReturnCorrectAnswer_data()
{
    QTest::addColumn<Study*>("study");
    QTest::addColumn<Series*>("series");
    QTest::addColumn<bool>("result");

    Study *emptyStudy = StudyTestHelper::createStudy(0);
    Series *emptySeries = SeriesTestHelper::createSeriesByUID("", 0);

    Study *studyWithOneSeries = StudyTestHelper::createStudy(1);
    Series *testSeries = SeriesTestHelper::createSeriesByUID("0", 0);

    Study *anotherStudyWithOneSeries = StudyTestHelper::createStudy(1); 
    Series *testSeries2 = SeriesTestHelper::createSeriesByUID("1", 0);

    QTest::newRow("Adding an Empty Series into empty Study") << emptyStudy << emptySeries  << false;
    QTest::newRow("Adding an existing Series into non empty Study") << studyWithOneSeries << testSeries  << false;
    QTest::newRow("Adding a new Series") << anotherStudyWithOneSeries << testSeries2  << true;
}

void test_Study::addSeries_ShouldReturnCorrectAnswer()
{
    QFETCH(Study*, study);
    QFETCH(Series*, series);
    QFETCH(bool, result);

    QCOMPARE(study->addSeries(series), result);
    // No li mola eliminar les images!!!!!!!!
    //ImageTestHelper::cleanUp(images);
    StudyTestHelper::cleanUp(study);
}

void test_Study::getViewableSeries_ShouldReturnCorrectList_data()
{
    QTest::addColumn<int>("listOfSeries");
    QTest::addColumn<int>("result");

    Study *fourSeriesStudy = StudyTestHelper::createStudy(0);
    // PR amb imatge
    Series *fullPRSeries = SeriesTestHelper::createSeries(2);
    fullPRSeries->setModality("PR");
    fourSeriesStudy->addSeries(fullPRSeries);

    // KO amb imatge
    Series *fullKOSeries = SeriesTestHelper::createSeries(2);
    fullKOSeries->setModality("KO");
    fourSeriesStudy->addSeries(fullKOSeries);

    // SR amb imatge
    Series *fullSRSeries = SeriesTestHelper::createSeries(2);
    fullSRSeries->setModality("SR");
    fourSeriesStudy->addSeries(fullSRSeries);

    QTest::newRow("Number of series") << fourSeriesStudy->getViewableSeries().size() << 0;

    Study *fourViewableSeriesStudy = StudyTestHelper::createStudy(0);

    // CT sense imatge
    Series *fullCTSeries = SeriesTestHelper::createSeriesByUID("CT", 2);
    fullCTSeries->setModality("CT");
    fourViewableSeriesStudy->addSeries(fullCTSeries);

    // XA sense imatge
    Series *fullXASeries = SeriesTestHelper::createSeriesByUID("XA", 2);
    fullXASeries->setModality("XA");
    fourViewableSeriesStudy->addSeries(fullXASeries);

    // CR sense imatge
    Series *fullCRSeries = SeriesTestHelper::createSeriesByUID("CR", 2);
    fullCRSeries->setModality("CR");
    fourViewableSeriesStudy->addSeries(fullCRSeries);

    // VALIDA sense Imatge
    Series *fullMRSeries = SeriesTestHelper::createSeriesByUID("MR", 2);
    fullMRSeries->setModality("MR");
    fourViewableSeriesStudy->addSeries(fullMRSeries);

    QTest::newRow("Number of viewable series in a study") << fourViewableSeriesStudy->getViewableSeries().size() << 4;
}

void test_Study::getViewableSeries_ShouldReturnCorrectList()
{
    QFETCH(int, listOfSeries);
    QFETCH(int, result);

    QCOMPARE(listOfSeries, result);
}

void test_Study::getSelectedSeries_ShouldReturnCorrectList_data()
{
    QTest::addColumn<Study*>("study");
    QTest::addColumn<QList<Series*> >("result");
   
    Study *seriesStudy = StudyTestHelper::createStudy(0);
    QList<Series*> selectedSeriesList;

    Series *selectedSeries1 = SeriesTestHelper::createSeriesByUID("0", 2);
    selectedSeries1->setSelectStatus(true);
    seriesStudy->addSeries(selectedSeries1);

    Series *selectedSeries2 = SeriesTestHelper::createSeriesByUID("1", 2);
    selectedSeries2->setSelectStatus(true);
    seriesStudy->addSeries(selectedSeries2);

    Series *selectedSeries3 = SeriesTestHelper::createSeriesByUID("2", 2);
    selectedSeries3->setSelectStatus(true);
    seriesStudy->addSeries(selectedSeries3);

    selectedSeriesList.append(selectedSeries3);
    selectedSeriesList.append(selectedSeries2);
    selectedSeriesList.append(selectedSeries1);

    QTest::newRow("Number of selected series in a study with selected Series") << seriesStudy << selectedSeriesList;

    Study *notSelectedSeriesStudy = StudyTestHelper::createStudy(0);
    QList<Series*> notSelectedSeriesList;

    Series *notSelectedSeries1 = SeriesTestHelper::createSeriesByUID("0", 2);
    notSelectedSeries1->setSelectStatus(false);
    notSelectedSeriesStudy->addSeries(notSelectedSeries1);

    Series *notSelectedSeries2 = SeriesTestHelper::createSeriesByUID("1", 2);
    notSelectedSeries2->setSelectStatus(false);
    notSelectedSeriesStudy->addSeries(notSelectedSeries2);

    Series *notSelectedSeries3 = SeriesTestHelper::createSeriesByUID("2", 2);
    notSelectedSeries3->setSelectStatus(false);
    notSelectedSeriesStudy->addSeries(notSelectedSeries3);

    QTest::newRow("Number of selected series in a study with not selected Series") << notSelectedSeriesStudy << notSelectedSeriesList;
}

void test_Study::getSelectedSeries_ShouldReturnCorrectList()
{
    QFETCH(Study*, study);
    QFETCH(QList<Series*>, result);

    QCOMPARE(study->getSelectedSeries(), result);
    StudyTestHelper::cleanUp(study);
}

void test_Study::getSeries_ShouldReturnCorrectSeriesPassedByUID_data()
{
    QTest::addColumn<Study*>("study");
    QTest::addColumn<Series*>("series");
    QTest::addColumn<bool>("result");

    Study *seriesStudy = StudyTestHelper::createStudy(0);
    Series *fullTestSeries = SeriesTestHelper::createSeriesByUID("0", 2);
    seriesStudy->addSeries(fullTestSeries);

    QTest::newRow("Existinng UID returns de right series of only one series?") << seriesStudy << fullTestSeries << true;

    Study *fullStudy = StudyTestHelper::createStudy(4);
    Series *fullTestSeriesByUID = SeriesTestHelper::createSeriesByUID("testing", 2);
    fullStudy->addSeries(fullTestSeriesByUID);

    QTest::newRow("Existinng UID returns de right series from full of series study?") << fullStudy << fullTestSeriesByUID << true;

    Study *fullStudyWithoutDesiredSerie = StudyTestHelper::createStudy(4);
    Series *notIncludedSeries = SeriesTestHelper::createSeriesByUID("notIncluded", 2);

    QTest::newRow("NotExisting UID didn't return any series") << fullStudyWithoutDesiredSerie << notIncludedSeries << false;
}

void test_Study::getSeries_ShouldReturnCorrectSeriesPassedByUID()
{
    QFETCH(Study*, study);
    QFETCH(Series*, series);
    QFETCH(bool, result);

    QCOMPARE(study->getSeries(series->getInstanceUID()) == series, result);
}

DECLARE_TEST(test_Study)

#include "test_study.moc"
