#include "autotest.h"
#include "patientorientation.h"

#include "image.h"
#include "series.h"
#include "study.h"
#include "seriestesthelper.h"
#include "studytesthelper.h"
#include "imagetesthelper.h"
#include "testingmammographyimagehelper.h"

using namespace udg;
using namespace testing;

class test_MammographyImageHelper : public QObject {
Q_OBJECT

private slots:
    void isStandardMammographyImage_ShouldReturnIsStandard_data();
    void isStandardMammographyImage_ShouldReturnIsStandard();
    void isStandardMammographyImage_ShouldReturnIsNotStandard_data();
    void isStandardMammographyImage_ShouldReturnIsNotStandard();

    void getImageOrientationPresentation_ShouldReturnExpectedResult_data();
    void getImageOrientationPresentation_ShouldReturnExpectedResult();

    void getMammographyProjectionLabel_ShouldReturnExpectedResult_data();
    void getMammographyProjectionLabel_ShouldReturnExpectedResult();
};

Q_DECLARE_METATYPE(Image*)
Q_DECLARE_METATYPE(PatientOrientation)


void test_MammographyImageHelper::getMammographyProjectionLabel_ShouldReturnExpectedResult_data()
{
    QTest::addColumn<QString>("codeMeaning");
    QTest::addColumn<QString>("expectedProjectionLabel");

    QTest::newRow("medio-lateral") << "medio-lateral" << "ML";
    QTest::newRow("medio-lateral oblique") << "medio-lateral oblique" << "MLO";
    QTest::newRow("latero-medial") << "latero-medial" << "LM";
    QTest::newRow("latero-medial oblique") << "latero-medial oblique" << "LMO";
    QTest::newRow("cranio-caudal") << "cranio-caudal" << "CC";
    QTest::newRow("caudo-cranial (from below)") << "caudo-cranial (from below)" << "FB";
    QTest::newRow("superolateral to inferomedial oblique") << "superolateral to inferomedial oblique" << "SIO";
    QTest::newRow("exaggerated cranio-caudal") << "exaggerated cranio-caudal" << "XCC";
    QTest::newRow("cranio-caudal exaggerated laterally") << "cranio-caudal exaggerated laterally" << "XCCL";
    QTest::newRow("cranio-caudal exaggerated medially") << "cranio-caudal exaggerated medially" << "XCCM";
}

void test_MammographyImageHelper::getMammographyProjectionLabel_ShouldReturnExpectedResult()
{
    QFETCH(QString, codeMeaning);
    QFETCH(QString, expectedProjectionLabel);

    Image *image = new Image();
    image->setViewCodeMeaning(codeMeaning);

    TestingMammographyImageHelper helper;

    QCOMPARE(helper.getMammographyProjectionLabel(image), expectedProjectionLabel);

    ImageTestHelper::cleanUp(image);

}

void test_MammographyImageHelper::getImageOrientationPresentation_ShouldReturnExpectedResult_data()
{
    QTest::addColumn<QString>("codeMeaning");
    QTest::addColumn<QChar>("laterality");
    QTest::addColumn<PatientOrientation>("desiredOrientation");

    QStringList codeMeaningList;
    codeMeaningList << "cranio-caudal" << "exaggerated cranio-caudal" << "cranio-caudal exaggerated laterally" << "cranio-caudal exaggerated medially" << "caudo-cranial (from below)";

    foreach (QString codeMeaning, codeMeaningList)
    {
        QChar lateralityLeft = PatientOrientation::LeftLabel.at(0);
        PatientOrientation orientationAnteriorRight;
        orientationAnteriorRight.setLabels(PatientOrientation::AnteriorLabel, PatientOrientation::RightLabel);

        QTest::newRow(qPrintable(QString("%1, laterality: %2").arg(codeMeaning).arg(lateralityLeft))) << codeMeaning << lateralityLeft << orientationAnteriorRight;

        QChar lateralityRight = PatientOrientation::RightLabel.at(0);
        PatientOrientation orientationPosteriorLeft;
        orientationPosteriorLeft.setLabels(PatientOrientation::PosteriorLabel, PatientOrientation::LeftLabel);

        QTest::newRow(qPrintable(QString("%1, laterality: %2").arg(codeMeaning).arg(lateralityRight))) << codeMeaning << lateralityRight << orientationPosteriorLeft;
    }

    QStringList codeMeaningList2;
    codeMeaningList2 << "medio-lateral oblique" << "medio-lateral" << "latero-medial" << "latero-medial oblique" << "superolateral to inferomedial oblique";

    foreach (QString codeMeaning, codeMeaningList2)
    {
        QChar lateralityLeft = PatientOrientation::LeftLabel.at(0);
        PatientOrientation orientationAnteriorFeed;
        orientationAnteriorFeed.setLabels(PatientOrientation::AnteriorLabel, PatientOrientation::FeetLabel);

        QTest::newRow(qPrintable(QString("%1, laterality: %2").arg(codeMeaning).arg(lateralityLeft))) << codeMeaning << lateralityLeft << orientationAnteriorFeed;

        QChar lateralityRight = PatientOrientation::RightLabel.at(0);
        PatientOrientation orientationPosteriorFeed;
        orientationPosteriorFeed.setLabels(PatientOrientation::PosteriorLabel, PatientOrientation::FeetLabel);

        QTest::newRow(qPrintable(QString("%1, laterality: %2").arg(codeMeaning).arg(lateralityRight))) << codeMeaning << lateralityRight << orientationPosteriorFeed;
    }
}

