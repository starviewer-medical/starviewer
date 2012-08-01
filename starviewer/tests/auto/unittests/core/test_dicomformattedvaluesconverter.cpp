#include "autotest.h"
#include "dicomformattedvaluesconverter.h"
#include "windowlevel.h"

#include <QList>

using namespace udg;

class test_DICOMFormattedValuesConverter : public QObject {
Q_OBJECT

private slots:
    void parseWindowLevelValues_ReturnsExpectedValues_data();
    void parseWindowLevelValues_ReturnsExpectedValues();
};

Q_DECLARE_METATYPE(QList<WindowLevel>)

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

DECLARE_TEST(test_DICOMFormattedValuesConverter)

#include "test_dicomformattedvaluesconverter.moc"

