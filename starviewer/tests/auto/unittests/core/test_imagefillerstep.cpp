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

private:

    static TestingDICOMTagReader* createReader(int i);
    void checkVolumeNumberInSeries();

};

Q_DECLARE_METATYPE(TestingDICOMTagReader*)
Q_DECLARE_METATYPE(QList<TestingDICOMTagReader*>)
Q_DECLARE_METATYPE(QList<int>)

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

TestingDICOMTagReader* test_ImageFillerStep::createReader(int i)
{
    auto *reader = new TestingDICOMTagReader();
    reader->addTag(DICOMPixelData);
    reader->addTag(DICOMSOPClassUID, UIDCTImageStorage);
    reader->addTag(DICOMSOPInstanceUID, i);
    reader->addTag(DICOMModality, "CT");
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
