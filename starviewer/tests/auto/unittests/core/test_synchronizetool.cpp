#include "autotest.h"
#include "synchronizetool.h"
#include "synchronizetooldata.h"
#include "q2dviewer.h"
#include <iostream>

using namespace udg;

class test_SynchronizeTool : public QObject {
Q_OBJECT

private slots:

void setEnable_ActivateAndDeactivateSynchronization_data();
void setEnable_ActivateAndDeactivateSynchronization();

};

Q_DECLARE_METATYPE(Q2DViewer*)

void test_SynchronizeTool::setEnable_ActivateAndDeactivateSynchronization_data()
{
    /*QTest::addColumn<Q2DViewer*>("viewer_1");
    QTest::addColumn<Q2DViewer*>("viewer_2");
    QTest::addColumn<bool>("isSamePosition");

    Q2DViewer *viewer_1 = new Q2DViewer();
    Q2DViewer *viewer_2 = new Q2DViewer();
    Q2DViewer *viewer_3 = new Q2DViewer();

    SynchronizeTool *tool_1 = new SynchronizeTool(viewer_1);
    SynchronizeTool *tool_2 = new SynchronizeTool(viewer_2);
    SynchronizeTool *tool_3 = new SynchronizeTool(viewer_3);
    SynchronizeToolData *data = new SynchronizeToolData();
    tool_1->setToolData(data);
    tool_1->setEnabled(true);
    tool_2->setToolData(data);
    tool_2->setEnabled(true);
    tool_3->setToolData(data);
    tool_3->setEnabled(false);
    viewer_1->setSlice(5);
    viewer_3->setSlice(1);
    
    QTest::newRow("enabled") << viewer_1 << viewer_2 << true; 
    QTest::newRow("disabled") << viewer_1 << viewer_3 << false;*/
}

void test_SynchronizeTool::setEnable_ActivateAndDeactivateSynchronization()
{
    /*QFETCH(Q2DViewer*, viewer_1);
    QFETCH(Q2DViewer*, viewer_2);
    QFETCH(bool, isSamePosition);
    
    QCOMPARE(viewer_1->getCurrentSlice() == viewer_2->getCurrentSlice(), isSamePosition);*/
}

DECLARE_TEST(test_SynchronizeTool)

#include "test_SynchronizeTool.moc"
