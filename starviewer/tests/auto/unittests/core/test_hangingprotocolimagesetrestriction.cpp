#include "autotest.h"
#include "hangingprotocolimagesetrestriction.h"

#include "image.h"
#include "patient.h"
#include "series.h"
#include "study.h"
#include "volume.h"

using namespace udg;

class test_HangingProtocolImageSetRestriction : public QObject {
    Q_OBJECT

private slots:
    void test_Series_ShouldReturnExpectedValue_data();
    void test_Series_ShouldReturnExpectedValue();

    void test_Image_ShouldReturnExpectedValue_data();
    void test_Image_ShouldReturnExpectedValue();

};

Q_DECLARE_METATYPE(HangingProtocolImageSetRestriction)

void test_HangingProtocolImageSetRestriction::test_Series_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<HangingProtocolImageSetRestriction>("restriction");
    QTest::addColumn<Series*>("series");
    QTest::addColumn<bool>("expectedValue");

    Patient *patient = new Patient(this);
    patient->setFullName("Smith^John");

    Study *study = new Study();
    study->setParentPatient(patient);
    study->setDescription("this is a Study");

    Series *series = new Series();
    series->setParentStudy(study);
    series->setBodyPartExamined("CHEST");
    series->setProtocolName("this is protocol1");
    series->setViewPosition("LATERAL");
    series->setDescription("A Fancy Description");
    series->setSeriesNumber("1");

    Image *image = new Image();
    image->setParentSeries(series);

    Volume *volume = new Volume(this);
    volume->addImage(image);
    series->addVolume(volume);

    QTest::newRow("empty selector attribute") << HangingProtocolImageSetRestriction() << series << true;
    QTest::newRow("unsupported selector attribute") << HangingProtocolImageSetRestriction(0, "foo", "", 0) << series << true;
    QTest::newRow("body part examined, match") << HangingProtocolImageSetRestriction(0, "BodyPartExamined", "CHEST", 0) << series << true;
    QTest::newRow("body part examined, no match") << HangingProtocolImageSetRestriction(0, "BodyPartExamined", "HEAD", 0) << series << false;
    QTest::newRow("protocol name, match") << HangingProtocolImageSetRestriction(0, "ProtocolName", "protocol[0-9]", 0) << series << true;
    QTest::newRow("protocol name, no match") << HangingProtocolImageSetRestriction(0, "ProtocolName", "bar", 0) << series << false;
    QTest::newRow("view position, match") << HangingProtocolImageSetRestriction(0, "ViewPosition", "LATERAL", 0) << series << true;
    QTest::newRow("view position, no match") << HangingProtocolImageSetRestriction(0, "ViewPosition", "AP", 0) << series << false;
    QTest::newRow("series description, match") << HangingProtocolImageSetRestriction(0, "SeriesDescription", "a.* desc", 0) << series << true;
    QTest::newRow("series description, no match") << HangingProtocolImageSetRestriction(0, "SeriesDescription", "^[^a-z ]*$", 0) << series << false;
    QTest::newRow("study description, match") << HangingProtocolImageSetRestriction(0, "StudyDescription", "study", 0) << series << true;
    QTest::newRow("study description, no match") << HangingProtocolImageSetRestriction(0, "StudyDescription", "[0-9]", 0) << series << false;
    QTest::newRow("patient name, match") << HangingProtocolImageSetRestriction(0, "PatientName", "Smith^John", 0) << series << true;
    QTest::newRow("patient name, no match") << HangingProtocolImageSetRestriction(0, "PatientName", "Smith", 0) << series << false;
    QTest::newRow("series number, match") << HangingProtocolImageSetRestriction(0, "SeriesNumber", "1", 0) << series << true;
    QTest::newRow("series number, no match") << HangingProtocolImageSetRestriction(0, "SeriesNumber", "4", 0) << series << false;
    QTest::newRow("minimum number of images, match") << HangingProtocolImageSetRestriction(0, "MinimumNumberOfImages", "1", 0) << series << true;
    QTest::newRow("minimum number of images, no match") << HangingProtocolImageSetRestriction(0, "MinimumNumberOfImages", "2", 0) << series << false;
}

