#include "autotest.h"
#include "volumefillerstep.h"

#include "image.h"
#include "imagefillerstep.h"
#include "patientfillerinput.h"
#include "series.h"
#include "testingdicomtagreader.h"

using namespace udg;
using namespace testing;

class test_VolumeFillerStep : public QObject {

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

private:
    void checkVolumeNumberInSeries();

};

Q_DECLARE_METATYPE(TestingDICOMTagReader*)
Q_DECLARE_METATYPE(QList<TestingDICOMTagReader*>)
Q_DECLARE_METATYPE(QList<int>)

namespace {

TestingDICOMTagReader* createReader(int i, const QString &modality = "CT", const QString &SOPClassUID = UIDCTImageStorage)
{
    TestingDICOMTagReader *reader = new TestingDICOMTagReader();
    reader->addTag(DICOMPixelData);
    reader->addTag(DICOMSOPClassUID, SOPClassUID);
    reader->addTag(DICOMSOPInstanceUID, i);
    reader->addTag(DICOMModality, modality);
    return reader;
}

}

void test_VolumeFillerStep::fillIndividually_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<TestingDICOMTagReader*>("reader");
    QTest::addColumn<bool>("expectedValue");

    QTest::newRow("null reader") << static_cast<TestingDICOMTagReader*>(NULL) << false;
    QTest::newRow("empty reader") << new TestingDICOMTagReader() << false;
}

void test_VolumeFillerStep::fillIndividually_ShouldReturnExpectedValue()
{
    QFETCH(TestingDICOMTagReader*, reader);
    QFETCH(bool, expectedValue);

    PatientFillerInput input;
    input.setDICOMFile(reader);

    ImageFillerStep imageFillerStep;
    imageFillerStep.setInput(&input);
    imageFillerStep.fillIndividually();

    VolumeFillerStep volumeFillerStep;
    volumeFillerStep.setInput(&input);

    QCOMPARE(volumeFillerStep.fillIndividually(), expectedValue);
}

void test_VolumeFillerStep::fillIndividually_ShouldCreateSeparateVolumesForDifferentSizes_data()
{
    QTest::addColumn<QList<TestingDICOMTagReader*>>("readers");
    QTest::addColumn<QList<int>>("volumeNumberInSeries");

    QList<TestingDICOMTagReader*> readers;
    QList<int> volumeNumberInSeries;

    // all images same size
    {
        readers.clear();
        volumeNumberInSeries.clear();

        for (int i = 0; i < 10; i++)
        {
            TestingDICOMTagReader *reader = createReader(i);
            reader->addTag(DICOMRows, 256);
            reader->addTag(DICOMColumns, 256);
            readers.append(reader);
            volumeNumberInSeries.append(1);
        }

        QTest::newRow("10 images same size") << readers << volumeNumberInSeries;
    }

    // different variations
    {
        readers.clear();
        volumeNumberInSeries.clear();

        for (int i = 0; i < 1; i++)
        {
            TestingDICOMTagReader *reader = createReader(i);
            reader->addTag(DICOMRows, 256);
            reader->addTag(DICOMColumns, 256);
            readers.append(reader);
            volumeNumberInSeries.append(1);
        }

        for (int i = 1; i < 3; i++)
        {
            TestingDICOMTagReader *reader = createReader(i);
            reader->addTag(DICOMRows, 256);
            reader->addTag(DICOMColumns, 512);
            readers.append(reader);
            volumeNumberInSeries.append(2);
        }

        for (int i = 3; i < 6; i++)
        {
            TestingDICOMTagReader *reader = createReader(i);
            reader->addTag(DICOMRows, 512);
            reader->addTag(DICOMColumns, 512);
            readers.append(reader);
            volumeNumberInSeries.append(3);
        }

        for (int i = 6; i < 10; i++)
        {
            TestingDICOMTagReader *reader = createReader(i);
            reader->addTag(DICOMRows, 256);
            reader->addTag(DICOMColumns, 256);
            readers.append(reader);
            volumeNumberInSeries.append(1);
        }

        QTest::newRow("different variations in size") << readers << volumeNumberInSeries;
    }
}

void test_VolumeFillerStep::fillIndividually_ShouldCreateSeparateVolumesForDifferentSizes()
{
    checkVolumeNumberInSeries();
}

