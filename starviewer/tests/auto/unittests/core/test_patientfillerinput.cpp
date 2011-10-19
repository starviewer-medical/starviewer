#include "autotest.h"
#include "patientfillerinput.h"
#include "series.h"

using namespace udg;

class test_PatientFillerInput : public QObject {
Q_OBJECT

private slots:
    void getCurrentSingleFrameVolumeNumber_ShouldReturnErrorWithNullSeries();
    void increaseCurrentSingleFrameVolumeNumber_ShouldIncreaseCountersSeparately();

    void getCurrentMultiframeVolumeNumber_ShouldReturnErrorWithNullSeries();
    void increaseCurrentMultiframeVolumeNumber_ShouldIncreaseCountersSeparately();
};

void test_PatientFillerInput::getCurrentSingleFrameVolumeNumber_ShouldReturnErrorWithNullSeries()
{
    PatientFillerInput *patientFillerInput = new PatientFillerInput();

    patientFillerInput->setCurrentSeries(NULL);
    QCOMPARE(patientFillerInput->getCurrentSingleFrameVolumeNumber(), -1);

    delete patientFillerInput;
}

void test_PatientFillerInput::increaseCurrentSingleFrameVolumeNumber_ShouldIncreaseCountersSeparately()
{
    PatientFillerInput *patientFillerInput = new PatientFillerInput();

    patientFillerInput->setCurrentSeries(NULL);
    QCOMPARE(patientFillerInput->getCurrentSingleFrameVolumeNumber(), -1);

    Series *series1 = new Series();
    patientFillerInput->setCurrentSeries(series1);
    QCOMPARE(patientFillerInput->getCurrentSingleFrameVolumeNumber(), 100);

    patientFillerInput->increaseCurrentSingleFrameVolumeNumber();

    Series *series2 = new Series();
    patientFillerInput->setCurrentSeries(series2);
    patientFillerInput->increaseCurrentSingleFrameVolumeNumber();
    patientFillerInput->increaseCurrentSingleFrameVolumeNumber();
    QCOMPARE(patientFillerInput->getCurrentSingleFrameVolumeNumber(), 102);

    patientFillerInput->setCurrentSeries(series1);
    QCOMPARE(patientFillerInput->getCurrentSingleFrameVolumeNumber(), 101);

    delete patientFillerInput;
    delete series1;
    delete series2;
}

void test_PatientFillerInput::getCurrentMultiframeVolumeNumber_ShouldReturnErrorWithNullSeries()
{
    PatientFillerInput *patientFillerInput = new PatientFillerInput();

    patientFillerInput->setCurrentSeries(NULL);
    QCOMPARE(patientFillerInput->getCurrentMultiframeVolumeNumber(), -1);

    delete patientFillerInput;
}

void test_PatientFillerInput::increaseCurrentMultiframeVolumeNumber_ShouldIncreaseCountersSeparately()
{
    PatientFillerInput *patientFillerInput = new PatientFillerInput();

    patientFillerInput->setCurrentSeries(NULL);
    QCOMPARE(patientFillerInput->getCurrentMultiframeVolumeNumber(), -1);

    Series *series1 = new Series();
    patientFillerInput->setCurrentSeries(series1);
    QCOMPARE(patientFillerInput->getCurrentMultiframeVolumeNumber(), 1);

    patientFillerInput->increaseCurrentMultiframeVolumeNumber();

    Series *series2 = new Series();
    patientFillerInput->setCurrentSeries(series2);
    patientFillerInput->increaseCurrentMultiframeVolumeNumber();
    patientFillerInput->increaseCurrentMultiframeVolumeNumber();
    QCOMPARE(patientFillerInput->getCurrentMultiframeVolumeNumber(), 3);

    patientFillerInput->setCurrentSeries(series1);
    QCOMPARE(patientFillerInput->getCurrentMultiframeVolumeNumber(), 2);

    delete patientFillerInput;
    delete series1;
    delete series2;
}

DECLARE_TEST(test_PatientFillerInput)

#include "test_patientfillerinput.moc"
