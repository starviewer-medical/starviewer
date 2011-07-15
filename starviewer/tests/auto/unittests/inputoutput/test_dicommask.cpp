#include "autotest.h"

#include "dicommask.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "studytestutils.h"
#include "seriestestutils.h"
#include "imagetestutils.h"

using namespace udg;

class test_DicomMask : public QObject {
Q_OBJECT

private slots:

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

void test_DicomMask::fromStudy_ShouldReturnValidDICOMMask_data()
{
    QTest::addColumn<Study*>("inputStudy");
    QTest::addColumn<DicomMask>("result");

    Study *study = StudyTestUtils::getTestStudy("1", "1") ;

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
}

void test_DicomMask::fromSeries_ShouldReturnValidDICOMMask_data()
{
    QTest::addColumn<Series*>("inputSeries");
    QTest::addColumn<DicomMask>("result");

    Series *series = StudyTestUtils::getTestStudy("1","1", 1)->getSeries().at(0);

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
}

void test_DicomMask::fromSeries_ShouldReturnInvalidDICOMMask_data()
{
    QTest::addColumn<Series*>("inputSeries");

    Series *seriesWithNoInstanceUID = StudyTestUtils::getTestStudy("", "", 1)->getSeries().at(0);
    seriesWithNoInstanceUID->setInstanceUID("");

    Series *seriesWithParentStudyWithoutInstanceUID = StudyTestUtils::getTestStudy("", "", 1)->getSeries().at(0);

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
}

void test_DicomMask::fromImage_ShouldReturnValidDICOMMask_data()
{
    QTest::addColumn<Image*>("inputImage");
    QTest::addColumn<DicomMask>("result");

    Image *image = StudyTestUtils::getTestStudy("1", "1", 1, 1)->getSeries().at(0)->getImageByIndex(0);

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
}

void test_DicomMask::fromImage_ShouldReturnInvalidDICOMMask_data()
{
    QTest::addColumn<Image*>("inputImage");

    Image *imageWithNoInstanceUID = StudyTestUtils::getTestStudy("", "", 1, 1)->getSeries().at(0)->getImageByIndex(0);
    imageWithNoInstanceUID->setSOPInstanceUID("");
    imageWithNoInstanceUID->getParentSeries()->setInstanceUID("");

    Image *imageWithoutSOPInstanceUID = StudyTestUtils::getTestStudy("1", "1", 1, 1)->getSeries().at(0)->getImageByIndex(0);
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
}

DECLARE_TEST(test_DicomMask)

#include "test_dicommask.moc"