void test_MammographyImageHelper::getImageOrientationPresentation_ShouldReturnExpectedResult()
{
    QFETCH(QString, codeMeaning);
    QFETCH(QChar, laterality);
    QFETCH(PatientOrientation, desiredOrientation);

    Image *image = new Image();
    image->setViewCodeMeaning(codeMeaning);
    image->setImageLaterality(laterality);

    TestingMammographyImageHelper helper;

    QCOMPARE(helper.getImageOrientationPresentation(image).getDICOMFormattedPatientOrientation(), desiredOrientation.getDICOMFormattedPatientOrientation());

    ImageTestHelper::cleanUp(image);
}

void test_MammographyImageHelper::isStandardMammographyImage_ShouldReturnIsStandard_data()
{
    QTest::addColumn<Image*>("image");
    QTest::addColumn<QStringList>("exceptionsList");
    Image *imageTest;

    QTest::newRow("image in MG series") << SeriesTestHelper::createSeriesByModality("MG", 1)->getImageByIndex(0) << QStringList();
    imageTest = StudyTestHelper::createStudyWithDescriptionAndSeriesModality("UnusedString", "MG", 1, 1)->getSeries("0")->getImageByIndex(0);
    QTest::newRow("image in MG series in study") << imageTest << QStringList();

    QStringList exceptionsList;
    exceptionsList << "test";
    imageTest = StudyTestHelper::createStudyWithDescriptionAndSeriesModality("non matching", "MG", 1, 1)->getSeries("0")->getImageByIndex(0);
    QTest::newRow("image in MG series in study with exact exception") << imageTest << exceptionsList;

}

void test_MammographyImageHelper::isStandardMammographyImage_ShouldReturnIsStandard()
{
    QFETCH(Image*, image);
    QFETCH(QStringList, exceptionsList);

    TestingMammographyImageHelper helper;
    helper.m_testingMammographyAutoOrientationExceptions = exceptionsList;

    QCOMPARE(helper.isStandardMammographyImage(image), true);

//    ImageTestHelper::cleanUp(image);
}

void test_MammographyImageHelper::isStandardMammographyImage_ShouldReturnIsNotStandard_data()
{
    QTest::addColumn<Image*>("image");
    QTest::addColumn<QStringList>("exceptionsList");

    QTest::newRow("image is null") << (Image*)0 << QStringList();
    QTest::newRow("series is null") << new Image() << QStringList();
    QTest::newRow("image in non MG series") << SeriesTestHelper::createSeriesByModality("no-MG-Modality")->getImageByIndex(0) << QStringList();

    Image *imageTest;
    QStringList exceptionsList;
    exceptionsList << "test";

    imageTest = StudyTestHelper::createStudyWithDescriptionAndSeriesModality("TEST", "MG", 1, 1)->getSeries().at(0)->getImageByIndex(0);
    QTest::newRow("image in MG series in study with exact exception") << imageTest << exceptionsList;

    imageTest = StudyTestHelper::createStudyWithDescriptionAndSeriesModality("somethingTESTsomething", "MG", 1, 1)->getSeries("0")->getImageByIndex(0);
    QTest::newRow("image in MG series in study with exception contains") << imageTest << exceptionsList;

    imageTest = StudyTestHelper::createStudyWithDescriptionAndSeriesModality("TESTinTheBeginning", "MG", 1, 1)->getSeries("0")->getImageByIndex(0);
    QTest::newRow("image in MG series in study with exception contains") << imageTest << exceptionsList;

    imageTest = StudyTestHelper::createStudyWithDescriptionAndSeriesModality("endWithTEST", "MG", 1, 1)->getSeries("0")->getImageByIndex(0);
    QTest::newRow("image in MG series in study with exception contains") << imageTest << exceptionsList;
}

void test_MammographyImageHelper::isStandardMammographyImage_ShouldReturnIsNotStandard()
{
    QFETCH(Image*, image);
    QFETCH(QStringList, exceptionsList);

    TestingMammographyImageHelper helper;
    helper.m_testingMammographyAutoOrientationExceptions = exceptionsList;

    QCOMPARE(helper.isStandardMammographyImage(image), false);

//    ImageTestHelper::cleanUp(image);
}

DECLARE_TEST(test_MammographyImageHelper)

#include "test_mammographyimagehelper.moc"

