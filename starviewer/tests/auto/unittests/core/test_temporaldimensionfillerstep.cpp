#include "autotest.h"
#include "temporaldimensionfillerstep.h"

#include "image.h"
#include "series.h"

#include <functional>

using namespace std;
using namespace udg;

class TestingTemporalDimensionFillerStep : public TemporalDimensionFillerStep {
public:
   using TemporalDimensionFillerStep::VolumeInfo;
   using TemporalDimensionFillerStep::TemporalDimensionInternalInfo;
};

class test_TemporalDimensionFillerStep : public QObject {
Q_OBJECT

private slots:

    void postProcessing_ShouldSetExpectedPhaseNumberWhenAllSlicesHaveTheSameNumberOfPhases_data();
    void postProcessing_ShouldSetExpectedPhaseNumberWhenAllSlicesHaveTheSameNumberOfPhases();

    void postProcessing_ShouldSetSinglePhaseWhenNotAllSlicesHaveTheSameNumberOfPhases_data();
    void postProcessing_ShouldSetSinglePhaseWhenNotAllSlicesHaveTheSameNumberOfPhases();

    void postProcessing_ShouldSetSinglePhaseWhenTheVolumeIsNotProcessed_data();
    void postProcessing_ShouldSetSinglePhaseWhenTheVolumeIsNotProcessed();

private:

    QSharedPointer<TestingTemporalDimensionFillerStep> createStep(int numberOfSlices, const function<int(int)> &numberOfPhasesForSlice);
    void testPostProcessing(const function<int(int)> &expectedPhaseNumber);

};

Q_DECLARE_METATYPE(QSharedPointer<TestingTemporalDimensionFillerStep>)

void test_TemporalDimensionFillerStep::postProcessing_ShouldSetExpectedPhaseNumberWhenAllSlicesHaveTheSameNumberOfPhases_data()
{
    QTest::addColumn<QSharedPointer<TestingTemporalDimensionFillerStep>>("step");
    QTest::addColumn<int>("numberOfPhases");

    QTest::newRow("one slice") << createStep(1, [](int) { return 10; }) << 10;
    QTest::newRow("10 slices") << createStep(10, [](int) { return 12; }) << 12;
}

void test_TemporalDimensionFillerStep::postProcessing_ShouldSetExpectedPhaseNumberWhenAllSlicesHaveTheSameNumberOfPhases()
{
    QFETCH(int, numberOfPhases);

    testPostProcessing([=](int i) { return i % numberOfPhases; });
}

void test_TemporalDimensionFillerStep::postProcessing_ShouldSetSinglePhaseWhenNotAllSlicesHaveTheSameNumberOfPhases_data()
{
    QTest::addColumn<QSharedPointer<TestingTemporalDimensionFillerStep>>("step");

    QTest::newRow("differents numbers of phases per slice") << createStep(10, [](int i) { return i + 1; });
}

void test_TemporalDimensionFillerStep::postProcessing_ShouldSetSinglePhaseWhenNotAllSlicesHaveTheSameNumberOfPhases()
{
    testPostProcessing([](int) { return 0; });
}

void test_TemporalDimensionFillerStep::postProcessing_ShouldSetSinglePhaseWhenTheVolumeIsNotProcessed_data()
{
    QTest::addColumn<QSharedPointer<TestingTemporalDimensionFillerStep>>("step");

    auto step = createStep(10, [](int) { return 10; });
    delete (*step->TemporalDimensionInternalInfo.begin())->take(0); // esborrem el VolumeInfo del volum 0
    QTest::newRow("volume not processed") << step;
}

void test_TemporalDimensionFillerStep::postProcessing_ShouldSetSinglePhaseWhenTheVolumeIsNotProcessed()
{
    testPostProcessing([](int) { return 0; });
}

QSharedPointer<TestingTemporalDimensionFillerStep> test_TemporalDimensionFillerStep::createStep(int numberOfSlices,
                                                                                                const function<int(int)> &numberOfPhasesForSlice)
{
    Series *series = new Series(this);
    auto *volumeInfo = new TestingTemporalDimensionFillerStep::VolumeInfo();
    volumeInfo->numberOfPhases = numberOfPhasesForSlice(0);

    for (int i = 0; i < numberOfSlices; i++)
    {
        int numberOfPhases = numberOfPhasesForSlice(i);

        for (int j = 0; j < numberOfPhases; j++)
        {
            Image *image = new Image();
            image->setVolumeNumberInSeries(0);
            image->setSOPInstanceUID(QString("%1.%2").arg(i).arg(j));
            series->addImage(image);
        }

        volumeInfo->phasesPerPositionHash.insert(QString("0\\0\\%1").arg(i), numberOfPhases);
    }

    auto *volumeHash = new QHash<int, TestingTemporalDimensionFillerStep::VolumeInfo*>();
    volumeHash->insert(0, volumeInfo);

    auto step = QSharedPointer<TestingTemporalDimensionFillerStep>(new TestingTemporalDimensionFillerStep());
    step->TemporalDimensionInternalInfo.insert(series, volumeHash);

    return step;
}

void test_TemporalDimensionFillerStep::testPostProcessing(const function<int(int)> &expectedPhaseNumber)
{
    QFETCH(QSharedPointer<TestingTemporalDimensionFillerStep>, step);

    auto seriesList = step->TemporalDimensionInternalInfo.keys();

    step->postProcessing();

    int i = 0;

    foreach (Series *series, seriesList)
    {
        foreach (Image *image, series->getImages())
        {
            QCOMPARE(image->getPhaseNumber(), expectedPhaseNumber(i));
            i++;
        }
    }
}

DECLARE_TEST(test_TemporalDimensionFillerStep)

#include "test_temporaldimensionfillerstep.moc"
