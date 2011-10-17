#include "autotest.h"
#include "qapplicationmainwindow.h"

using namespace udg;

class test_QApplicationMainWindow : public QObject {
Q_OBJECT

private slots:
    void getCountQApplicationMainWindow_ShoudReturnExpectedNumberOfQApplicationMainWindows();
    void getQApplicationMainWindows_ShouldReturnExpectedQApplicationMainWindowList();

private:
    int countSameMainWindowObjects(const QList<QApplicationMainWindow*> &originalObjects, const QList<QApplicationMainWindow*> &gottenObjects);
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

void test_QApplicationMainWindow::getQApplicationMainWindows_ShouldReturnExpectedQApplicationMainWindowList()
{
    QList<QApplicationMainWindow*> mainWindows;

    QCOMPARE(QApplicationMainWindow::getQApplicationMainWindows().count(), 0);

    QSharedPointer<QApplicationMainWindow> mainWindow(new QApplicationMainWindow(0));
    QSharedPointer<QApplicationMainWindow> mainWindow2(new QApplicationMainWindow(0));
    QSharedPointer<QMainWindow> mainWindow3(new QMainWindow(0));

    mainWindows << mainWindow.data();
    mainWindows << mainWindow2.data();

    QCOMPARE(QApplicationMainWindow::getQApplicationMainWindows().count(), mainWindows.count());
    QCOMPARE(countSameMainWindowObjects(mainWindows, QApplicationMainWindow::getQApplicationMainWindows()), mainWindows.count());
}

int test_QApplicationMainWindow::countSameMainWindowObjects(const QList<QApplicationMainWindow*> &originalObjects, const QList<QApplicationMainWindow*> &gottenObjects)
{
    int count = 0;
    foreach(QApplicationMainWindow *mainWindow, originalObjects)
    {
        if (gottenObjects.contains(mainWindow))
        {
            count++;
        }
    }
    return count;
}

DECLARE_TEST(test_QApplicationMainWindow)

#include "test_qapplicationmainwindow.moc"


