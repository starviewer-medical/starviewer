#include "autotest.h"
#include "voilut.h"

using namespace udg;

class test_VoiLut : public QObject {
    Q_OBJECT

private slots:
    void constructor_ShouldCreateDefaultWindowLevel();

    void constructor_ShouldCreateGivenWindowLevelAndEmptyLut_data();
    void constructor_ShouldCreateGivenWindowLevelAndEmptyLut();

    void constructor_ShouldCreateGivenLutAndExpectedWindowLevel_data();
    void constructor_ShouldCreateGivenLutAndExpectedWindowLevel();

    void setWindowLevel_ShouldSetGivenWindowLevelAndEmptyLut_data();
    void setWindowLevel_ShouldSetGivenWindowLevelAndEmptyLut();

    void setLut_ShouldSetGivenLutAndExpectedWindowLevel_data();
    void setLut_ShouldSetGivenLutAndExpectedWindowLevel();

    void getExplanation_ShouldReturnExpectedExplanation_data();
    void getExplanation_ShouldReturnExpectedExplanation();

    void setExplanation_ShouldSetNameOnWindowLevelAndVoiLut();

    void isWindowLevel_ShouldReturnExpectedValue_data();
    void isWindowLevel_ShouldReturnExpectedValue();

    void isLut_ShouldReturnExpectedValue_data();
    void isLut_ShouldReturnExpectedValue();

    void operatorEquals_ShouldReturnExpectedValue_data();
    void operatorEquals_ShouldReturnExpectedValue();

    void operatorNotEqual_ShouldReturnExpectedValue_data();
    void operatorNotEqual_ShouldReturnExpectedValue();

    void inverse_ShouldReturnExpectedValue_data();
    void inverse_ShouldReturnExpectedValue();

private:
    void setupSetWindowLevelData();
    void setupSetLutData();
    void setupIsTypeData();
    void setupComparisonData();

};

Q_DECLARE_METATYPE(TransferFunction)
Q_DECLARE_METATYPE(VoiLut)
Q_DECLARE_METATYPE(WindowLevel)

void test_VoiLut::constructor_ShouldCreateDefaultWindowLevel()
{
    VoiLut voiLut;

    QCOMPARE(voiLut, VoiLut(WindowLevel()));
}

void test_VoiLut::constructor_ShouldCreateGivenWindowLevelAndEmptyLut_data()
{
    setupSetWindowLevelData();
}

void test_VoiLut::constructor_ShouldCreateGivenWindowLevelAndEmptyLut()
{
    QFETCH(WindowLevel, windowLevel);

    VoiLut voiLut(windowLevel);

    QCOMPARE(voiLut.getWindowLevel(), windowLevel);
    QCOMPARE(voiLut.getLut(), TransferFunction());
}

void test_VoiLut::constructor_ShouldCreateGivenLutAndExpectedWindowLevel_data()
{
    setupSetLutData();
}

void test_VoiLut::constructor_ShouldCreateGivenLutAndExpectedWindowLevel()
{
    QFETCH(TransferFunction, lut);
    QFETCH(WindowLevel, expectedWindowLevel);

    VoiLut voiLut(lut);

    QCOMPARE(voiLut.getLut(), lut);
    QCOMPARE(voiLut.getWindowLevel(), expectedWindowLevel);
}

void test_VoiLut::setWindowLevel_ShouldSetGivenWindowLevelAndEmptyLut_data()
{
    setupSetWindowLevelData();
}

void test_VoiLut::setWindowLevel_ShouldSetGivenWindowLevelAndEmptyLut()
{
    QFETCH(WindowLevel, windowLevel);

    VoiLut voiLut;
    voiLut.setWindowLevel(windowLevel);

    QCOMPARE(voiLut.getWindowLevel(), windowLevel);
    QCOMPARE(voiLut.getLut(), TransferFunction());
}

void test_VoiLut::setLut_ShouldSetGivenLutAndExpectedWindowLevel_data()
{
    setupSetLutData();
}

void test_VoiLut::setLut_ShouldSetGivenLutAndExpectedWindowLevel()
{
    QFETCH(TransferFunction, lut);
    QFETCH(WindowLevel, expectedWindowLevel);

    VoiLut voiLut;
    voiLut.setLut(lut);

    QCOMPARE(voiLut.getLut(), lut);
    QCOMPARE(voiLut.getWindowLevel(), expectedWindowLevel);
}

