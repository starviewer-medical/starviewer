#include "qdiagnosistest.h"

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QThread>
#include <QMovie>

#include "diagnosistestfactory.h"
#include "diagnosistest.h"

#include "qdiagnosistestresultwidget.h"

namespace udg {

QDiagnosisTest::QDiagnosisTest(QWidget *parent)
 : QDialog(parent)
{
    setupUi(this);

    updateWidgetToRunDiagnosisTest();

    createConnections();

    QMovie *operationAnimation = new QMovie(this);
    operationAnimation->setFileName(":/images/loader.gif");
    m_animationInProgressLabel->setMovie(operationAnimation);
    operationAnimation->start();
}

void QDiagnosisTest::qdiagnosisTestResultWidgetResized()
{
    m_testsResultsTable->resizeRowsToContents();
}

void QDiagnosisTest::runDiagnosisTest()
{
    QList<DiagnosisTest*> diagnosisTestsToRun = getDiagnosisTestsToRun();
    m_animationInProgressLabel->setVisible(true);

    m_testsProgressBar->setValue(0);
    m_testsProgressBar->setMaximum(diagnosisTestsToRun.count());
    QThread *threadRunningTests = new QThread();

    foreach(DiagnosisTest *diagnosisTest, diagnosisTestsToRun)
    {
        m_executingTestLabel->setText(tr("Running test: ") + diagnosisTest->getDescription());
        m_executingTestLabel->repaint();
        this->repaint();

        diagnosisTest->moveToThread(threadRunningTests);
        DiagnosisTestResult diagnosisTestResult = diagnosisTest->run();
        m_testsProgressBar->setValue(m_testsProgressBar->value() + 1);

        QPair<DiagnosisTest *, DiagnosisTestResult> executedTest;
        executedTest.first = diagnosisTest;
        executedTest.second = diagnosisTestResult;

        if (diagnosisTestResult.getState() == DiagnosisTestResult::Ok)
        {
            m_okExecutedDiagnosisTests.append(executedTest);
        }
        else if (diagnosisTestResult.getState() == DiagnosisTestResult::Warning)
        {
            m_warningExecutedDiagnosisTests.append(executedTest);
        }
        else if (diagnosisTestResult.getState() == DiagnosisTestResult::Error || diagnosisTestResult.getState() == DiagnosisTestResult::Invalid)
        {
            m_errorExecutedDiagnosisTests.append(executedTest);
        }
    }

    showResults();
} 

void QDiagnosisTest::viewTestsLabelClicked()
{
    m_diagnosisTestsResultsFrame->setVisible(true);

    m_succeededTestsToolButton->setChecked(true);
    fillDiagnosisTestsResultTable();

    this->adjustSize();
}

void QDiagnosisTest::createConnections()
{
    connect(m_runDiagnosisTestButton, SIGNAL(clicked()), SLOT(runDiagnosisTest()));

    connect(m_succeededTestsToolButton, SIGNAL(clicked()), SLOT(fillDiagnosisTestsResultTable()));
    connect(m_warningTestsToolButton, SIGNAL(clicked()), SLOT(fillDiagnosisTestsResultTable()));
    connect(m_errorTestsToolButton, SIGNAL(clicked()), SLOT(fillDiagnosisTestsResultTable()));
    connect(m_closeButton, SIGNAL(clicked()), SLOT(accept()));
    
    connect(m_viewTestsLabel, SIGNAL(linkActivated(QString)), SLOT(viewTestsLabelClicked()));
}

void QDiagnosisTest::updateWidgetToRunDiagnosisTest()
{
    m_diagnosisTestsResultsFrame->setVisible(false);
    m_allTestSuccededFrame->setVisible(false);
    m_closeButtonFrame->setVisible(false);

    this->adjustSize();
}

void QDiagnosisTest::showResults()
{
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

}
