#include "autotest.h"

#include <QDate>
#include <QTime>

#include "dicommask.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "studytesthelper.h"
#include "seriestesthelper.h"
#include "imagetesthelper.h"

using namespace udg;
using namespace testing;

class test_DicomMask : public QObject {
Q_OBJECT

private slots:

    void getStudyDateRangeAsDICOMFormat_ShouldReturnEmptyQString_data();
    void getStudyDateRangeAsDICOMFormat_ShouldReturnEmptyQString();

    void getStudyDateRangeAsDICOMFormat_ShouldReturnDICOMDateRange_data();
    void getStudyDateRangeAsDICOMFormat_ShouldReturnDICOMDateRange();

    void getStudyTimeRangeAsDICOMFormat_ShouldReturnEmptyQString_data();
    void getStudyTimeRangeAsDICOMFormat_ShouldReturnEmptyQString();

    void getStudyTimeRangeAsDICOMFormat_ShouldReturnDICOMTime_data();
    void getStudyTimeRangeAsDICOMFormat_ShouldReturnDICOMTime();

    void fromStudy_ShouldReturnValidDICOMMask_data();
    void fromStudy_ShouldReturnValidDICOMMask();

    void fromStudy_ShouldReturnInvalidDICOMMask_data();
    void fromStudy_ShouldReturnInvalidDICOMMask();

    void fromSeries_ShouldReturnValidDICOMMask_data();
    void fromSeries_ShouldReturnValidDICOMMask();

    void fromSeries_ShouldReturnInvalidDICOMMask_data();
    void fromSeries_ShouldReturnInvalidDICOMMask();

    void fromImage_ShouldReturnValidDICOMMask_data();
    void fromImage_ShouldReturnValidDICOMMask();

    void fromImage_ShouldReturnInvalidDICOMMask_data();
    void fromImage_ShouldReturnInvalidDICOMMask();
};

Q_DECLARE_METATYPE(DicomMask)
Q_DECLARE_METATYPE(Study*)
Q_DECLARE_METATYPE(Series*)
Q_DECLARE_METATYPE(Image*)

void test_DicomMask::getStudyDateRangeAsDICOMFormat_ShouldReturnEmptyQString_data()
{
    QTest::addColumn<QDate>("minimumDate");
    QTest::addColumn<QDate>("maximumDate");

    QTest::newRow("Minimum and maximum date are empty") << QDate() << QDate();
    QTest::newRow("Invalid minimum and maximum date") << QDate::fromString("99999999", "yyyyMMdd") << QDate::fromString("88888888", "yyyyMMdd");
}

void test_DicomMask::getStudyDateRangeAsDICOMFormat_ShouldReturnEmptyQString()
{
    QFETCH(QDate, minimumDate);
    QFETCH(QDate, maximumDate);

    DicomMask dicomMask;
    dicomMask.setStudyDate(minimumDate, maximumDate);

    QVERIFY(dicomMask.getStudyDateRangeAsDICOMFormat().isEmpty());
}

void test_DicomMask::getStudyDateRangeAsDICOMFormat_ShouldReturnDICOMDateRange_data()
{
    QTest::addColumn<QDate>("minimumDate");
    QTest::addColumn<QDate>("maximumDate");
    QTest::addColumn<QString>("dateRangeAsDICOMFormat");

    QString dateDICOMFormat = "yyyyMMdd";
    QDate today = QDate::currentDate();
    QDate oneYearAgo = QDate::currentDate().addYears(-1);

    //A la pàgina 25 de PS3.5 es pot consultar quin és format del rang de  dates en DICOM
    QTest::newRow("Maximum date empty") << today << QDate() << today.toString(dateDICOMFormat) + "-";
    QTest::newRow("Minimum date empty") << QDate() << today << "-" + today.toString(dateDICOMFormat);
    QTest::newRow("Minimum date smaller than maximum date") << oneYearAgo << today << oneYearAgo.toString(dateDICOMFormat) + "-" + today.toString(dateDICOMFormat);
    QTest::newRow("Minimum date greater than maximum date") << today << oneYearAgo << today.toString(dateDICOMFormat) + "-" + oneYearAgo.toString(dateDICOMFormat);
    QTest::newRow("Minimum and maximum with same date") << today << today << today.toString(dateDICOMFormat);
}

void test_DicomMask::getStudyDateRangeAsDICOMFormat_ShouldReturnDICOMDateRange()
{
    QFETCH(QDate, minimumDate);
    QFETCH(QDate, maximumDate);
    QFETCH(QString, dateRangeAsDICOMFormat);

    DicomMask dicomMask;
    dicomMask.setStudyDate(minimumDate, maximumDate);

    QCOMPARE(dicomMask.getStudyDateRangeAsDICOMFormat(), dateRangeAsDICOMFormat);
}

