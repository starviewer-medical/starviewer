#include "autotest.h"
#include "dicomformattedvaluesconverter.h"

#include "voilut.h"

using namespace udg;

class test_DICOMFormattedValuesConverter : public QObject {
Q_OBJECT

private slots:
    void parseWindowLevelValues_ReturnsExpectedValues_data();
    void parseWindowLevelValues_ReturnsExpectedValues();

    void parseVoiLut_ReturnsExpectedVoiLut_data();
    void parseVoiLut_ReturnsExpectedVoiLut();

};

Q_DECLARE_METATYPE(QList<WindowLevel>)
Q_DECLARE_METATYPE(VoiLut)

void test_DICOMFormattedValuesConverter::parseWindowLevelValues_ReturnsExpectedValues_data()
{
    QTest::addColumn<QString>("windowWidthString");
    QTest::addColumn<QString>("windowCenterString");
    QTest::addColumn<QString>("explanationString");
    QTest::addColumn<QList<WindowLevel> >("expectedResult");

    QTest::newRow("Empty strings (All)") << QString() << QString() << QString() << QList<WindowLevel>();

    WindowLevel wl1(1.0, 3.0);
    
    QList<WindowLevel> windowLevelList;
    windowLevelList << wl1;
    QTest::newRow("Different number of ww/wl (1 ww, 2 wl)") << "1.0" << "3.0\\2.0" << QString() << windowLevelList;
    QTest::newRow("Different number of ww/wl (2 ww, 1 wl)") << "1.0\\22.6" << "3.0" << QString() << windowLevelList;
    QTest::newRow("Different number of ww/wl (0 ww, 2 wl)") << QString() << "1.0\\2.0" << QString() << QList<WindowLevel>();
    QTest::newRow("Different number of ww/wl (1 ww, 0 wl)") << "1.0" << QString() << QString() << QList<WindowLevel>();
    QTest::newRow("WW is not a decimal string - 1 element") << "absjkh" << "1.0\\2.0" << QString() << QList<WindowLevel>();
    QTest::newRow("WW is not a decimal string - 2 element") << "1.0\\absjkh" << "1.0\\2.0" << QString() << QList<WindowLevel>();
    QTest::newRow("WL is not a decimal string - 1 element") << "1.0" << "378jnso2" << QString() << QList<WindowLevel>();
    QTest::newRow("WL is not a decimal string - 2 element") << "1.0" << "4.3\\abc2do2" << QString() << QList<WindowLevel>();
    
    WindowLevel wl2(22.6, 44.3);
    
    windowLevelList.clear();
    windowLevelList << wl1 << wl2;
    QTest::newRow("Same number of WW & WL") << "1.0\\22.6" << "3.0\\44.3" << QString() << windowLevelList;
    
    windowLevelList.clear();
    wl1.setName("WINDOW 1");
    windowLevelList << wl1 << wl2;
    QTest::newRow("Same number of WW & WL, less number of explanations") << "1.0\\22.6" << "3.0\\44.3" << "WINDOW 1" << windowLevelList;
    
    windowLevelList.clear();
    wl2.setName("WINDOW 2");
    windowLevelList << wl1 << wl2;
    QTest::newRow("Same number of WW & WL, same number of explanations") << "1.0\\22.6" << "3.0\\44.3" << "WINDOW 1\\WINDOW 2" << windowLevelList;

    QTest::newRow("Same number of WW & WL, more number of explanations") << "1.0\\22.6" << "3.0\\44.3" << "WINDOW 1\\WINDOW 2\\WINDOW 3" << windowLevelList;
}

void test_DICOMFormattedValuesConverter::parseWindowLevelValues_ReturnsExpectedValues()
{
    QFETCH(QString, windowWidthString);
    QFETCH(QString, windowCenterString);
    QFETCH(QString, explanationString);
    QFETCH(QList<WindowLevel>, expectedResult);

    QList<WindowLevel> windowLevelList = DICOMFormattedValuesConverter::parseWindowLevelValues(windowWidthString, windowCenterString, explanationString);

    QCOMPARE(windowLevelList.count(), expectedResult.count());
    for (int i = 0; i < windowLevelList.count(); ++i)
    {
        WindowLevel currentWL = windowLevelList.at(i);
        WindowLevel expectedWL = expectedResult.at(i);
        
        QCOMPARE(currentWL, expectedWL);
    }
}

