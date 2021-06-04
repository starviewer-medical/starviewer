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

#include "diagnosistest.h"
#include "diagnosistestfactory.h"
#include "diagnosistestresultwriter.h"
#include "machineinformation.h"
#include "qdiagnosistestsresultsitem.h"
#include "rundiagnosistest.h"
#include "screenmanager.h"
#include "starviewerapplication.h"
#include "systeminformation.h"

#include <QDateTime>
#include <QFileDialog>
#include <QMovie>
#include <QScrollBar>
#include <QStyle>
#include <QThread>

namespace udg {

namespace {

QStringList getSystemInformation()
{
    std::unique_ptr<SystemInformation> systemInformation(SystemInformation::newInstance());
    QStringList systemDetails{QObject::tr("Operating system: %1").arg(systemInformation->getOperatingSystemAsString())};
    QString desktopComposition(QObject::tr("Desktop composition: %1"));

    if (systemInformation->isDesktopCompositionAvailable())
    {
        if (systemInformation->isDesktopCompositionEnabled())
        {
            desktopComposition = desktopComposition.arg(QObject::tr("Available and enabled"));
        }
        else
        {
            desktopComposition = desktopComposition.arg(QObject::tr("Available but disabled"));
        }
    }
    else
    {
        desktopComposition = desktopComposition.arg(QObject::tr("Not available"));
    }

    systemDetails << desktopComposition
                  << QObject::tr("RAM memory: %1 MB").arg(systemInformation->getRAMTotalAmount());
    QStringList cpus;

    foreach (unsigned int frequency, systemInformation->getCPUFrequencies())
    {
        cpus.append(QObject::tr("%1 MHz").arg(frequency));
    }

    systemDetails << QObject::tr("CPU clock speed: %1").arg(cpus.join(", "))
                  << QObject::tr("Number of cores: %1").arg(systemInformation->getCPUNumberOfCores());
    QStringList gpuModels = systemInformation->getGPUModel();
    QList<unsigned int> gpuRams = systemInformation->getGPURAM();
    QStringList gpuDrivers = systemInformation->getGPUDriverVersion();

    for (int i = 0; i < gpuRams.size(); i++)
    {
        systemDetails.append(QObject::tr("GPU %1: %2 %3 MB. Driver: %4.").arg(i + 1).arg(gpuModels.at(i)).arg(gpuRams.at(i)).arg(gpuDrivers.at(i)));
    }

    systemDetails.append(QObject::tr("OpenGL: %1").arg(systemInformation->getGPUOpenGLVersion()));
    ScreenManager screenManager;
    ScreenLayout layout = screenManager.getScreenLayout();
    int primaryID = layout.getPrimaryScreenID();

    for (int i = 0; i < layout.getNumberOfScreens(); i++)
    {
        QSize size = layout.getScreen(i).getGeometry().size();
        systemDetails.append(QObject::tr("Screen %1: %2×%3%4").arg(i + 1).arg(size.width()).arg(size.height())
                                                              .arg(i == primaryID ? QObject::tr(" Primary") : QString()));
    }

    systemDetails.append(QObject::tr("MAC address: %1").arg(MachineInformation().getMACAddress()));

    return systemDetails;
}

}

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

    m_diagnosisTestsResultsScrollArea->verticalScrollBar()->installEventFilter(new ScrollBarEventFilter(this));
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
    fillDiagnosisTestsResultTable();

    if (allDiagnosisTestResultAreOk())
    {
        m_resultsIconLabel->setPixmap(QPixmap(":/images/icons/emblem-success.svg"));
        m_resultsTitleLabel->setText(tr("All tests completed successfully"));
        m_buttonsAndScrollAreaWidget->hide();
    }
    else
    {
        m_resultsIconLabel->setPixmap(QPixmap(":/images/icons/emblem-warning.svg"));
        m_resultsTitleLabel->setText(tr("Some tests have failed"));
        m_viewTestsLabel->hide();
        m_correctPushButton->setChecked(false);
        this->resize(640, 540); // hardcoded default size to show results (don't worry, used only here and below)
    }

    m_progressBarWidget->hide();
    m_resultsWidget->show();
}

void QDiagnosisTest::viewTestsLabelClicked()
{
    m_buttonsAndScrollAreaWidget->show();
    m_viewTestsLabel->hide();
    this->resize(640, 540); // hardcoded default size to show results (don't worry, used only here and above)
}

