#include "autotest.h"
#include "qapplicationmainwindow.h"
#include "patient.h"

using namespace udg;

Q_DECLARE_METATYPE(QSharedPointer<Patient>)

class test_QApplicationMainWindow : public QObject {
Q_OBJECT

private slots:
    void getCountQApplicationMainWindow_ShoudReturnExpectedNumberOfQApplicationMainWindows();
    void getQApplicationMainWindows_ShouldReturnExpectedQApplicationMainWindowList();
    void setPatientInNewWindow_ShouldReturnNewWindowWithExpectedPatient_data();
    void setPatientInNewWindow_ShouldReturnNewWindowWithExpectedPatient();

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

void test_QApplicationMainWindow::setPatientInNewWindow_ShouldReturnNewWindowWithExpectedPatient_data()
{
    QTest::addColumn<QSharedPointer<Patient> >("patient");

    QTest::newRow("null patient") << QSharedPointer<Patient>(0);
    QTest::newRow("default patient") << QSharedPointer<Patient>(new Patient());
}

void test_QApplicationMainWindow::setPatientInNewWindow_ShouldReturnNewWindowWithExpectedPatient()
{
    QFETCH(QSharedPointer<Patient>, patient);

    QSharedPointer<QApplicationMainWindow> mainWindow(new QApplicationMainWindow(0));
    QList<QApplicationMainWindow*> mainApps = QApplicationMainWindow::getQApplicationMainWindows();
    QApplicationMainWindow *newWindow = mainWindow->setPatientInNewWindow(patient.data());

    QVERIFY(!mainApps.contains(newWindow));
    QCOMPARE(newWindow->getCurrentPatient(), patient.data());
}

DECLARE_TEST(test_QApplicationMainWindow)

#include "test_qapplicationmainwindow.moc"


