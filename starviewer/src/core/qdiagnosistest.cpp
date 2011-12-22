#include "qdiagnosistest.h"

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QThread>
#include <QMovie>

#include "diagnosistestfactory.h"
#include "diagnosistest.h"
#include "rundiagnosistest.h"

#include "qdiagnosistestresultwidget.h"

namespace udg {

QDiagnosisTest::QDiagnosisTest(QWidget *parent)
 : QDialog(parent)
{
    setupUi(this);

    m_threadRunningDiagnosisTest = new QThread();
    m_threadRunningDiagnosisTest->start();

    m_runDiagnosisTest = new RunDiagnosisTest(getDiagnosisTestsToRun());
    m_runDiagnosisTest->moveToThread(m_threadRunningDiagnosisTest);

    createConnections();

    QMovie *operationAnimation = new QMovie(this);
    operationAnimation->setFileName(":/images/loader.gif");
    m_animationInProgressLabel->setMovie(operationAnimation);
    operationAnimation->start();
}

QDiagnosisTest::~QDiagnosisTest()
{
    qDeleteAll(m_runDiagnosisTest->getDiagnosisTestToRun());
    delete m_runDiagnosisTest;

    m_threadRunningDiagnosisTest->terminate();
    m_threadRunningDiagnosisTest->wait();
    delete m_threadRunningDiagnosisTest;
}

void QDiagnosisTest::execAndRunDiagnosisTest()
{
    this->show();
    runDiagnosisTest();
    this->exec();
}

void QDiagnosisTest::createConnections()
{
    connect(m_succeededTestsToolButton, SIGNAL(clicked()), SLOT(fillDiagnosisTestsResultTable()));
    connect(m_warningTestsToolButton, SIGNAL(clicked()), SLOT(fillDiagnosisTestsResultTable()));
    connect(m_errorTestsToolButton, SIGNAL(clicked()), SLOT(fillDiagnosisTestsResultTable()));
    connect(m_closeButton, SIGNAL(clicked()), SLOT(accept()));
    
    connect(m_viewTestsLabel, SIGNAL(linkActivated(QString)), SLOT(viewTestsLabelClicked()));

    connect(this, SIGNAL(start()), m_runDiagnosisTest, SLOT(run()));
    connect(m_runDiagnosisTest, SIGNAL(runningDiagnosisTest(DiagnosisTest *)), this, SLOT(updateRunningDiagnosisTestProgress(DiagnosisTest *)));
    connect(m_runDiagnosisTest, SIGNAL(finished()), this, SLOT(finishedRunningDiagnosisTest()));
}

void QDiagnosisTest::qdiagnosisTestResultWidgetResized()
{
    m_testsResultsTable->resizeRowsToContents();
}

void QDiagnosisTest::runDiagnosisTest()
{
    updateWidgetToRunDiagnosisTest();

    m_testsProgressBar->setValue(0);
    m_testsProgressBar->setMaximum(m_runDiagnosisTest->getDiagnosisTestToRun().count());

    emit start();
} 

void QDiagnosisTest::updateRunningDiagnosisTestProgress(DiagnosisTest * diagnosisTest)
{
    m_executingTestLabel->setText(tr("Running test: ") + diagnosisTest->getDescription());
    m_testsProgressBar->setValue(m_testsProgressBar->value() + 1);
}

void QDiagnosisTest::finishedRunningDiagnosisTest()
{
    groupDiagnosisTestFromRunDiagnosisTestByState();

    m_errorTestsToolButton->setText(tr("%1 errors").arg(m_errorExecutedDiagnosisTests.count()));
    m_succeededTestsToolButton->setText(tr("%1 Ok").arg(m_okExecutedDiagnosisTests.count()));
    m_warningTestsToolButton->setText(tr("%1 warnings").arg(m_warningExecutedDiagnosisTests.count()));

    if (allDiagnosisTestResultAreOk())
    {
        m_allTestSuccededFrame->setVisible(true);
    }
    else
    {
        //Marquem els botons de warning i error perqué en el llistat apareguin només els testos que han fallat 
        m_warningTestsToolButton->setChecked(true);
        m_errorTestsToolButton->setChecked(true);
        
        m_diagnosisTestsResultsFrame->setVisible(true);
        fillDiagnosisTestsResultTable();
    }

    m_progressBarFrame->setVisible(false);
    m_closeButtonFrame->setVisible(true);

    this->adjustSize();
}

void QDiagnosisTest::viewTestsLabelClicked()
{
    m_diagnosisTestsResultsFrame->setVisible(true);

    m_succeededTestsToolButton->setChecked(true);
    fillDiagnosisTestsResultTable();

    this->adjustSize();
}

void QDiagnosisTest::fillDiagnosisTestsResultTable()
{
    m_testsResultsTable->setColumnWidth(0, m_testsResultsTable->width());

    m_testsResultsTable->clear();
    m_testsResultsTable->setRowCount(0);

    QList<QPair<DiagnosisTest *,DiagnosisTestResult> > testsToShow = getDiagnosisTestsToShowInDiagnosisTestsResultTable();

    for (int index = 0; index < testsToShow.count(); index++)
    {
        QPair<DiagnosisTest *, DiagnosisTestResult> executedTest = testsToShow.at(index);
        addDiagnosisTestResultToTable(executedTest.first, executedTest.second);
    }
    
    m_testsResultsTable->resizeRowsToContents();
}

void QDiagnosisTest::addDiagnosisTestResultToTable(DiagnosisTest *diagnosisTest, DiagnosisTestResult diagnosisTestResult)
{
    QDiagnosisTestResultWidget *qdiagnosisTestResultWidget = new QDiagnosisTestResultWidget(diagnosisTest, diagnosisTestResult);
    connect(qdiagnosisTestResultWidget, SIGNAL(resized()), SLOT(qdiagnosisTestResultWidgetResized()));

    m_testsResultsTable->setRowCount(m_testsResultsTable->rowCount() + 1);
    m_testsResultsTable->setCellWidget(m_testsResultsTable->rowCount() -1, 0, qdiagnosisTestResultWidget);
}

void QDiagnosisTest::updateWidgetToRunDiagnosisTest()
{
    m_diagnosisTestsResultsFrame->setVisible(false);
    m_allTestSuccededFrame->setVisible(false);
    m_closeButtonFrame->setVisible(false);

    this->adjustSize();
}

bool QDiagnosisTest::allDiagnosisTestResultAreOk()
{
    return m_errorExecutedDiagnosisTests.count() == 0 && m_warningExecutedDiagnosisTests.count() == 0;
}

QList<DiagnosisTest*> QDiagnosisTest::getDiagnosisTestsToRun() const
{
    QList<DiagnosisTest*> diagnosisTestsToRun;

    foreach(QString className, DiagnosisTestFactory::instance()->getFactoryNamesList())
    {
        diagnosisTestsToRun.append(DiagnosisTestFactory::instance()->create(className));
    }

    return diagnosisTestsToRun;
}

QList<QPair<DiagnosisTest *,DiagnosisTestResult> > QDiagnosisTest::getDiagnosisTestsToShowInDiagnosisTestsResultTable()
{
    QList<QPair<DiagnosisTest *,DiagnosisTestResult> > testsToShow;
    
    if (m_warningTestsToolButton->isChecked())
    {
        testsToShow.append(m_warningExecutedDiagnosisTests);
    }
    
    if (m_errorTestsToolButton->isChecked())
    {
        testsToShow.append(m_errorExecutedDiagnosisTests);
    }
    
    if (m_succeededTestsToolButton->isChecked())
    {
        testsToShow.append(m_okExecutedDiagnosisTests);
    }

    return testsToShow;
}

void QDiagnosisTest::groupDiagnosisTestFromRunDiagnosisTestByState()
{
    QList<QPair<DiagnosisTest *,DiagnosisTestResult> >  runDiagnosisTests = m_runDiagnosisTest->getRunTests();

    for (int index = 0; index < runDiagnosisTests.count(); index++)
    {
        QPair<DiagnosisTest *, DiagnosisTestResult> runDiagnosisTest = runDiagnosisTests.at(index);

        if (runDiagnosisTest.second.getState() == DiagnosisTestResult::Ok)
        {
            m_okExecutedDiagnosisTests.append(runDiagnosisTest);
        }
        else if (runDiagnosisTest.second.getState() == DiagnosisTestResult::Warning)
        {
            m_warningExecutedDiagnosisTests.append(runDiagnosisTest);
        }
        else
        {
            m_errorExecutedDiagnosisTests.append(runDiagnosisTest);
        }
    }
}

}