void test_HangingProtocolImageSetRestriction::test_Series_ShouldReturnExpectedValue()
{
    QFETCH(HangingProtocolImageSetRestriction, restriction);
    QFETCH(Series*, series);
    QFETCH(bool, expectedValue);

    QCOMPARE(restriction.test(series), expectedValue);
}

void test_HangingProtocolImageSetRestriction::test_Image_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<HangingProtocolImageSetRestriction>("restriction");
    QTest::addColumn<Image*>("image");
    QTest::addColumn<bool>("expectedValue");

    Series *series = new Series(this);
    series->setLaterality('R');

    Image *image = new Image();
    image->setParentSeries(series);
    image->setViewPosition("LATERAL");
    image->setImageLaterality('R');
    PatientOrientation patientOrientation;
    patientOrientation.setDICOMFormattedPatientOrientation("R\\F");
    image->setPatientOrientation(patientOrientation);
    image->setViewCodeMeaning("cranio-caudal");
    image->setImageType("LOCALIZER");

    Volume *volume = new Volume(this);
    volume->addImage(image);
    series->addVolume(volume);

    QTest::newRow("empty selector attribute") << HangingProtocolImageSetRestriction() << image << true;
    QTest::newRow("unsupported selector attribute") << HangingProtocolImageSetRestriction(0, "foo", "", 0) << image << true;
    QTest::newRow("view position, match") << HangingProtocolImageSetRestriction(0, "ViewPosition", "LATERAL", 0) << image << true;
    QTest::newRow("view position, no match") << HangingProtocolImageSetRestriction(0, "ViewPosition", "AP", 0) << image << false;
    QTest::newRow("image laterality, match") << HangingProtocolImageSetRestriction(0, "ImageLaterality", "R", 0) << image << true;
    QTest::newRow("image laterality, no match") << HangingProtocolImageSetRestriction(0, "ImageLaterality", "L", 0) << image << false;
    QTest::newRow("laterality, match") << HangingProtocolImageSetRestriction(0, "Laterality", "R", 0) << image << true;
    QTest::newRow("laterality, no match") << HangingProtocolImageSetRestriction(0, "Laterality", "L", 0) << image << false;
    QTest::newRow("patient orientation, match") << HangingProtocolImageSetRestriction(0, "PatientOrientation", "[RL]\\\\[FH]", 0) << image << true;
    QTest::newRow("patient orientation, no match") << HangingProtocolImageSetRestriction(0, "PatientOrientation", "[AP]\\\\[HF]", 0) << image << false;
    QTest::newRow("code meaning, match") << HangingProtocolImageSetRestriction(0, "CodeMeaning", "cranio-caudal", 0) << image << true;
    QTest::newRow("code meaning, no match") << HangingProtocolImageSetRestriction(0, "CodeMeaning", "lateral|oblique", 0) << image << false;
    QTest::newRow("image type, match") << HangingProtocolImageSetRestriction(0, "ImageType", "localizer|secondary", 0) << image << true;
    QTest::newRow("image type, no match") << HangingProtocolImageSetRestriction(0, "ImageType", "original", 0) << image << false;
    QTest::newRow("minimum number of images, match") << HangingProtocolImageSetRestriction(0, "MinimumNumberOfImages", "1", 0) << image << true;
    QTest::newRow("minimum number of images, no match") << HangingProtocolImageSetRestriction(0, "MinimumNumberOfImages", "2", 0) << image << false;
}

void test_HangingProtocolImageSetRestriction::test_Image_ShouldReturnExpectedValue()
{
    QFETCH(HangingProtocolImageSetRestriction, restriction);
    QFETCH(Image*, image);
    QFETCH(bool, expectedValue);

    QCOMPARE(restriction.test(image), expectedValue);
}

DECLARE_TEST(test_HangingProtocolImageSetRestriction)

#include "test_hangingprotocolimagesetrestriction.moc"