void test_VoiLut::getExplanation_ShouldReturnExpectedExplanation_data()
{
    QTest::addColumn<VoiLut>("voiLut");
    QTest::addColumn<QString>("expectedExplanation");

    WindowLevel windowLevel(1, 0, "window level");
    TransferFunction lut;
    lut.setName("lut");
    lut.set(0, 0, 0, 0, 0);
    lut.set(1, 1, 1, 1, 1);

    QTest::newRow("default") << VoiLut() << "";
    QTest::newRow("window level") << VoiLut(windowLevel) << windowLevel.getName();
    QTest::newRow("lut") << VoiLut(lut) << lut.name();

    VoiLut voiLut(windowLevel);
    voiLut.setLut(lut);
    QTest::newRow("window level, then lut") << voiLut << lut.name();

    voiLut.setWindowLevel(windowLevel);
    QTest::newRow("lut, then window level") << voiLut << windowLevel.getName();

    voiLut.setExplanation("explanation");
    QTest::newRow("set explanation") << voiLut << "explanation";
}

void test_VoiLut::getExplanation_ShouldReturnExpectedExplanation()
{
    QFETCH(VoiLut, voiLut);
    QFETCH(QString, expectedExplanation);

    QCOMPARE(voiLut.getExplanation(), expectedExplanation);
}

void test_VoiLut::setExplanation_ShouldSetNameOnWindowLevelAndVoiLut()
{
    VoiLut voiLut;
    voiLut.setExplanation("explanation");

    QCOMPARE(voiLut.getWindowLevel().getName(), QString("explanation"));
    QCOMPARE(voiLut.getLut().name(), QString("explanation"));
}

void test_VoiLut::isWindowLevel_ShouldReturnExpectedValue_data()
{
    setupIsTypeData();
}

void test_VoiLut::isWindowLevel_ShouldReturnExpectedValue()
{
    QFETCH(VoiLut, voiLut);
    QFETCH(bool, isWindowLevel);

    QCOMPARE(voiLut.isWindowLevel(), isWindowLevel);
}

void test_VoiLut::isLut_ShouldReturnExpectedValue_data()
{
    setupIsTypeData();
}

void test_VoiLut::isLut_ShouldReturnExpectedValue()
{
    QFETCH(VoiLut, voiLut);
    QFETCH(bool, isLut);

    QCOMPARE(voiLut.isLut(), isLut);
}

void test_VoiLut::operatorEquals_ShouldReturnExpectedValue_data()
{
    setupComparisonData();
}

void test_VoiLut::operatorEquals_ShouldReturnExpectedValue()
{
    QFETCH(VoiLut, voiLut1);
    QFETCH(VoiLut, voiLut2);
    QFETCH(bool, equal);

    QCOMPARE(voiLut1 == voiLut2, equal);
}

void test_VoiLut::operatorNotEqual_ShouldReturnExpectedValue_data()
{
    setupComparisonData();
}

void test_VoiLut::operatorNotEqual_ShouldReturnExpectedValue()
{
    QFETCH(VoiLut, voiLut1);
    QFETCH(VoiLut, voiLut2);
    QFETCH(bool, notEqual);

    QCOMPARE(voiLut1 != voiLut2, notEqual);
}

void test_VoiLut::inverse_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<VoiLut>("original");
    QTest::addColumn<VoiLut>("inverse");

    WindowLevel originalWindowLevel(100, 50, "wl");
    WindowLevel inverseWindowLevel(-100, 50, "wl");
    QTest::newRow("window level") << VoiLut(originalWindowLevel) << VoiLut(inverseWindowLevel);
    QTest::newRow("window level reverse") << VoiLut(inverseWindowLevel) << VoiLut(originalWindowLevel);

    TransferFunction originalLut;
    originalLut.setName("lut");
    originalLut.setColor(0, 0.0, 0.0, 0.0);
    originalLut.setColor(100, 0.8, 0.8, 0.8);
    originalLut.setColor(200, 0.4, 0.4, 0.4);
    originalLut.setColor(255, 1.0, 1.0, 1.0);
    originalLut.setOpacity(0, 1);
    TransferFunction inverseLut;
    inverseLut.setName("lut");
    inverseLut.setColor(0, 1.0, 1.0, 1.0);
    inverseLut.setColor(100, 0.2, 0.2, 0.2);
    inverseLut.setColor(200, 0.6, 0.6, 0.6);
    inverseLut.setColor(255, 0.0, 0.0, 0.0);
    inverseLut.setOpacity(0, 1);
    QTest::newRow("lut") << VoiLut(originalLut) << VoiLut(inverseLut);
    QTest::newRow("lut reverse") << VoiLut(inverseLut) << VoiLut(originalLut);
}

