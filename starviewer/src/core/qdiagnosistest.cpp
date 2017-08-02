/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "qdiagnosistest.h"

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QThread>
#include <QMovie>
#include <QFileDialog>
#include <QScrollBar>
#include <QWebEngineHistory>

#include "diagnosistestfactory.h"
#include "diagnosistest.h"
#include "rundiagnosistest.h"
#include "diagnosistestresultwriter.h"

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
    operationAnimation->setFileName(":/images/animations/loader.gif");
    m_animationInProgressLabel->setMovie(operationAnimation);
    operationAnimation->start();

    //Treiem icona amb ? que apareix al costat del botó de tancar
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    
    m_diagnosisTestsResults->setContextMenuPolicy(Qt::NoContextMenu);
}

QDiagnosisTest::~QDiagnosisTest()
{
    // TODO Should finish in a cleaner way. Calling terminate() is dangerous.
    //      Instead, should stop running the tests (in particular, cancel PACS echoes) and call m_threadRunningDiagnosisTest->quit()
    //      And should not need to wait for the thread to finish: just connect to the finished() signal and destroy thread and tests there
    //      Exception: if the application quits, should the thread be terminated?
    m_threadRunningDiagnosisTest->terminate();
    m_threadRunningDiagnosisTest->wait();
    qDeleteAll(m_runDiagnosisTest->getDiagnosisTestToRun());
    delete m_runDiagnosisTest;
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
    connect(m_saveResultsButton, SIGNAL(clicked()), SLOT(saveDiagnosisTestResultsAsFile()));
    connect(m_closeButton, SIGNAL(clicked()), SLOT(accept()));
    
    connect(m_viewTestsLabel, SIGNAL(linkActivated(QString)), SLOT(viewTestsLabelClicked()));

    connect(this, SIGNAL(start()), m_runDiagnosisTest, SLOT(run()));
    connect(m_runDiagnosisTest, SIGNAL(runningDiagnosisTest(DiagnosisTest*)), this, SLOT(updateRunningDiagnosisTestProgress(DiagnosisTest*)));
    connect(m_runDiagnosisTest, SIGNAL(finished()), this, SLOT(finishedRunningDiagnosisTest()));

    connect(m_diagnosisTestsResults, &QWebEngineView::loadFinished, [this] { m_diagnosisTestsResults->history()->clear(); });
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
    m_diagnosisTestResultWriter.setDiagnosisTests(m_errorExecutedDiagnosisTests + m_warningExecutedDiagnosisTests + m_okExecutedDiagnosisTests);
    fillDiagnosisTestsResultTable();

    if (allDiagnosisTestResultAreOk())
    {
        m_allTestSuccededFrame->setVisible(true);
    }
    else
    {
        m_someTestsFailedFrame->setVisible(true);
        m_diagnosisTestsResults->setVisible(true);
    }

    m_progressBarFrame->setVisible(false);
    m_closeButtonFrame->setVisible(true);

    this->adjustSize();
}

void QDiagnosisTest::viewTestsLabelClicked()
{
    m_diagnosisTestsResults->page()->runJavaScript("showAllTests(); null");
    m_diagnosisTestsResults->setVisible(true);
    m_viewTestsLabel->setVisible(false);
    this->adjustSize();
}

void QDiagnosisTest::fillDiagnosisTestsResultTable()
{
    QString html = m_diagnosisTestResultWriter.getAsQString();
    m_diagnosisTestsResults->page()->setHtml(html);
}

void QDiagnosisTest::updateWidgetToRunDiagnosisTest()
{
    m_diagnosisTestsResults->setVisible(false);
    m_allTestSuccededFrame->setVisible(false);
    m_closeButtonFrame->setVisible(false);
    m_someTestsFailedFrame->setVisible(false);

    this->adjustSize();
}

void QDiagnosisTest::saveDiagnosisTestResultsAsFile()
{
    QString pathFile = QFileDialog::getSaveFileName(this, tr("Save Diagnosis Tests Results"), QDir::homePath(), tr("HTML Files (*.html)"));

    if (!pathFile.isEmpty())
    {
        m_diagnosisTestResultWriter.write(pathFile);
    }
}

bool QDiagnosisTest::allDiagnosisTestResultAreOk()
{
    return m_errorExecutedDiagnosisTests.count() == 0 && m_warningExecutedDiagnosisTests.count() == 0;
}

QList<DiagnosisTest*> QDiagnosisTest::getDiagnosisTestsToRun() const
{
    QList<DiagnosisTest*> diagnosisTestsToRun;

    foreach(QString className, DiagnosisTestFactory::instance()->getFactoryIdentifiersList())
    {
        diagnosisTestsToRun.append(DiagnosisTestFactory::instance()->create(className));
    }

    return diagnosisTestsToRun;
}

void QDiagnosisTest::groupDiagnosisTestFromRunDiagnosisTestByState()
{
    QList<QPair<DiagnosisTest*, DiagnosisTestResult> > runDiagnosisTests = m_runDiagnosisTest->getRunTests();

    for (int index = 0; index < runDiagnosisTests.count(); index++)
    {
        QPair<DiagnosisTest*, DiagnosisTestResult> runDiagnosisTest = runDiagnosisTests.at(index);

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