void test_VolumeFillerStep::fillIndividually_ShouldCreateSeparateVolumesForDifferentPhotometricInterpretations_data()
{
    QTest::addColumn<QList<TestingDICOMTagReader*>>("readers");
    QTest::addColumn<QList<int>>("volumeNumberInSeries");

    QList<TestingDICOMTagReader*> readers;
    QList<int> volumeNumberInSeries;

    // all images with the same photometric interpretation
    {
        readers.clear();
        volumeNumberInSeries.clear();

        for (int i = 0; i < 10; i++)
        {
            TestingDICOMTagReader *reader = createReader(i);
            reader->addTag(DICOMPhotometricInterpretation, "MONOCHROME2");
            readers.append(reader);
            volumeNumberInSeries.append(1);
        }

        QTest::newRow("10 MONO") << readers << volumeNumberInSeries;
    }

    // 4 + 6
    {
        readers.clear();
        volumeNumberInSeries.clear();

        for (int i = 0; i < 4; i++)
        {
            TestingDICOMTagReader *reader = createReader(i);
            reader->addTag(DICOMPhotometricInterpretation, "MONOCHROME2");
            readers.append(reader);
            volumeNumberInSeries.append(1);
        }

        for (int i = 4; i < 10; i++)
        {
            TestingDICOMTagReader *reader = createReader(i);
            reader->addTag(DICOMPhotometricInterpretation, "RGB");
            readers.append(reader);
            volumeNumberInSeries.append(2);
        }

        QTest::newRow("4 MONO + 6 RGB") << readers << volumeNumberInSeries;
    }

    // 4 + 3 + 3
    {
        readers.clear();
        volumeNumberInSeries.clear();

        for (int i = 0; i < 4; i++)
        {
            TestingDICOMTagReader *reader = createReader(i);
            reader->addTag(DICOMPhotometricInterpretation, "MONOCHROME2");
            readers.append(reader);
            volumeNumberInSeries.append(1);
        }

        for (int i = 4; i < 7; i++)
        {
            TestingDICOMTagReader *reader = createReader(i);
            reader->addTag(DICOMPhotometricInterpretation, "RGB");
            readers.append(reader);
            volumeNumberInSeries.append(2);
        }

        for (int i = 7; i < 10; i++)
        {
            TestingDICOMTagReader *reader = createReader(i);
            reader->addTag(DICOMPhotometricInterpretation, "MONOCHROME2");
            readers.append(reader);
            volumeNumberInSeries.append(1);
        }

        QTest::newRow("4 MONO + 3 RGB + 3 MONO") << readers << volumeNumberInSeries;
    }
}

void test_VolumeFillerStep::fillIndividually_ShouldCreateSeparateVolumesForDifferentPhotometricInterpretations()
{
    checkVolumeNumberInSeries();
}

void test_VolumeFillerStep::fillIndividually_ShouldCreateSeparateVolumesForDifferentSpacings_data()
{
    QTest::addColumn<QList<TestingDICOMTagReader*>>("readers");
    QTest::addColumn<QList<int>>("volumeNumberInSeries");

    QList<TestingDICOMTagReader*> readers;
    QList<int> volumeNumberInSeries;

    // all images with the same spacing
    {
        readers.clear();
        volumeNumberInSeries.clear();

        for (int i = 0; i < 10; i++)
        {
            TestingDICOMTagReader *reader = createReader(i);
            reader->addTag(DICOMPixelSpacing, "1\\1");
            readers.append(reader);
            volumeNumberInSeries.append(1);
        }

        QTest::newRow("10 images same spacing") << readers << volumeNumberInSeries;
    }

    // 4 + 6
    {
        readers.clear();
        volumeNumberInSeries.clear();

        for (int i = 0; i < 4; i++)
        {
            TestingDICOMTagReader *reader = createReader(i);
            reader->addTag(DICOMPixelSpacing, "1\\1");
            readers.append(reader);
            volumeNumberInSeries.append(1);
        }

        for (int i = 4; i < 10; i++)
        {
            TestingDICOMTagReader *reader = createReader(i);
            reader->addTag(DICOMPixelSpacing, "0.5\\0.5");
            readers.append(reader);
            volumeNumberInSeries.append(2);
        }

        QTest::newRow("4 + 6") << readers << volumeNumberInSeries;
    }

    // 4 + 3 + 3
    {
        readers.clear();
        volumeNumberInSeries.clear();

        for (int i = 0; i < 4; i++)
        {
            TestingDICOMTagReader *reader = createReader(i);
            reader->addTag(DICOMPixelSpacing, "1\\1");
            readers.append(reader);
            volumeNumberInSeries.append(1);
        }

        for (int i = 4; i < 7; i++)
        {
            TestingDICOMTagReader *reader = createReader(i);
            reader->addTag(DICOMPixelSpacing, "0.5\\0.5");
            readers.append(reader);
            volumeNumberInSeries.append(2);
        }

        for (int i = 7; i < 10; i++)
        {
            TestingDICOMTagReader *reader = createReader(i);
            reader->addTag(DICOMPixelSpacing, "1\\1");
            readers.append(reader);
            volumeNumberInSeries.append(1);
        }

        QTest::newRow("4 + 3 + 3") << readers << volumeNumberInSeries;
    }
}

void test_VolumeFillerStep::fillIndividually_ShouldCreateSeparateVolumesForDifferentSpacings()
{
    checkVolumeNumberInSeries();
}

void test_VolumeFillerStep::checkVolumeNumberInSeries()
{
    QFETCH(QList<TestingDICOMTagReader*>, readers);
    QFETCH(QList<int>, volumeNumberInSeries);

    Series *series = new Series(this);

    PatientFillerInput input;
    input.setCurrentSeries(series);

    ImageFillerStep imageFillerStep;
    imageFillerStep.setInput(&input);

    VolumeFillerStep volumeFillerStep;
    volumeFillerStep.setInput(&input);

    foreach (TestingDICOMTagReader *reader, readers)
    {
        input.setDICOMFile(reader);
        imageFillerStep.fillIndividually();
        QCOMPARE(volumeFillerStep.fillIndividually(), true);
    }

    for (int i = 0; i < volumeNumberInSeries.size(); i++)
    {
        QCOMPARE(series->getImageByIndex(i)->getVolumeNumberInSeries(), volumeNumberInSeries[i]);
    }
}

DECLARE_TEST(test_VolumeFillerStep)

#include "test_volumefillerstep.moc"
