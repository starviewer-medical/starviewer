#include "autotest.h"
#include "imagefillerstep.h"

#include "dicomtagreader.h"
#include "image.h"
#include "patientfillerinput.h"
#include "series.h"
#include "testingdicomtagreader.h"

#include <QList>

using namespace udg;
using namespace testing;

class test_ImageFillerStep : public QObject {
Q_OBJECT

private slots:

    void fillIndividually_ShouldReturnExpectedValue_data();
    void fillIndividually_ShouldReturnExpectedValue();

    void fillIndividually_ShouldFillWellFormattedPixelSpacingProperly_data();
    void fillIndividually_ShouldFillWellFormattedPixelSpacingProperly();

    void fillIndividually_ShouldNotFillBadFormattedPixelSpacing_data();
    void fillIndividually_ShouldNotFillBadFormattedPixelSpacing();

    void fillIndividually_ShouldFillWellFormattedImagerPixelSpacingProperly_data();
    void fillIndividually_ShouldFillWellFormattedImagerPixelSpacingProperly();

    void fillIndividually_ShouldFillWellFormattedPixelSpacingAndImagerPixelSpacingProperly_data();
    void fillIndividually_ShouldFillWellFormattedPixelSpacingAndImagerPixelSpacingProperly();

    void fillIndividually_ShouldFillEstimatedRadiographicMagnificationFactorForTheAppropiateModalities_data();
    void fillIndividually_ShouldFillEstimatedRadiographicMagnificationFactorForTheAppropiateModalities();

private:

    static TestingDICOMTagReader* createReader(int i, const QString &modality = "CT", const QString &SOPClassUID = UIDCTImageStorage);

};

Q_DECLARE_METATYPE(TestingDICOMTagReader*)
Q_DECLARE_METATYPE(QList<TestingDICOMTagReader*>)
Q_DECLARE_METATYPE(QList<int>)
Q_DECLARE_METATYPE(Image*)

void test_ImageFillerStep::fillIndividually_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<TestingDICOMTagReader*>("reader");
    QTest::addColumn<bool>("expectedValue");

    QTest::newRow("null reader") << static_cast<TestingDICOMTagReader*>(NULL) << false;
    QTest::newRow("empty reader") << new TestingDICOMTagReader() << true;
}

void test_ImageFillerStep::fillIndividually_ShouldReturnExpectedValue()
{
    QFETCH(TestingDICOMTagReader*, reader);
    QFETCH(bool, expectedValue);

    PatientFillerInput *input = new PatientFillerInput();
    input->setDICOMFile(reader);
    ImageFillerStep step;
    step.setInput(input);

    QCOMPARE(step.fillIndividually(), expectedValue);

    delete input;
}