void test_VoiLut::inverse_ShouldReturnExpectedValue()
{
    QFETCH(VoiLut, original);
    QFETCH(VoiLut, inverse);

    QCOMPARE(original.inverse(), inverse);
}

void test_VoiLut::setupSetWindowLevelData()
{
    QTest::addColumn<WindowLevel>("windowLevel");

    QTest::newRow("default window level") << WindowLevel();
    QTest::newRow("100, 50") << WindowLevel(100, 50);
    QTest::newRow("-2, 8") << WindowLevel(-2, 8);
}

void test_VoiLut::setupSetLutData()
{
    QTest::addColumn<TransferFunction>("lut");
    QTest::addColumn<WindowLevel>("expectedWindowLevel");

    TransferFunction lut;
    lut.set(0, 0, 0, 0, 1);
    lut.set(255, 1, 1, 1, 1);
    QTest::newRow("0 - 255") << lut << WindowLevel(255, 127.5);

    lut.clear();
    lut.set(-100, 30, 30, 30, 1);
    lut.set(40, 200, 200, 200, 1);
    lut.set(500, 0, 0, 0, 1);
    QTest::newRow("-100 - 500") << lut << WindowLevel(600, 200);
}

void test_VoiLut::setupIsTypeData()
{
    QTest::addColumn<VoiLut>("voiLut");
    QTest::addColumn<bool>("isWindowLevel");
    QTest::addColumn<bool>("isLut");

    TransferFunction lut;
    lut.set(0, 0, 0, 0, 0);

    QTest::newRow("default") << VoiLut() << true << false;
    QTest::newRow("window level") << VoiLut(WindowLevel(100, 50)) << true << false;
    QTest::newRow("lut") << VoiLut(lut) << false << true;

    VoiLut voiLut;
    voiLut.setWindowLevel(WindowLevel(1, 2));
    voiLut.setLut(lut);
    QTest::newRow("window level, then lut") << voiLut << false << true;

    voiLut.setWindowLevel(WindowLevel(3, 4));
    QTest::newRow("lut, then window level") << voiLut << true << false;
}

void test_VoiLut::setupComparisonData()
{
    QTest::addColumn<VoiLut>("voiLut1");
    QTest::addColumn<VoiLut>("voiLut2");
    QTest::addColumn<bool>("equal");
    QTest::addColumn<bool>("notEqual");

    WindowLevel windowLevel1(1, 1, "wl1");
    WindowLevel windowLevel2(2, 2, "wl2");
    TransferFunction lut1;
    lut1.setName("lut1");
    lut1.setColor(0, 0, 0, 0);
    lut1.setColor(255, 255, 255, 255);
    lut1.setOpacity(0, 1);
    TransferFunction lut2;
    lut2.setName("lut2");
    lut2.setColor(0, 0, 0, 0);
    lut2.setColor(255, 0.5, 0.5, 0.5);
    lut2.setColor(4095, 1.0, 1.0, 1.0);
    lut2.setOpacity(0, 1);
    TransferFunction lut3(lut1);
    lut3.setColor(100, 0.2, 0.2, 0.2);

    QTest::newRow("same window level") << VoiLut(windowLevel1) << VoiLut(windowLevel1) << true << false;
    QTest::newRow("same lut") << VoiLut(lut1) << VoiLut(lut1) << true << false;
    QTest::newRow("different window level") << VoiLut(windowLevel1) << VoiLut(windowLevel2) << false << true;
    QTest::newRow("different lut") << VoiLut(lut1) << VoiLut(lut2) << false << true;
    QTest::newRow("window level vs lut") << VoiLut(windowLevel2) << VoiLut(lut2) << false << true;
    QTest::newRow("different lut, same window level") << VoiLut(lut1) << VoiLut(lut3) << false << true;

    VoiLut voiLut1(lut1);
    voiLut1.setWindowLevel(windowLevel1);
    VoiLut voiLut2(lut2);
    voiLut2.setWindowLevel(windowLevel1);
    QTest::newRow("start different, then set same window level") << voiLut1 << voiLut2 << true << false;

    VoiLut voiLut3(windowLevel1);
    voiLut3.setLut(lut1);
    VoiLut voiLut4(windowLevel2);
    voiLut4.setLut(lut1);
    QTest::newRow("start different, then set same lut") << voiLut3 << voiLut4 << true << false;

}

DECLARE_TEST(test_VoiLut)

#include "test_voilut.moc"
