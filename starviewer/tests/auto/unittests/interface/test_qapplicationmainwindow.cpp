#include "autotest.h"
#include "qapplicationmainwindow.h"

using namespace udg;

class test_QApplicationMainWindow : public QObject {
Q_OBJECT

private slots:
    void getCountQApplicationMainWindow_ShoudReturnExpectedNumberOfQApplicationMainWindows();
};

void test_QApplicationMainWindow::getCountQApplicationMainWindow_ShoudReturnExpectedNumberOfQApplicationMainWindows()
{
    unsigned int numOfWindows = 0;
    QSharedPointer<QApplicationMainWindow> newMainWindow(new QApplicationMainWindow(0));
    numOfWindows++;
    QCOMPARE(newMainWindow->getCountQApplicationMainWindow(), numOfWindows);

    QSharedPointer<QApplicationMainWindow> newMainWindow2(new QApplicationMainWindow(0));
    numOfWindows++;

    QApplicationMainWindow *newMainWindow3 = new QApplicationMainWindow(0);
    numOfWindows++;
    QCOMPARE(newMainWindow->getCountQApplicationMainWindow(), numOfWindows);

    delete newMainWindow3;
    numOfWindows--;
    QCOMPARE(newMainWindow->getCountQApplicationMainWindow(), numOfWindows);

    QSharedPointer<QMainWindow> mainWindow(new QMainWindow(0));

    QCOMPARE(newMainWindow->getCountQApplicationMainWindow(), numOfWindows);
}

DECLARE_TEST(test_QApplicationMainWindow)

#include "test_qapplicationmainwindow.moc"


