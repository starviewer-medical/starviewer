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

    void fillIndividually_ShouldCreateSeparateVolumesForDifferentSizes_data();
    void fillIndividually_ShouldCreateSeparateVolumesForDifferentSizes();

    void fillIndividually_ShouldCreateSeparateVolumesForDifferentPhotometricInterpretations_data();
    void fillIndividually_ShouldCreateSeparateVolumesForDifferentPhotometricInterpretations();

    void fillIndividually_ShouldCreateSeparateVolumesForDifferentSpacings_data();
    void fillIndividually_ShouldCreateSeparateVolumesForDifferentSpacings();

    void fillIndividually_ShouldFillWellFormattedPixelSpacingProperly_data();
    void fillIndividually_ShouldFillWellFormattedPixelSpacingProperly();

    void fillIndividually_ShouldNotFillBadFormattedPixelSpacing_data();
    void fillIndividually_ShouldNotFillBadFormattedPixelSpacing();

    void fillIndividually_ShouldFillWellFormattedImagerPixelSpacingProperly_data();
    void fillIndividually_ShouldFillWellFormattedImagerPixelSpacingProperly();

private:

    static TestingDICOMTagReader* createReader(int i, const QString &modality = "CT", const QString &SOPClassUID = UIDCTImageStorage);
    void checkVolumeNumberInSeries();

};

Q_DECLARE_METATYPE(TestingDICOMTagReader*)
Q_DECLARE_METATYPE(QList<TestingDICOMTagReader*>)
Q_DECLARE_METATYPE(QList<int>)
Q_DECLARE_METATYPE(Image*)

void test_ImageFillerStep::fillIndividually_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<TestingDICOMTagReader*>("reader");
    QTest::addColumn<bool>("expectedValue");

    QTest::newRow("null reader") << static_cast<TestingDICOMTagReader*>(nullptr) << false;
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

void test_ImageFillerStep::fillIndividually_ShouldCreateSeparateVolumesForDifferentSizes_data()
{
    QTest::addColumn<QList<TestingDICOMTagReader*>>("readers");
    QTest::addColumn<QList<int>>("volumeNumberInSeries");

    QList<TestingDICOMTagReader*> readers;
    QList<int> volumeNumberInSeries;

    // totes les imatges de la mateixa mida
    {
        readers.clear();
        volumeNumberInSeries.clear();

        for (int i = 0; i < 10; i++)
        {
            auto *reader = createReader(i);
            reader->addTag(DICOMRows, 256);
            reader->addTag(DICOMColumns, 256);
            readers.append(reader);
            volumeNumberInSeries.append(100);
        }

        QTest::newRow("10 images same size") << readers << volumeNumberInSeries;
    }

    // diferents variacions
    {
        readers.clear();
        volumeNumberInSeries.clear();

        for (int i = 0; i < 1; i++)
        {
            auto *reader = createReader(i);
            reader->addTag(DICOMRows, 256);
            reader->addTag(DICOMColumns, 256);
            readers.append(reader);
            volumeNumberInSeries.append(100);
        }

        for (int i = 1; i < 3; i++)
        {
            auto *reader = createReader(i);
            reader->addTag(DICOMRows, 256);
            reader->addTag(DICOMColumns, 512);
            readers.append(reader);
            volumeNumberInSeries.append(101);
        }

        for (int i = 3; i < 6; i++)
        {
            auto *reader = createReader(i);
            reader->addTag(DICOMRows, 512);
            reader->addTag(DICOMColumns, 512);
            readers.append(reader);
            volumeNumberInSeries.append(102);
        }

        for (int i = 6; i < 10; i++)
        {
            auto *reader = createReader(i);
            reader->addTag(DICOMRows, 256);
            reader->addTag(DICOMColumns, 256);
            readers.append(reader);
            volumeNumberInSeries.append(103);
        }

        QTest::newRow("different variations in size") << readers << volumeNumberInSeries;
    }
}

void test_ImageFillerStep::fillIndividually_ShouldCreateSeparateVolumesForDifferentSizes()
{
    checkVolumeNumberInSeries();
}