void test_DicomMask::getStudyTimeRangeAsDICOMFormat_ShouldReturnEmptyQString_data()
{
    QTest::addColumn<QTime>("minimumTime");
    QTest::addColumn<QTime>("maximumTime");

    QTest::newRow("Minimum and maximum time empty") << QTime() << QTime();
    QTest::newRow("Invalid minimum and maximum time") << QTime::fromString("99:99:99", "HHmmss") << QTime::fromString("99:99:99", "HHmmss");
}

void test_DicomMask::getStudyTimeRangeAsDICOMFormat_ShouldReturnEmptyQString()
{
    QFETCH(QTime, minimumTime);
    QFETCH(QTime, maximumTime);

    DicomMask dicomMask;
    dicomMask.setStudyTime(minimumTime, maximumTime);

    QVERIFY(dicomMask.getStudyTimeRangeAsDICOMFormat().isEmpty());
}

void test_DicomMask::getStudyTimeRangeAsDICOMFormat_ShouldReturnDICOMTime_data()
{
    QTest::addColumn<QTime>("minimumTime");
    QTest::addColumn<QTime>("maximumTime");
    QTest::addColumn<QString>("timeRangeAsDICOMFormat");

    QString timeDICOMFormat = "HHmmss";
    QTime now = QTime::currentTime();
    QTime oneHoureAgo = QTime::currentTime().addSecs(-3600);

    //A la pàgina 31 de PS3.5 es pot consultar quin és format del rang de  dates en DICOM
    QTest::newRow("Maximum time empty") << now << QTime() << now.toString(timeDICOMFormat) + "-";
    QTest::newRow("Minimum time empty") << QTime() << now << "-" + now.toString(timeDICOMFormat);
    QTest::newRow("Minimum time smaller than maximum time") << oneHoureAgo << now << oneHoureAgo.toString(timeDICOMFormat) + "-" + now.toString(timeDICOMFormat);
    QTest::newRow("Minimum and maximum with same time") << now << now << now.toString(timeDICOMFormat);
    QTest::newRow("Minimum time greater than maximum time") << now << oneHoureAgo << now.toString(timeDICOMFormat) + "-" + oneHoureAgo.toString(timeDICOMFormat);
}

void test_DicomMask::getStudyTimeRangeAsDICOMFormat_ShouldReturnDICOMTime()
{
    QFETCH(QTime, minimumTime);
    QFETCH(QTime, maximumTime);
    QFETCH(QString, timeRangeAsDICOMFormat);

    DicomMask dicomMask;
    dicomMask.setStudyTime(minimumTime, maximumTime);

    QCOMPARE(dicomMask.getStudyTimeRangeAsDICOMFormat(), timeRangeAsDICOMFormat);
}

void test_DicomMask::fromStudy_ShouldReturnValidDICOMMask_data()
{
    QTest::addColumn<Study*>("inputStudy");
    QTest::addColumn<DicomMask>("result");

    Study *study = StudyTestHelper::createStudyByUID("1") ;

    DicomMask studyDicomMask;
    studyDicomMask.setStudyInstanceUID(study->getInstanceUID());

    QTest::newRow("Create DICOMMask from valid Study with InstanceUID") << study << studyDicomMask;
}

void test_DicomMask::fromStudy_ShouldReturnValidDICOMMask()
{
    QFETCH(Study*, inputStudy);
    QFETCH(DicomMask, result);

    bool ok;

    QCOMPARE(DicomMask::fromStudy(inputStudy, ok) == result, true);
    QCOMPARE(ok, true);

    StudyTestHelper::cleanUp(inputStudy);
}

void test_DicomMask::fromStudy_ShouldReturnInvalidDICOMMask_data()
{
    QTest::addColumn<Study*>("inputStudy");

    QTest::newRow("Create DICOMMask from invalid Study with InstanceUID Empty") << new Study();
}

void test_DicomMask::fromStudy_ShouldReturnInvalidDICOMMask()
{
    QFETCH(Study*, inputStudy);

    bool ok;
    DicomMask::fromStudy(inputStudy, ok);

    QCOMPARE(ok, false);

    StudyTestHelper::cleanUp(inputStudy);
}