void test_ImageFillerStep::fillIndividually_ShouldFillWellFormattedPixelSpacingProperly_data()
{
    QTest::addColumn<TestingDICOMTagReader*>("tagReader");
    QTest::addColumn<Image*>("expectedResultingImage");

    TestingDICOMTagReader *reader = 0;
    Image *imageWithPixelSpacing =  new Image(this);
    imageWithPixelSpacing->setPixelSpacing(1, 1);
    QString wellFormattedPixelSpacingString("1\\1");
    
    // Modalities that take into account PixelSpacing tag, although it's 
    // not defined in standard that these modalities should have pixel spacing tag
    reader = createReader(0, "XC", UIDVLPhotographicImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("XC PixelSpacing - well formatted") << reader << imageWithPixelSpacing;

    reader = createReader(0, "OP", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("OP (SC SOP Class) PixelSpacing - well formatted") << reader << imageWithPixelSpacing;

    reader = createReader(0, "OP", UIDOphthalmicPhotography16BitImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("OP (16 bit SOP Class) PixelSpacing - well formatted") << reader << imageWithPixelSpacing;

    reader = createReader(0, "OP", UIDOphthalmicPhotography8BitImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("OP (8 bit SOP Class) PixelSpacing - well formatted") << reader << imageWithPixelSpacing;
    
    reader = createReader(0, "SC", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("SC PixelSpacing - well formatted") << reader << imageWithPixelSpacing;

    reader = createReader(0, "NM", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("NM PixelSpacing - well formatted") << reader << imageWithPixelSpacing;
    
    reader = createReader(0, "ES", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("ES PixelSpacing - well formatted") << reader << imageWithPixelSpacing;
    
    // Modalities that take into account PixelSpacing
    reader = createReader(0, "CT", UIDCTImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("CT PixelSpacing - well formatted") << reader << imageWithPixelSpacing;
    
    reader = createReader(0, "MR", UIDMRImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("MR PixelSpacing - well formatted") << reader << imageWithPixelSpacing;

    reader = createReader(0, "PT", UIDPositronEmissionTomographyImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("PET PixelSpacing - well formatted") << reader << imageWithPixelSpacing;
    
    reader = createReader(0, "CR", UIDComputedRadiographyImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("CR PixelSpacing - well formatted") << reader << imageWithPixelSpacing;

    reader = createReader(0, "DX", UIDDigitalXRayImageStorageForPresentation);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("DX PixelSpacing - well formatted") << reader << imageWithPixelSpacing;
    
    reader = createReader(0, "MG", UIDDigitalMammographyXRayImageStorageForPresentation);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("MG PixelSpacing - well formatted") << reader << imageWithPixelSpacing;

    reader = createReader(0, "XA", UIDXRayAngiographicImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("XA PixelSpacing - well formatted") << reader << imageWithPixelSpacing;

    reader = createReader(0, "RF", UIDXRayRadioFluoroscopicImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("RF PixelSpacing - well formatted") << reader << imageWithPixelSpacing;
    
    reader = createReader(0, "IO", UIDDigitalIntraOralXRayImageStorageForPresentation);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("IO PixelSpacing - well formatted") << reader << imageWithPixelSpacing;
    
    // Modalities that should ignore PixelSpacing tag although it is present
    Image *imageWithNoPixelSpacing =  new Image(this);
    imageWithNoPixelSpacing->setPixelSpacing(0, 0);
    
    reader = createReader(0, "US", UIDUltrasoundImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("US PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    reader = createReader(0, "US", UIDUltrasoundMultiframeImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("US Multiframe PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    // Enhanced modalities, should ignore PixelSpacing tag
    reader = createReader(0, "CT", UIDEnhancedCTImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced CT PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    reader = createReader(0, "MR", UIDEnhancedMRImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced MR PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    reader = createReader(0, "MR", UIDEnhancedMRColorImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced MR Color PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    reader = createReader(0, "PT", UIDEnhancedPETImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced PET PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    reader = createReader(0, "XA", UIDEnhancedXAImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced XA PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    reader = createReader(0, "RF", UIDEnhancedXRFImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced RF PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    reader = createReader(0, "US", UIDEnhancedUSVolumeStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced US Volume PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;

    // TODO Enhanced modalities test cases with their appropiate pixel spacing attributes in sequences are missing
}

void test_ImageFillerStep::fillIndividually_ShouldFillWellFormattedPixelSpacingProperly()
{
    QFETCH(TestingDICOMTagReader*, tagReader);
    QFETCH(Image*, expectedResultingImage);

    Series *series = new Series(this);
    PatientFillerInput *input = new PatientFillerInput();
    input->setCurrentSeries(series);
    ImageFillerStep step;
    step.setInput(input);
    input->setDICOMFile(tagReader);
    
    QCOMPARE(step.fillIndividually(), true);
    Image *image = series->getImageByIndex(0);
    QVERIFY2(image, "Returned image is null");
    QVERIFY(image->getPixelSpacing().isEqual(expectedResultingImage->getPixelSpacing()));

    // Imager Pixel Spacing should not be filled
    QVERIFY(image->getImagerPixelSpacing().isEqual(PixelSpacing2D()));
}

void test_ImageFillerStep::fillIndividually_ShouldNotFillBadFormattedPixelSpacing_data()
{
    QTest::addColumn<TestingDICOMTagReader*>("tagReader");

    TestingDICOMTagReader *reader = 0;
    QString testRowDescription;
    QStringList badFormattedPixelSpacingList;
    badFormattedPixelSpacingList << "1\\1\\" << "abc" << "1" << "a\\b" << "1\\" << "\\1";
    
    foreach (const QString &badFormattedPixelSpacingString, badFormattedPixelSpacingList)
    {
        reader = createReader(0, "CR", UIDComputedRadiographyImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        testRowDescription = "CR PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;

        reader = createReader(0, "DX", UIDDigitalXRayImageStorageForPresentation);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        testRowDescription ="DX PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;
    
        reader = createReader(0, "MG", UIDDigitalMammographyXRayImageStorageForPresentation);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        testRowDescription ="MG PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;

        reader = createReader(0, "XA", UIDXRayAngiographicImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        testRowDescription ="XA PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;

        reader = createReader(0, "XC", UIDVLPhotographicImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        testRowDescription ="XC PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;
    
        reader = createReader(0, "RF", UIDXRayRadioFluoroscopicImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        testRowDescription ="RF PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;

        reader = createReader(0, "IO", UIDDigitalIntraOralXRayImageStorageForPresentation);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        testRowDescription ="IO PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;

        reader = createReader(0, "US", UIDUltrasoundImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        testRowDescription ="US PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;
    
        reader = createReader(0, "US", UIDUltrasoundMultiframeImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        testRowDescription ="US Multiframe PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;

        reader = createReader(0, "OP", UIDSecondaryCaptureImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        testRowDescription ="OP (SC SOP Class) PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;

        reader = createReader(0, "OP", UIDOphthalmicPhotography16BitImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        testRowDescription ="OP (16 bit SOP Class) PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;

        reader = createReader(0, "OP", UIDOphthalmicPhotography8BitImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        testRowDescription ="OP (8 bit SOP Class) PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;
    
        reader = createReader(0, "SC", UIDSecondaryCaptureImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        testRowDescription ="SC PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;

        reader = createReader(0, "NM", UIDSecondaryCaptureImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        testRowDescription ="NM PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;
    
        reader = createReader(0, "ES", UIDSecondaryCaptureImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        testRowDescription ="ES PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;
    
        reader = createReader(0, "CT", UIDCTImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        testRowDescription ="CT PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;
    
        reader = createReader(0, "MR", UIDMRImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        testRowDescription ="MR PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;

        reader = createReader(0, "PT", UIDPositronEmissionTomographyImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        testRowDescription ="PT PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;
    
        reader = createReader(0, "CT", UIDEnhancedCTImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        reader->addTag(DICOMNumberOfFrames, 1);
        testRowDescription ="Enhanced CT PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;
    
        reader = createReader(0, "MR", UIDEnhancedMRImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        reader->addTag(DICOMNumberOfFrames, 1);
        testRowDescription ="Enhanced MR PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;
    
        reader = createReader(0, "MR", UIDEnhancedMRColorImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        reader->addTag(DICOMNumberOfFrames, 1);
        testRowDescription ="Enhanced MR Color PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;
    
        reader = createReader(0, "PT", UIDEnhancedPETImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        reader->addTag(DICOMNumberOfFrames, 1);
        testRowDescription ="Enhanced PET PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;
    
        reader = createReader(0, "XA", UIDEnhancedXAImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        reader->addTag(DICOMNumberOfFrames, 1);
        testRowDescription ="Enhanced XA PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;
    
        reader = createReader(0, "RF", UIDEnhancedXRFImageStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        reader->addTag(DICOMNumberOfFrames, 1);
        testRowDescription ="Enhanced RF PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;
    
        reader = createReader(0, "US", UIDEnhancedUSVolumeStorage);
        reader->addTag(DICOMPixelSpacing, badFormattedPixelSpacingString);
        reader->addTag(DICOMNumberOfFrames, 1);
        testRowDescription ="Enhanced US Volume PixelSpacing - bad formatted ->" + badFormattedPixelSpacingString;
        QTest::newRow(qPrintable(testRowDescription)) << reader;

        // TODO Enhanced modalities test cases with their appropiate pixel spacing attributes in sequences are missing
    }
}

void test_ImageFillerStep::fillIndividually_ShouldNotFillBadFormattedPixelSpacing()
{
    QFETCH(TestingDICOMTagReader*, tagReader);

    Series *series = new Series(this);
    PatientFillerInput *input = new PatientFillerInput();
    input->setCurrentSeries(series);
    ImageFillerStep step;
    step.setInput(input);
    input->setDICOMFile(tagReader);
    
    QCOMPARE(step.fillIndividually(), true);
    Image *image = series->getImageByIndex(0);
    QVERIFY2(image, "Returned image is null");
    QVERIFY(image->getPixelSpacing().isEqual(PixelSpacing2D()));
}

void test_ImageFillerStep::fillIndividually_ShouldFillWellFormattedImagerPixelSpacingProperly_data()
{
    QTest::addColumn<TestingDICOMTagReader*>("tagReader");
    QTest::addColumn<Image*>("expectedResultingImage");

    TestingDICOMTagReader *reader = 0;
    Image *imageWithNoImagerPixelSpacing =  new Image(this);
    imageWithNoImagerPixelSpacing->setImagerPixelSpacing(0, 0);
    QString wellFormattedImagerPixelSpacingString("1\\1");
    
    // Modalities that do not take into account ImagerPixelSpacing tag, although it exists
    reader = createReader(0, "CT", UIDCTImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("CT ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;
    
    reader = createReader(0, "MR", UIDMRImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("MR ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;

    reader = createReader(0, "PT", UIDPositronEmissionTomographyImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("PET ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;
    
    reader = createReader(0, "US", UIDUltrasoundImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("US ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;
    
    reader = createReader(0, "US", UIDUltrasoundMultiframeImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("US Multiframe ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;

    reader = createReader(0, "XC", UIDVLPhotographicImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("XC ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;

    reader = createReader(0, "OP", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("OP (SC SOP Class) ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;

    reader = createReader(0, "OP", UIDOphthalmicPhotography16BitImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("OP (16 bit SOP Class) ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;

    reader = createReader(0, "OP", UIDOphthalmicPhotography8BitImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("OP (8 bit SOP Class) ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;
    
    reader = createReader(0, "SC", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("SC ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;

    reader = createReader(0, "NM", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("NM ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;
    
    reader = createReader(0, "ES", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("ES ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;
    
    // Modalities that take into account ImagerPixelSpacing
    Image *imageWithImagerPixelSpacing =  new Image(this);
    imageWithImagerPixelSpacing->setImagerPixelSpacing(1, 1);
    
    reader = createReader(0, "CR", UIDComputedRadiographyImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("CR ImagerPixelSpacing - well formatted") << reader << imageWithImagerPixelSpacing;

    reader = createReader(0, "DX", UIDDigitalXRayImageStorageForPresentation);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("DX ImagerPixelSpacing - well formatted") << reader << imageWithImagerPixelSpacing;
    
    reader = createReader(0, "MG", UIDDigitalMammographyXRayImageStorageForPresentation);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("MG ImagerPixelSpacing - well formatted") << reader << imageWithImagerPixelSpacing;

    reader = createReader(0, "XA", UIDXRayAngiographicImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("XA ImagerPixelSpacing - well formatted") << reader << imageWithImagerPixelSpacing;

    reader = createReader(0, "RF", UIDXRayRadioFluoroscopicImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("RF ImagerPixelSpacing - well formatted") << reader << imageWithImagerPixelSpacing;

    reader = createReader(0, "IO", UIDDigitalIntraOralXRayImageStorageForPresentation);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("IO ImagerPixelSpacing - well formatted") << reader << imageWithImagerPixelSpacing;

    // TODO The test case for 3D XA is missing
    // In the case for 3D XA, we should look for Imager Pixel Spacing in
    // X-Ray 3D Angiographic Image Contributing Sources Module (C.8.21.2.1), Contributing Sources Sequence
    
    // Enhanced modalities that should ignore ImagerPixelSpacing tag
    reader = createReader(0, "CT", UIDEnhancedCTImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced CT ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;
    
    reader = createReader(0, "MR", UIDEnhancedMRImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced MR ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;
    
    reader = createReader(0, "MR", UIDEnhancedMRColorImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced MR Color ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;
    
    reader = createReader(0, "PT", UIDEnhancedPETImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced PET ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;
    
    reader = createReader(0, "US", UIDEnhancedUSVolumeStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced US Volume ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;

    reader = createReader(0, "XA", UIDEnhancedXAImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced XA ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;
    
    reader = createReader(0, "RF", UIDEnhancedXRFImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced RF ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;

    // TODO Enhanced modalities test cases with their appropiate pixel spacing attributes in sequences are missing
}

void test_ImageFillerStep::fillIndividually_ShouldFillWellFormattedImagerPixelSpacingProperly()
{
    QFETCH(TestingDICOMTagReader*, tagReader);
    QFETCH(Image*, expectedResultingImage);

    Series *series = new Series(this);
    PatientFillerInput *input = new PatientFillerInput();
    input->setCurrentSeries(series);
    ImageFillerStep step;
    step.setInput(input);
    input->setDICOMFile(tagReader);
    
    QCOMPARE(step.fillIndividually(), true);
    Image *image = series->getImageByIndex(0);
    QVERIFY2(image, "Returned image is null");
    QVERIFY(image->getImagerPixelSpacing().isEqual(expectedResultingImage->getImagerPixelSpacing()));

    // Pixel Spacing should not be filled
    QVERIFY(image->getPixelSpacing().isEqual(PixelSpacing2D()));
}

void test_ImageFillerStep::fillIndividually_ShouldFillWellFormattedPixelSpacingAndImagerPixelSpacingProperly_data()
{
    QTest::addColumn<TestingDICOMTagReader*>("tagReader");
    QTest::addColumn<Image*>("expectedResultingImage");

    TestingDICOMTagReader *reader = 0;
    QString wellFormattedPixelSpacingString("0.5\\0.5");
    QString wellFormattedImagerPixelSpacingString("0.75\\0.75");
    Image *imageWithNoImagerPixelSpacing =  new Image(this);
    imageWithNoImagerPixelSpacing->setPixelSpacing(0.5, 0.5);
    imageWithNoImagerPixelSpacing->setImagerPixelSpacing(0, 0);
    
    // Modalities that do not take into account ImagerPixelSpacing tag, although it exists, but do take into account PixelSpacing
    reader = createReader(0, "CT", UIDCTImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("CT PixelSpacing & ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;
    
    reader = createReader(0, "MR", UIDMRImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("MR PixelSpacing & ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;

    reader = createReader(0, "PT", UIDPositronEmissionTomographyImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("PET PixelSpacing & ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;
    
    // Modalities that do not take into account neither Pixel Spacing nor Imager Pixel Spacing although they are present
    Image *neitherPixelSpacingNorImagerPixelSpacing =  new Image(this);
    neitherPixelSpacingNorImagerPixelSpacing->setPixelSpacing(0.0, 0.0);
    neitherPixelSpacingNorImagerPixelSpacing->setImagerPixelSpacing(0, 0);
    
    reader = createReader(0, "US", UIDUltrasoundImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("US PixelSpacing & ImagerPixelSpacing - well formatted") << reader << neitherPixelSpacingNorImagerPixelSpacing;

    reader = createReader(0, "US", UIDUltrasoundMultiframeImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("US Multiframe PixelSpacing & ImagerPixelSpacing - well formatted") << reader << neitherPixelSpacingNorImagerPixelSpacing;
    //
    reader = createReader(0, "XC", UIDVLPhotographicImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("XC PixelSpacing & ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;

    reader = createReader(0, "OP", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("OP (SC SOP Class) PixelSpacing & ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;

    reader = createReader(0, "OP", UIDOphthalmicPhotography16BitImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("OP (16 bit SOP Class) PixelSpacing & ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;

    reader = createReader(0, "OP", UIDOphthalmicPhotography8BitImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("OP (8 bit SOP Class) PixelSpacing & ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;
    
    reader = createReader(0, "SC", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("SC PixelSpacing & ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;

    reader = createReader(0, "NM", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("NM PixelSpacing & ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;
    
    reader = createReader(0, "ES", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("ES PixelSpacing & ImagerPixelSpacing - well formatted") << reader << imageWithNoImagerPixelSpacing;
    
    // Modalities that take into account both PixelSpacing and ImagerPixelSpacing
    Image *bothPixelSpacingAndImagerPixelSpacing =  new Image(this);
    bothPixelSpacingAndImagerPixelSpacing->setPixelSpacing(0.5, 0.5);
    bothPixelSpacingAndImagerPixelSpacing->setImagerPixelSpacing(0.75, 0.75);
    
    reader = createReader(0, "CR", UIDComputedRadiographyImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("CR PixelSpacing & ImagerPixelSpacing - well formatted") << reader << bothPixelSpacingAndImagerPixelSpacing;

    reader = createReader(0, "DX", UIDDigitalXRayImageStorageForPresentation);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("DX PixelSpacing & ImagerPixelSpacing - well formatted") << reader << bothPixelSpacingAndImagerPixelSpacing;
    
    reader = createReader(0, "MG", UIDDigitalMammographyXRayImageStorageForPresentation);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("MG PixelSpacing & ImagerPixelSpacing - well formatted") << reader << bothPixelSpacingAndImagerPixelSpacing;

    reader = createReader(0, "XA", UIDXRayAngiographicImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("XA PixelSpacing & ImagerPixelSpacing - well formatted") << reader << bothPixelSpacingAndImagerPixelSpacing;

    reader = createReader(0, "RF", UIDXRayRadioFluoroscopicImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("RF PixelSpacing & ImagerPixelSpacing - well formatted") << reader << bothPixelSpacingAndImagerPixelSpacing;

    reader = createReader(0, "IO", UIDDigitalIntraOralXRayImageStorageForPresentation);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    QTest::newRow("IO PixelSpacing & ImagerPixelSpacing - well formatted") << reader << bothPixelSpacingAndImagerPixelSpacing;
    
    // TODO The test case for 3D XA is missing
    // In the case for 3D XA, we should look for Imager Pixel Spacing in
    // X-Ray 3D Angiographic Image Contributing Sources Module (C.8.21.2.1), Contributing Sources Sequence
    
    // Enhanced modalities that should ignore both PixelSpacing and ImagerPixelSpacing tag
    reader = createReader(0, "CT", UIDEnhancedCTImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced CT PixelSpacing & ImagerPixelSpacing - well formatted") << reader << neitherPixelSpacingNorImagerPixelSpacing;
    
    reader = createReader(0, "MR", UIDEnhancedMRImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced MR PixelSpacing & ImagerPixelSpacing - well formatted") << reader << neitherPixelSpacingNorImagerPixelSpacing;
    
    reader = createReader(0, "MR", UIDEnhancedMRColorImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced MR Color PixelSpacing & ImagerPixelSpacing - well formatted") << reader << neitherPixelSpacingNorImagerPixelSpacing;
    
    reader = createReader(0, "PT", UIDEnhancedPETImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced PET PixelSpacing & ImagerPixelSpacing - well formatted") << reader << neitherPixelSpacingNorImagerPixelSpacing;
    
    reader = createReader(0, "US", UIDEnhancedUSVolumeStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced US Volume PixelSpacing & ImagerPixelSpacing - well formatted") << reader << neitherPixelSpacingNorImagerPixelSpacing;

    reader = createReader(0, "XA", UIDEnhancedXAImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced XA PixelSpacing & ImagerPixelSpacing - well formatted") << reader << neitherPixelSpacingNorImagerPixelSpacing;
    
    reader = createReader(0, "RF", UIDEnhancedXRFImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedImagerPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced RF PixelSpacing & ImagerPixelSpacing - well formatted") << reader << neitherPixelSpacingNorImagerPixelSpacing;

    // TODO Enhanced modalities test cases with their appropiate pixel spacing attributes in sequences are missing
}

void test_ImageFillerStep::fillIndividually_ShouldFillWellFormattedPixelSpacingAndImagerPixelSpacingProperly()
{
    QFETCH(TestingDICOMTagReader*, tagReader);
    QFETCH(Image*, expectedResultingImage);

    Series *series = new Series(this);
    PatientFillerInput *input = new PatientFillerInput();
    input->setCurrentSeries(series);
    ImageFillerStep step;
    step.setInput(input);
    input->setDICOMFile(tagReader);
    
    QCOMPARE(step.fillIndividually(), true);
    Image *image = series->getImageByIndex(0);
    QVERIFY2(image, "Returned image is null");
    // Check PixelSpacing
    QVERIFY(image->getPixelSpacing().isEqual(expectedResultingImage->getPixelSpacing()));
    // Check ImagerPixelSpacing
    QVERIFY(image->getImagerPixelSpacing().isEqual(expectedResultingImage->getImagerPixelSpacing()));
}

void test_ImageFillerStep::fillIndividually_ShouldFillEstimatedRadiographicMagnificationFactorForTheAppropiateModalities_data()
{
    QTest::addColumn<TestingDICOMTagReader*>("tagReader");
    QTest::addColumn<double>("expectedValue");

    TestingDICOMTagReader *reader = 0;
    double estimatedRadiographicMagnificationFactor = 1.75;
    double defaultestimatedRadiographicMagnificationFactor = 1.0;

    // Modalities that take into account EstimatedRadiographicMagnificationFactor
    reader = createReader(0, "DX", UIDDigitalXRayImageStorageForPresentation);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    QTest::newRow("DX EstimatedRadiographicMagnificationFactor") << reader << estimatedRadiographicMagnificationFactor;
    
    reader = createReader(0, "MG", UIDDigitalMammographyXRayImageStorageForPresentation);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    QTest::newRow("MG EstimatedRadiographicMagnificationFactor") << reader << estimatedRadiographicMagnificationFactor;

    reader = createReader(0, "XA", UIDXRayAngiographicImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    QTest::newRow("XA EstimatedRadiographicMagnificationFactor") << reader << estimatedRadiographicMagnificationFactor;

    reader = createReader(0, "RF", UIDXRayRadioFluoroscopicImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    QTest::newRow("RF EstimatedRadiographicMagnificationFactor") << reader << estimatedRadiographicMagnificationFactor;

    reader = createReader(0, "IO", UIDDigitalIntraOralXRayImageStorageForPresentation);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    QTest::newRow("IO EstimatedRadiographicMagnificationFactor") << reader << estimatedRadiographicMagnificationFactor;
    
    // Modalities that ignore EstimatedRadiographicMagnificationFactor although it is present
    reader = createReader(0, "CR", UIDComputedRadiographyImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    QTest::newRow("CR EstimatedRadiographicMagnificationFactor") << reader << defaultestimatedRadiographicMagnificationFactor;

    reader = createReader(0, "CT", UIDCTImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    QTest::newRow("CT EstimatedRadiographicMagnificationFactor") << reader << defaultestimatedRadiographicMagnificationFactor;
    
    reader = createReader(0, "MR", UIDMRImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    QTest::newRow("MR EstimatedRadiographicMagnificationFactor") << reader << defaultestimatedRadiographicMagnificationFactor;

    reader = createReader(0, "PT", UIDPositronEmissionTomographyImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    QTest::newRow("PET EstimatedRadiographicMagnificationFactor") << reader << defaultestimatedRadiographicMagnificationFactor;
    
    reader = createReader(0, "US", UIDUltrasoundImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    QTest::newRow("US EstimatedRadiographicMagnificationFactor") << reader << defaultestimatedRadiographicMagnificationFactor;

    reader = createReader(0, "US", UIDUltrasoundMultiframeImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    QTest::newRow("US Multiframe EstimatedRadiographicMagnificationFactor") << reader << defaultestimatedRadiographicMagnificationFactor;
    //
    reader = createReader(0, "XC", UIDVLPhotographicImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    QTest::newRow("XC EstimatedRadiographicMagnificationFactor") << reader << defaultestimatedRadiographicMagnificationFactor;

    reader = createReader(0, "OP", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    QTest::newRow("OP (SC SOP Class) EstimatedRadiographicMagnificationFactor") << reader << defaultestimatedRadiographicMagnificationFactor;

    reader = createReader(0, "OP", UIDOphthalmicPhotography16BitImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    QTest::newRow("OP (16 bit SOP Class) EstimatedRadiographicMagnificationFactor") << reader << defaultestimatedRadiographicMagnificationFactor;

    reader = createReader(0, "OP", UIDOphthalmicPhotography8BitImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    QTest::newRow("OP (8 bit SOP Class) EstimatedRadiographicMagnificationFactor") << reader << defaultestimatedRadiographicMagnificationFactor;
    
    reader = createReader(0, "SC", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    QTest::newRow("SC EstimatedRadiographicMagnificationFactor") << reader << defaultestimatedRadiographicMagnificationFactor;

    reader = createReader(0, "NM", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    QTest::newRow("NM EstimatedRadiographicMagnificationFactor") << reader << defaultestimatedRadiographicMagnificationFactor;
    
    reader = createReader(0, "ES", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    QTest::newRow("ES EstimatedRadiographicMagnificationFactor") << reader << defaultestimatedRadiographicMagnificationFactor;
    
    // Enhanced modalities should ignore EstimatedRadiographicMagnificationFactor although it is present
    reader = createReader(0, "CT", UIDEnhancedCTImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced CT EstimatedRadiographicMagnificationFactor") << reader << defaultestimatedRadiographicMagnificationFactor;
    
    reader = createReader(0, "MR", UIDEnhancedMRImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced MR EstimatedRadiographicMagnificationFactor") << reader << defaultestimatedRadiographicMagnificationFactor;
    
    reader = createReader(0, "MR", UIDEnhancedMRColorImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced MR Color EstimatedRadiographicMagnificationFactor") << reader << defaultestimatedRadiographicMagnificationFactor;
    
    reader = createReader(0, "PT", UIDEnhancedPETImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced PET EstimatedRadiographicMagnificationFactor") << reader << defaultestimatedRadiographicMagnificationFactor;
    
    reader = createReader(0, "US", UIDEnhancedUSVolumeStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced US Volume EstimatedRadiographicMagnificationFactor") << reader << defaultestimatedRadiographicMagnificationFactor;

    reader = createReader(0, "XA", UIDEnhancedXAImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced XA EstimatedRadiographicMagnificationFactor") << reader << defaultestimatedRadiographicMagnificationFactor;
    
    reader = createReader(0, "RF", UIDEnhancedXRFImageStorage);
    reader->addTag(DICOMEstimatedRadiographicMagnificationFactor, estimatedRadiographicMagnificationFactor);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced RF EstimatedRadiographicMagnificationFactor") << reader << defaultestimatedRadiographicMagnificationFactor;
}

void test_ImageFillerStep::fillIndividually_ShouldFillEstimatedRadiographicMagnificationFactorForTheAppropiateModalities()
{
    QFETCH(TestingDICOMTagReader*, tagReader);
    QFETCH(double, expectedValue);

    Series *series = new Series(this);
    PatientFillerInput *input = new PatientFillerInput();
    input->setCurrentSeries(series);
    ImageFillerStep step;
    step.setInput(input);
    input->setDICOMFile(tagReader);
    
    QCOMPARE(step.fillIndividually(), true);
    Image *image = series->getImageByIndex(0);
    QVERIFY2(image, "Returned image is null");
    QCOMPARE(image->getEstimatedRadiographicMagnificationFactor(), expectedValue);
}

TestingDICOMTagReader* test_ImageFillerStep::createReader(int i, const QString &modality, const QString &SOPClassUID)
{
    TestingDICOMTagReader *reader = new TestingDICOMTagReader();
    reader->addTag(DICOMPixelData);
    reader->addTag(DICOMSOPClassUID, SOPClassUID);
    reader->addTag(DICOMSOPInstanceUID, i);
    reader->addTag(DICOMModality, modality);
    return reader;
}

DECLARE_TEST(test_ImageFillerStep)

#include "test_imagefillerstep.moc"