void test_ImageFillerStep::fillIndividually_ShouldCreateSeparateVolumesForDifferentPhotometricInterpretations_data()
{
    QTest::addColumn<QList<TestingDICOMTagReader*>>("readers");
    QTest::addColumn<QList<int>>("volumeNumberInSeries");

    QList<TestingDICOMTagReader*> readers;
    QList<int> volumeNumberInSeries;

    // totes les imatges amb la mateixa photometric interpretation
    {
        readers.clear();
        volumeNumberInSeries.clear();

        for (int i = 0; i < 10; i++)
        {
            auto *reader = createReader(i);
            reader->addTag(DICOMPhotometricInterpretation, "MONOCHROME2");
            readers.append(reader);
            volumeNumberInSeries.append(100);
        }

        QTest::newRow("10 MONO") << readers << volumeNumberInSeries;
    }

    // 4 + 6
    {
        readers.clear();
        volumeNumberInSeries.clear();

        for (int i = 0; i < 4; i++)
        {
            auto *reader = createReader(i);
            reader->addTag(DICOMPhotometricInterpretation, "MONOCHROME2");
            readers.append(reader);
            volumeNumberInSeries.append(100);
        }

        for (int i = 4; i < 10; i++)
        {
            auto *reader = createReader(i);
            reader->addTag(DICOMPhotometricInterpretation, "RGB");
            readers.append(reader);
            volumeNumberInSeries.append(101);
        }

        QTest::newRow("4 MONO + 6 RGB") << readers << volumeNumberInSeries;
    }

    // 4 + 3 + 3
    {
        readers.clear();
        volumeNumberInSeries.clear();

        for (int i = 0; i < 4; i++)
        {
            auto *reader = createReader(i);
            reader->addTag(DICOMPhotometricInterpretation, "MONOCHROME2");
            readers.append(reader);
            volumeNumberInSeries.append(100);
        }

        for (int i = 4; i < 7; i++)
        {
            auto *reader = createReader(i);
            reader->addTag(DICOMPhotometricInterpretation, "RGB");
            readers.append(reader);
            volumeNumberInSeries.append(101);
        }
        
        for (int i = 7; i < 10; i++)
        {
            auto *reader = createReader(i);
            reader->addTag(DICOMPhotometricInterpretation, "MONOCHROME2");
            readers.append(reader);
            volumeNumberInSeries.append(102);
        }

        QTest::newRow("4 MONO + 3 RGB + 3 MONO") << readers << volumeNumberInSeries;
    }
}

void test_ImageFillerStep::fillIndividually_ShouldCreateSeparateVolumesForDifferentPhotometricInterpretations()
{
    checkVolumeNumberInSeries();
}

void test_ImageFillerStep::fillIndividually_ShouldCreateSeparateVolumesForDifferentSpacings_data()
{
    QTest::addColumn<QList<TestingDICOMTagReader*>>("readers");
    QTest::addColumn<QList<int>>("volumeNumberInSeries");

    QList<TestingDICOMTagReader*> readers;
    QList<int> volumeNumberInSeries;

    // totes les imatges amb el mateix spacing
    {
        readers.clear();
        volumeNumberInSeries.clear();

        for (int i = 0; i < 10; i++)
        {
            auto *reader = createReader(i);
            reader->addTag(DICOMPixelSpacing, "1\\1");
            readers.append(reader);
            volumeNumberInSeries.append(100);
        }

        QTest::newRow("10 images same spacing") << readers << volumeNumberInSeries;
    }

    // 4 + 6
    {
        readers.clear();
        volumeNumberInSeries.clear();

        for (int i = 0; i < 4; i++)
        {
            auto *reader = createReader(i);
            reader->addTag(DICOMPixelSpacing, "1\\1");
            readers.append(reader);
            volumeNumberInSeries.append(100);
        }

        for (int i = 4; i < 10; i++)
        {
            auto *reader = createReader(i);
            reader->addTag(DICOMPixelSpacing, "0.5\\0.5");
            readers.append(reader);
            volumeNumberInSeries.append(101);
        }

        QTest::newRow("4 + 6") << readers << volumeNumberInSeries;
    }

    // 4 + 3 + 3
    {
        readers.clear();
        volumeNumberInSeries.clear();

        for (int i = 0; i < 4; i++)
        {
            auto *reader = createReader(i);
            reader->addTag(DICOMPixelSpacing, "1\\1");
            readers.append(reader);
            volumeNumberInSeries.append(100);
        }

        for (int i = 4; i < 7; i++)
        {
            auto *reader = createReader(i);
            reader->addTag(DICOMPixelSpacing, "0.5\\0.5");
            readers.append(reader);
            volumeNumberInSeries.append(101);
        }
        
        for (int i = 7; i < 10; i++)
        {
            auto *reader = createReader(i);
            reader->addTag(DICOMPixelSpacing, "1\\1");
            readers.append(reader);
            volumeNumberInSeries.append(102);
        }

        QTest::newRow("4 + 3 + 3") << readers << volumeNumberInSeries;
    }
}