void QDiagnosisTest::fillDiagnosisTestsResultTable()
{
    // Information
    QStringList testDetails{QString("%1 %2").arg(ApplicationNameString).arg(StarviewerVersionString),
                            tr("Timestamp: %1").arg(QDateTime::currentDateTime().toString(Qt::ISODate))};
    m_informationItems << qMakePair(tr("Test details"), testDetails)
                       << qMakePair(tr("System information"), getSystemInformation());

    foreach (auto pair, m_informationItems)
    {
        auto item = new QDiagnosisTestsResultsItem(pair.first, pair.second);
        connect(m_informationPushButton, &QPushButton::toggled, item, &QWidget::setVisible);
        m_diagnosisTestsResults->layout()->addWidget(item);
    }

    if (m_informationItems.isEmpty())
    {
        m_informationPushButton->hide();
    }
    else
    {
        m_informationPushButton->setText(m_informationPushButton->text().arg(m_informationItems.size()));
    }

    // Errors
    foreach (auto pair, m_errorExecutedDiagnosisTests)
    {
        auto item = new QDiagnosisTestsResultsItem(*pair.first, pair.second);
        connect(m_errorsPushButton, &QPushButton::toggled, item, &QWidget::setVisible);
        m_diagnosisTestsResults->layout()->addWidget(item);
    }

    if (m_errorExecutedDiagnosisTests.isEmpty())
    {
        m_errorsPushButton->hide();
    }
    else
    {
        m_errorsPushButton->setText(m_errorsPushButton->text().arg(m_errorExecutedDiagnosisTests.size()));
    }

    // Warnings
    foreach (auto pair, m_warningExecutedDiagnosisTests)
    {
        auto item = new QDiagnosisTestsResultsItem(*pair.first, pair.second);
        connect(m_warningsPushButton, &QPushButton::toggled, item, &QWidget::setVisible);
        m_diagnosisTestsResults->layout()->addWidget(item);
    }

    if (m_warningExecutedDiagnosisTests.isEmpty())
    {
        m_warningsPushButton->hide();
    }
    else
    {
        m_warningsPushButton->setText(m_warningsPushButton->text().arg(m_warningExecutedDiagnosisTests.size()));
    }

    // Correct
    foreach (auto pair, m_okExecutedDiagnosisTests)
    {
        auto item = new QDiagnosisTestsResultsItem(*pair.first, pair.second);
        connect(m_correctPushButton, &QPushButton::toggled, item, &QWidget::setVisible);
        m_diagnosisTestsResults->layout()->addWidget(item);
    }

    if (m_okExecutedDiagnosisTests.isEmpty())
    {
        m_correctPushButton->hide();
    }
    else
    {
        m_correctPushButton->setText(m_correctPushButton->text().arg(m_okExecutedDiagnosisTests.size()));
    }

    m_diagnosisTestsResults->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));
}

void QDiagnosisTest::updateWidgetToRunDiagnosisTest()
{
    m_resultsWidget->hide();
    this->adjustSize();
}

void QDiagnosisTest::saveDiagnosisTestResultsAsFile()
{
    QString pathFile = QFileDialog::getSaveFileName(this, tr("Save diagnosis tests results"), QDir::homePath() + "/diagnosistestsresults.json",
                                                    tr("JSON files (*.json)"));

    if (!pathFile.isEmpty())
    {
        DiagnosisTestResultWriter writer;
        writer.setInformations(m_informationItems);
        writer.setDiagnosisTests(m_errorExecutedDiagnosisTests + m_warningExecutedDiagnosisTests + m_okExecutedDiagnosisTests);
        writer.write(pathFile);
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

QDiagnosisTest::ScrollBarEventFilter::ScrollBarEventFilter(QDiagnosisTest *parent)
    : QObject(parent), m_diagnosisTest(parent)
{
}

bool QDiagnosisTest::ScrollBarEventFilter::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Show)
    {
        m_diagnosisTest->m_diagnosisTestsResults->layout()->setContentsMargins(0, 0, m_diagnosisTest->style()->pixelMetric(QStyle::PM_LayoutRightMargin), 0);
    }
    else if (event->type() == QEvent::Hide)
    {
        m_diagnosisTest->m_diagnosisTestsResults->layout()->setContentsMargins(0, 0, 0, 0);
    }

    return QObject::eventFilter(watched, event);
}

}
