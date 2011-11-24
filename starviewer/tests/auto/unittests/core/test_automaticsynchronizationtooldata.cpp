#include "autotest.h"
#include "automaticsynchronizationtooldata.h"
#include <iostream>

using namespace udg;

class test_AutomaticSynchronizationToolData : public QObject {
Q_OBJECT

private slots:

void setPosition_Works_data();
void setPosition_Works();
void addAndQuitGroup_Works();

};

Q_DECLARE_METATYPE(double*)

void test_AutomaticSynchronizationToolData::setPosition_Works_data()
{
    QTest::addColumn<QString>("frameOfReferenceUID_1");
    QTest::addColumn<QString>("view_1");
    QTest::addColumn<double*>("position_1");

    QTest::addColumn<QString>("frameOfReferenceUID_2");
    QTest::addColumn<QString>("view_2");
    QTest::addColumn<double*>("position_2");

    QTest::addColumn<double*>("result");

    double *position_1 = new double[3];
    position_1[0] = 1.0; 
    position_1[1] = 2.0;
    position_1[2] = 3.0;

    double *position_2 = new double[3];
    position_2[0] = 10.0; 
    position_2[1] = 20.0;
    position_2[2] = 30.0;

    QTest::newRow("same UID, same view") << "1" << "VIEW_1" << position_1 << "1" << "VIEW_1" << position_2; 
    QTest::newRow("same UID, different view") << "1" << "VIEW_1" << position_1 << "1" << "VIEW_2" << position_2;
    QTest::newRow("different UID, same view") << "1" << "VIEW_1" << position_1 << "2" << "VIEW_1" << position_2;
    QTest::newRow("different UID, different view") << "1" << "VIEW_1" << position_1 << "2" << "VIEW_2" << position_2;
}

void test_AutomaticSynchronizationToolData::setPosition_Works()
{
    QFETCH(QString, frameOfReferenceUID_1);
    QFETCH(QString, view_1);
    QFETCH(double*, position_1);
    QFETCH(QString, frameOfReferenceUID_2);
    QFETCH(QString, view_2);
    QFETCH(double*, position_2);

    AutomaticSynchronizationToolData *toolData = new AutomaticSynchronizationToolData();

    toolData->setPosition(frameOfReferenceUID_1, view_1, position_1, 0);
    toolData->setPosition(frameOfReferenceUID_2, view_2, position_2, 0);

    QCOMPARE(toolData->hasPosition(frameOfReferenceUID_1, view_1), true);
    QCOMPARE(toolData->hasPosition(frameOfReferenceUID_2, view_2), true);

    double *savedPosition = toolData->getPosition(frameOfReferenceUID_2, view_2);
    QCOMPARE(savedPosition[0], position_2[0]);
    QCOMPARE(savedPosition[1], position_2[1]);
    QCOMPARE(savedPosition[2], position_2[2]);

    delete toolData;
}

void test_AutomaticSynchronizationToolData::addAndQuitGroup_Works()
{
    AutomaticSynchronizationToolData *toolData = new AutomaticSynchronizationToolData();

    toolData->setGroupForUID("1", 1);
    toolData->setGroupForUID("2", 1);

    QCOMPARE(toolData->getGroupForUID("1"), 1);
    QCOMPARE(toolData->getGroupForUID("2"), 1);

    toolData->setGroupForUID("2", 2);

    QCOMPARE(toolData->getGroupForUID("1"), 1);
    QCOMPARE(toolData->getGroupForUID("2"), 2);
}

DECLARE_TEST(test_AutomaticSynchronizationToolData)

#include "test_automaticsynchronizationtooldata.moc"