void test_ImageFillerStep::fillIndividually_ShouldCreateSeparateVolumesForDifferentSpacings()
{
    checkVolumeNumberInSeries();
}

void test_ImageFillerStep::fillIndividually_ShouldFillWellFormattedPixelSpacingProperly_data()
{
    QTest::addColumn<TestingDICOMTagReader*>("tagReader");
    QTest::addColumn<Image*>("expectedResultingImage");

    TestingDICOMTagReader *reader = 0;
    Image *imageWithNoPixelSpacing =  new Image(this);
    imageWithNoPixelSpacing->setPixelSpacing(0, 0);
    QString wellFormattedPixelSpacingString("1\\1");
    
    // Modalities that do not take into account PixelSpacing tag, although it exists
    reader = createReader(0, "CR", UIDComputedRadiographyImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("CR PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;

    reader = createReader(0, "DX", UIDDigitalXRayImageStorageForPresentation);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("DX PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    reader = createReader(0, "MG", UIDDigitalMammographyXRayImageStorageForPresentation);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("MG PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;

    reader = createReader(0, "XA", UIDXRayAngiographicImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("XA PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;

    reader = createReader(0, "XC", UIDVLPhotographicImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("XC PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    reader = createReader(0, "RF", UIDXRayRadioFluoroscopicImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("RF PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;

    reader = createReader(0, "US", UIDUltrasoundImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("US PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    reader = createReader(0, "US", UIDUltrasoundMultiframeImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("US Multiframe PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;

    reader = createReader(0, "OP", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("OP (SC SOP Class) PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;

    reader = createReader(0, "OP", UIDOphthalmicPhotography16BitImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("OP (16 bit SOP Class) PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;

    reader = createReader(0, "OP", UIDOphthalmicPhotography8BitImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("OP (8 bit SOP Class) PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    reader = createReader(0, "SC", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("SC PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;

    reader = createReader(0, "NM", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("NM PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    reader = createReader(0, "ES", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("ES PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    // Modalities that take into account PixelSpacing
    Image *imageWithPixelSpacing =  new Image(this);
    imageWithPixelSpacing->setPixelSpacing(1, 1);
    
    reader = createReader(0, "CT", UIDCTImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("CT PixelSpacing - well formatted") << reader << imageWithPixelSpacing;
    
    reader = createReader(0, "MR", UIDMRImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("MR PixelSpacing - well formatted") << reader << imageWithPixelSpacing;

    reader = createReader(0, "PT", UIDPositronEmissionTomographyImageStorage);
    reader->addTag(DICOMPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("PET PixelSpacing - well formatted") << reader << imageWithPixelSpacing;
    
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
    QCOMPARE(image->getPixelSpacing()[0], expectedResultingImage->getPixelSpacing()[0]);
    QCOMPARE(image->getPixelSpacing()[1], expectedResultingImage->getPixelSpacing()[1]);
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
    QCOMPARE(image->getPixelSpacing()[0], .0);
    QCOMPARE(image->getPixelSpacing()[1], .0);
}

void test_ImageFillerStep::fillIndividually_ShouldFillWellFormattedImagerPixelSpacingProperly_data()
{
    QTest::addColumn<TestingDICOMTagReader*>("tagReader");
    QTest::addColumn<Image*>("expectedResultingImage");

    TestingDICOMTagReader *reader = 0;
    Image *imageWithNoPixelSpacing =  new Image(this);
    imageWithNoPixelSpacing->setPixelSpacing(0, 0);
    QString wellFormattedPixelSpacingString("1\\1");
    
    // Modalities that do not take into account ImagerPixelSpacing tag, although it exists
    reader = createReader(0, "CT", UIDCTImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("CT PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    reader = createReader(0, "MR", UIDMRImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("MR PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;

    reader = createReader(0, "PT", UIDPositronEmissionTomographyImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("PET PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    reader = createReader(0, "US", UIDUltrasoundImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("US PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    reader = createReader(0, "US", UIDUltrasoundMultiframeImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("US Multiframe PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    // Modalities that take into account ImagerPixelSpacing
    Image *imageWithPixelSpacing =  new Image(this);
    imageWithPixelSpacing->setPixelSpacing(1, 1);
    
    reader = createReader(0, "CR", UIDComputedRadiographyImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("CR PixelSpacing - well formatted") << reader << imageWithPixelSpacing;

    reader = createReader(0, "DX", UIDDigitalXRayImageStorageForPresentation);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("DX PixelSpacing - well formatted") << reader << imageWithPixelSpacing;
    
    reader = createReader(0, "MG", UIDDigitalMammographyXRayImageStorageForPresentation);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("MG PixelSpacing - well formatted") << reader << imageWithPixelSpacing;

    reader = createReader(0, "XA", UIDXRayAngiographicImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("XA PixelSpacing - well formatted") << reader << imageWithPixelSpacing;

    reader = createReader(0, "XC", UIDVLPhotographicImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("XC PixelSpacing - well formatted") << reader << imageWithPixelSpacing;
    
    reader = createReader(0, "RF", UIDXRayRadioFluoroscopicImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("RF PixelSpacing - well formatted") << reader << imageWithPixelSpacing;

    // TODO Enhanced modalities test cases with their appropiate pixel spacing attributes in sequences are missing

    reader = createReader(0, "OP", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("OP (SC SOP Class) PixelSpacing - well formatted") << reader << imageWithPixelSpacing;

    reader = createReader(0, "OP", UIDOphthalmicPhotography16BitImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("OP (16 bit SOP Class) PixelSpacing - well formatted") << reader << imageWithPixelSpacing;

    reader = createReader(0, "OP", UIDOphthalmicPhotography8BitImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("OP (8 bit SOP Class) PixelSpacing - well formatted") << reader << imageWithPixelSpacing;
    
    reader = createReader(0, "SC", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("SC PixelSpacing - well formatted") << reader << imageWithPixelSpacing;

    reader = createReader(0, "NM", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("NM PixelSpacing - well formatted") << reader << imageWithPixelSpacing;
    
    reader = createReader(0, "ES", UIDSecondaryCaptureImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    QTest::newRow("ES PixelSpacing - well formatted") << reader << imageWithPixelSpacing;
    
    // Enhanced modalities that should take into account ImagerPixelSpacing tag
    reader = createReader(0, "XA", UIDEnhancedXAImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced XA PixelSpacing - well formatted") << reader << imageWithPixelSpacing;
    
    reader = createReader(0, "RF", UIDEnhancedXRFImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced RF PixelSpacing - well formatted") << reader << imageWithPixelSpacing;
    
    // Enhanced modalities that should ignore ImagerPixelSpacing tag
    reader = createReader(0, "CT", UIDEnhancedCTImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced CT PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    reader = createReader(0, "MR", UIDEnhancedMRImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced MR PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    reader = createReader(0, "MR", UIDEnhancedMRColorImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced MR Color PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    reader = createReader(0, "PT", UIDEnhancedPETImageStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced PET PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
    
    reader = createReader(0, "US", UIDEnhancedUSVolumeStorage);
    reader->addTag(DICOMImagerPixelSpacing, wellFormattedPixelSpacingString);
    reader->addTag(DICOMNumberOfFrames, 1);
    QTest::newRow("Enhanced US Volume PixelSpacing - well formatted") << reader << imageWithNoPixelSpacing;
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
    QCOMPARE(image->getPixelSpacing()[0], expectedResultingImage->getPixelSpacing()[0]);
    QCOMPARE(image->getPixelSpacing()[1], expectedResultingImage->getPixelSpacing()[1]);
}

TestingDICOMTagReader* test_ImageFillerStep::createReader(int i, const QString &modality, const QString &SOPClassUID)
{
    auto *reader = new TestingDICOMTagReader();
    reader->addTag(DICOMPixelData);
    reader->addTag(DICOMSOPClassUID, SOPClassUID);
    reader->addTag(DICOMSOPInstanceUID, i);
    reader->addTag(DICOMModality, modality);
    return reader;
}

void test_ImageFillerStep::checkVolumeNumberInSeries()
{
    QFETCH(QList<TestingDICOMTagReader*>, readers);
    QFETCH(QList<int>, volumeNumberInSeries);

    Series *series = new Series(this);
    PatientFillerInput *input = new PatientFillerInput();
    input->setCurrentSeries(series);
    ImageFillerStep step;
    step.setInput(input);

    foreach (auto *reader, readers)
    {
        input->setDICOMFile(reader);
        QCOMPARE(step.fillIndividually(), true);
    }

    for (int i = 0; i < volumeNumberInSeries.size(); i++)
    {
        QCOMPARE(series->getImageByIndex(i)->getVolumeNumberInSeries(), volumeNumberInSeries[i]);
    }

    delete input;
}

DECLARE_TEST(test_ImageFillerStep)

#include "test_imagefillerstep.moc"