void test_DicomMask::fromSeries_ShouldReturnValidDICOMMask_data()
{
    QTest::addColumn<Series*>("inputSeries");
    QTest::addColumn<DicomMask>("result");

    Series *series = StudyTestHelper::createStudyByUID("1", 1)->getSeries().at(0);

    DicomMask seriesDicomMask;
    seriesDicomMask.setStudyInstanceUID(series->getParentStudy()->getInstanceUID());
    seriesDicomMask.setSeriesInstanceUID(series->getInstanceUID());

    QTest::newRow("Create DICOMMask from Series with InstanceUID with Parent Study with InstanceUID") << series << seriesDicomMask;
}

void test_DicomMask::fromSeries_ShouldReturnValidDICOMMask()
{
    QFETCH(Series*, inputSeries);
    QFETCH(DicomMask, result);

    bool ok;

    QCOMPARE(DicomMask::fromSeries(inputSeries, ok) == result, true);
    QCOMPARE(ok, true);

    SeriesTestHelper::cleanUp(inputSeries);
}

void test_DicomMask::fromSeries_ShouldReturnInvalidDICOMMask_data()
{
    QTest::addColumn<Series*>("inputSeries");

    Series *seriesWithNoInstanceUID = StudyTestHelper::createStudyByUID("", 1)->getSeries().at(0);
    seriesWithNoInstanceUID->setInstanceUID("");

    Series *seriesWithParentStudyWithoutInstanceUID = StudyTestHelper::createStudyByUID("", 1)->getSeries().at(0);

    QTest::newRow("Create DICOMMask from invalid Series with no Study parent") << new Series();
    QTest::newRow("Create DICOMMask from invalid Series without InstanceUID and with Study parent without InstanceUID") << seriesWithNoInstanceUID;
    QTest::newRow("Create DICOMMask from invalid Series with InstanceUID and with Study parent without InstanceUID") << seriesWithParentStudyWithoutInstanceUID;
}

void test_DicomMask::fromSeries_ShouldReturnInvalidDICOMMask()
{
    QFETCH(Series*, inputSeries);

    bool ok;
    DicomMask::fromSeries(inputSeries, ok);

    QCOMPARE(ok, false);

    SeriesTestHelper::cleanUp(inputSeries);
}

void test_DicomMask::fromImage_ShouldReturnValidDICOMMask_data()
{
    QTest::addColumn<Image*>("inputImage");
    QTest::addColumn<DicomMask>("result");

    Image *image = StudyTestHelper::createStudyByUID("1", 1, 1)->getSeries().at(0)->getImageByIndex(0);

    DicomMask imageDicomMask;
    imageDicomMask.setStudyInstanceUID(image->getParentSeries()->getParentStudy()->getInstanceUID());
    imageDicomMask.setSeriesInstanceUID(image->getParentSeries()->getInstanceUID());
    imageDicomMask.setSOPInstanceUID(image->getSOPInstanceUID());

    QTest::newRow("Create DICOMMask from Image with SOPInstanceUID and parent study and series with InstanceUID") << image << imageDicomMask;
}

void test_DicomMask::fromImage_ShouldReturnValidDICOMMask()
{
    QFETCH(Image*, inputImage);
    QFETCH(DicomMask, result);

    bool ok;

    QCOMPARE(DicomMask::fromImage(inputImage, ok) == result, true);
    QCOMPARE(ok, true);

    ImageTestHelper::cleanUp(inputImage);
}

void test_DicomMask::fromImage_ShouldReturnInvalidDICOMMask_data()
{
    QTest::addColumn<Image*>("inputImage");

    Image *imageWithNoInstanceUID = StudyTestHelper::createStudyByUID("", 1, 1)->getSeries().at(0)->getImageByIndex(0);
    imageWithNoInstanceUID->setSOPInstanceUID("");
    imageWithNoInstanceUID->getParentSeries()->setInstanceUID("");

    Image *imageWithoutSOPInstanceUID = StudyTestHelper::createStudyByUID("1", 1, 1)->getSeries().at(0)->getImageByIndex(0);
    imageWithoutSOPInstanceUID->setSOPInstanceUID("");

    QTest::newRow("Create DICOMMask from invalid Image with no Series parent") << new Image();
    QTest::newRow("Create DICOMMask from invalid Image without SOPInstanceUID and study and series parent without InstanceUID") << imageWithNoInstanceUID;
    QTest::newRow("Create DICOMMask from invalid Image without SOPInstanceUID") << imageWithoutSOPInstanceUID;
}

void test_DicomMask::fromImage_ShouldReturnInvalidDICOMMask()
{
    QFETCH(Image*, inputImage);

    bool ok;
    DicomMask::fromImage(inputImage, ok);

    QCOMPARE(ok, false);

    ImageTestHelper::cleanUp(inputImage);
}

DECLARE_TEST(test_DicomMask)

#include "test_dicommask.moc"