void test_DICOMFormattedValuesConverter::parseVoiLut_ReturnsExpectedVoiLut_data()
{
    QTest::addColumn<QString>("lutDescriptor");
    QTest::addColumn<QString>("lutExplanation");
    QTest::addColumn<QString>("lutData");
    QTest::addColumn<VoiLut>("expectedVoiLut");

    QString lutData = "189\\219\\3\\237\\61\\193\\20\\106\\202\\237\\94\\87\\200\\230\\95\\7\\164\\54\\117\\222";
    TransferFunction lut;
    lut.setName("dec");
    lut.setOpacity(0.0, 1.0);
    lut.setColor(0, 189 / 255.0, 189 / 255.0, 189 / 255.0);
    lut.setColor(142, 189 / 255.0, 189 / 255.0, 189 / 255.0);
    lut.setColor(143, 219 / 255.0, 219 / 255.0, 219 / 255.0);
    lut.setColor(144,   3 / 255.0,   3 / 255.0,   3 / 255.0);
    lut.setColor(145, 237 / 255.0, 237 / 255.0, 237 / 255.0);
    lut.setColor(146,  61 / 255.0,  61 / 255.0,  61 / 255.0);
    lut.setColor(147, 193 / 255.0, 193 / 255.0, 193 / 255.0);
    lut.setColor(148,  20 / 255.0,  20 / 255.0,  20 / 255.0);
    lut.setColor(149, 106 / 255.0, 106 / 255.0, 106 / 255.0);
    lut.setColor(150, 202 / 255.0, 202 / 255.0, 202 / 255.0);
    lut.setColor(151, 237 / 255.0, 237 / 255.0, 237 / 255.0);
    lut.setColor(152,  94 / 255.0,  94 / 255.0,  94 / 255.0);
    lut.setColor(153,  87 / 255.0,  87 / 255.0,  87 / 255.0);
    lut.setColor(154, 200 / 255.0, 200 / 255.0, 200 / 255.0);
    lut.setColor(155, 230 / 255.0, 230 / 255.0, 230 / 255.0);
    lut.setColor(156,  95 / 255.0,  95 / 255.0,  95 / 255.0);
    lut.setColor(157,   7 / 255.0,   7 / 255.0,   7 / 255.0);
    lut.setColor(158, 164 / 255.0, 164 / 255.0, 164 / 255.0);
    lut.setColor(159,  54 / 255.0,  54 / 255.0,  54 / 255.0);
    lut.setColor(160, 117 / 255.0, 117 / 255.0, 117 / 255.0);
    lut.setColor(161, 222 / 255.0, 222 / 255.0, 222 / 255.0);
    lut.setColor(255, 222 / 255.0, 222 / 255.0, 222 / 255.0);
    QTest::newRow("decimal") << "20\\142\\8" << "dec" << lutData << VoiLut(lut);

    lutData = "04\\7e\\76\\0a\\78\\cb\\c2\\4b\\44\\4f\\e2\\59\\fd\\d5\\23\\21\\5c\\54\\36\\93\\97\\f7\\29\\e5\\44";
    lut.setName("hex");
    lut.clearColor();
    lut.setColor(0, 0x04 / 255.0, 0x04 / 255.0, 0x04 / 255.0);
    lut.setColor(223, 0x04 / 255.0, 0x04 / 255.0, 0x04 / 255.0);
    lut.setColor(224, 0x7e / 255.0, 0x7e / 255.0, 0x7e / 255.0);
    lut.setColor(225, 0x76 / 255.0, 0x76 / 255.0, 0x76 / 255.0);
    lut.setColor(226, 0x0a / 255.0, 0x0a / 255.0, 0x0a / 255.0);
    lut.setColor(227, 0x78 / 255.0, 0x78 / 255.0, 0x78 / 255.0);
    lut.setColor(228, 0xcb / 255.0, 0xcb / 255.0, 0xcb / 255.0);
    lut.setColor(229, 0xc2 / 255.0, 0xc2 / 255.0, 0xc2 / 255.0);
    lut.setColor(230, 0x4b / 255.0, 0x4b / 255.0, 0x4b / 255.0);
    lut.setColor(231, 0x44 / 255.0, 0x44 / 255.0, 0x44 / 255.0);
    lut.setColor(232, 0x4f / 255.0, 0x4f / 255.0, 0x4f / 255.0);
    lut.setColor(233, 0xe2 / 255.0, 0xe2 / 255.0, 0xe2 / 255.0);
    lut.setColor(234, 0x59 / 255.0, 0x59 / 255.0, 0x59 / 255.0);
    lut.setColor(235, 0xfd / 255.0, 0xfd / 255.0, 0xfd / 255.0);
    lut.setColor(236, 0xd5 / 255.0, 0xd5 / 255.0, 0xd5 / 255.0);
    lut.setColor(237, 0x23 / 255.0, 0x23 / 255.0, 0x23 / 255.0);
    lut.setColor(238, 0x21 / 255.0, 0x21 / 255.0, 0x21 / 255.0);
    lut.setColor(239, 0x5c / 255.0, 0x5c / 255.0, 0x5c / 255.0);
    lut.setColor(240, 0x54 / 255.0, 0x54 / 255.0, 0x54 / 255.0);
    lut.setColor(241, 0x36 / 255.0, 0x36 / 255.0, 0x36 / 255.0);
    lut.setColor(242, 0x93 / 255.0, 0x93 / 255.0, 0x93 / 255.0);
    lut.setColor(243, 0x97 / 255.0, 0x97 / 255.0, 0x97 / 255.0);
    lut.setColor(244, 0xf7 / 255.0, 0xf7 / 255.0, 0xf7 / 255.0);
    lut.setColor(245, 0x29 / 255.0, 0x29 / 255.0, 0x29 / 255.0);
    lut.setColor(246, 0xe5 / 255.0, 0xe5 / 255.0, 0xe5 / 255.0);
    lut.setColor(247, 0x44 / 255.0, 0x44 / 255.0, 0x44 / 255.0);
    lut.setColor(255, 0x44 / 255.0, 0x44 / 255.0, 0x44 / 255.0);
    QTest::newRow("hexadecimal") << "25\\223\\8" << "hex" << lutData << VoiLut(lut);
}

void test_DICOMFormattedValuesConverter::parseVoiLut_ReturnsExpectedVoiLut()
{
    QFETCH(QString, lutDescriptor);
    QFETCH(QString, lutExplanation);
    QFETCH(QString, lutData);
    QFETCH(VoiLut, expectedVoiLut);

    QCOMPARE(DICOMFormattedValuesConverter::parseVoiLut(lutDescriptor, lutExplanation, lutData), expectedVoiLut);
}

DECLARE_TEST(test_DICOMFormattedValuesConverter)

#include "test_dicomformattedvaluesconverter.moc"
