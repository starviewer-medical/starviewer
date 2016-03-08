#include "autotest.h"
#include "patientfillerinput.h"

#include "image.h"
#include "patient.h"

using namespace udg;

class test_PatientFillerInput : public QObject {

    Q_OBJECT

private slots:
    void addPatient_ShouldAddPatientIfNotNull();

    void getPatient_ShouldReturnNullIfOutOfRange();

    void getPatientByID_ShouldReturnExpectedPatientOrNull();

    void setCurrentImages_ShouldRespectAddToHistoryParameter();

    void setCurrentImages_ShouldProperlyUpdateMultiframeInfo();

};

void test_PatientFillerInput::addPatient_ShouldAddPatientIfNotNull()
{
    Patient patient;
    PatientFillerInput input;
    input.addPatient(&patient);

    QCOMPARE(input.getNumberOfPatients(), 1);

    input.addPatient(nullptr);

    QCOMPARE(input.getNumberOfPatients(), 1);
}

void test_PatientFillerInput::getPatient_ShouldReturnNullIfOutOfRange()
{
    Patient patient;
    PatientFillerInput input;
    input.addPatient(&patient);

    QCOMPARE(input.getPatient(-1), static_cast<Patient*>(nullptr));
    QCOMPARE(input.getPatient(0), &patient);
    QCOMPARE(input.getPatient(1), static_cast<Patient*>(nullptr));
}

void test_PatientFillerInput::getPatientByID_ShouldReturnExpectedPatientOrNull()
{
    Patient patient;
    patient.setID("foo");
    PatientFillerInput input;
    input.addPatient(&patient);

    QCOMPARE(input.getPatientByID("foo"), &patient);
    QCOMPARE(input.getPatientByID("bar"), static_cast<Patient*>(nullptr));
}

void test_PatientFillerInput::setCurrentImages_ShouldRespectAddToHistoryParameter()
{
    Series series;
    PatientFillerInput input;
    input.setCurrentSeries(&series);
    input.setCurrentImages(QList<Image*>(), false);

    QVERIFY(input.getCurrentImagesHistory().isEmpty());

    input.setCurrentImages(QList<Image*>(), true);

    QVERIFY(!input.getCurrentImagesHistory().isEmpty());
}

void test_PatientFillerInput::setCurrentImages_ShouldProperlyUpdateMultiframeInfo()
{
    Series series;
    Image image;
    PatientFillerInput input;
    input.setCurrentSeries(&series);

    QVERIFY(!input.currentSeriesContainsMultiframeImages());

    input.setCurrentImages(QList<Image*>());

    QVERIFY(!input.currentSeriesContainsMultiframeImages());

    input.setCurrentImages(QList<Image*>() << &image);

    QVERIFY(!input.currentSeriesContainsMultiframeImages());

    input.setCurrentImages(QList<Image*>() << &image << &image);

    QVERIFY(input.currentSeriesContainsMultiframeImages());

    input.setCurrentImages(QList<Image*>() << &image);

    QVERIFY(input.currentSeriesContainsMultiframeImages());
}

DECLARE_TEST(test_PatientFillerInput)

#include "test_patientfillerinput.moc"
