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

#include "qapplicationmainwindow.h"

#include "extensionhandler.h"
#include "extensionworkspace.h"
#include "logging.h"
#include "qlogviewer.h"
#include "patient.h"
#include "qconfigurationdialog.h"
#include "volume.h"
#include "settings.h"
#include "extensionfactory.h"
#include "extensionmediatorfactory.h"
#include "starviewerapplication.h"
#include "statswatcher.h"
#include "databaseinstallation.h"
#include "interfacesettings.h"
#include "starviewerapplicationcommandline.h"
#include "qaboutdialog.h"
#include "externalapplication.h"
#include "externalapplicationsmanager.h"
#include "externalstudyrequestmediator.h"
#include "queryscreen.h"

// Pel LanguageLocale
#include "coresettings.h"
#include "inputoutputsettings.h"
#include "applicationversionchecker.h"
#include "screenmanager.h"
#include "qscreendistribution.h"
#include "volumerepository.h"
#include "qdiagnosistest.h"

// Amb starviewer lite no hi haurà hanging protocols, per tant no els carregarem
#ifndef STARVIEWER_LITE
#include "hangingprotocolsloader.h"
#include "customwindowlevelsloader.h"
#include "studylayoutconfigsloader.h"
#endif

#ifdef STARVIEWER_CE
#include "qmedicaldeviceinformationdialog.h"
#endif // STARVIEWER_CE

// Qt
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QLocale>
#include <QMenuBar>
#include <QMessageBox>
#include <QPair>
#include <QProgressDialog>
#include <QShortcut>
#include <QTimer>
#include <QWidgetAction>
// Shortucts
#include "shortcuts.h"
#include "shortcutmanager.h"

namespace udg {

QList<QApplicationMainWindow*> QApplicationMainWindow::m_lastActiveMainWindows;

// Per processar les opcions entrades per línia de comandes hem d'utilitzar un Singleton de StarviewerApplicationCommandLine, això ve degut a que
// d'instàncies de QApplicationMainWindow en tenim tantes com finestres obertes d'Starviewer tinguem. Instàncies deQApplicationMainWindow es crees
// i es destrueixen a mesura que s'obre una nova finestra o es tanca una finestra d'Starviewer per tant no podem responsabilitzar a cap
// QApplicationMainWindow que s'encarregui de antendre les peticions rebudes via arguments o rebudes d'altres instàncies d'Starviewer a través
// de QtSingleApplication, perquè no podem garantir que cap QApplicationMainWindow estigui viva durant tota l'execució d'Starviewer, per encarregar-se
// de processar els arugments de línia de comandes.

// Per això el que s'ha fet és que totes les QApplicationMainWindow es connectin a un signal de la mateixa instància de
// StarviewerSingleApplicationCommandLineSingleton, aquest signal és newOptionsToRun() que s'emet cada vegada que es reben nous arguments ja
// procedeixin de la mateixa instància al iniciar-la o d'altres instàncies via QtSingleApplication. Una vegada s'ha emés el signal les instàncies
// de QApplicationMainWindow a mesura que responen al signal amb el mètode takeOptionToRun() van processan tots els arguments fins que no en
// queda cap per processar.

// L'opció que processa una instància de QApplicationMainWindow obtinguda a través del mètode takeOptionToRun() desapereix de la llista d'opcions
// per processar de StarviewerApplicationCommandLine, de manera que tot i que totes les instàncies de QApplicationMainWindow poden processar
// opcions rebuts, cada opció només serà processat per la primera instància que l'agafi a través del mètode takeOptionToRun().

typedef SingletonPointer<StarviewerApplicationCommandLine> StarviewerSingleApplicationCommandLineSingleton;

QApplicationMainWindow::QApplicationMainWindow(QWidget *parent)
    : QMainWindow(parent), m_patient(0), m_isBetaVersion(false)
{
    // Add this window to the beginning of the list because it has never been active yet.
    // This ensures that all existent windows are on the list, thus the list is not empty as long as there is any window.
    m_lastActiveMainWindows.prepend(this);

    connect(StarviewerSingleApplicationCommandLineSingleton::instance(), SIGNAL(newOptionsToRun()), SLOT(newCommandLineOptionsToRun()));

    this->setAttribute(Qt::WA_DeleteOnClose);
    m_extensionWorkspace = new ExtensionWorkspace(this);
    this->setCentralWidget(m_extensionWorkspace);

    DatabaseInstallation databaseInstallation;
    if (!databaseInstallation.checkDatabase())
    {
        QString errorMessage = databaseInstallation.getErrorMessage();
        QMessageBox::critical(0, ApplicationNameString, tr("There have been some errors:") + "\n" + errorMessage + "\n\n" + 
                                                    tr("You can resolve this error at Tools > Configuration > Local Database."));
    }

    m_extensionHandler = new ExtensionHandler(this);

    m_logViewer = new QLogViewer(this);

    createActions();
    createMenus();

    // Llegim les configuracions de l'aplicació, estat de la finestra, posicio,etc
    readSettings();
    // Icona de l'aplicació
    this->setWindowIcon(QIcon(":/images/logo/logo.ico"));
    updateWindowTitle();

// Amb starviewer lite no hi haurà hanging protocols, per tant no els carregarem
#ifndef STARVIEWER_LITE
    // Càrrega dels repositoris que necessitem tenir carregats durant tota l'aplicació
    // Només carregarem un cop per sessió/instància d'starviewer
    static bool repositoriesLoaded = false;
    if (!repositoriesLoaded)
    {
        HangingProtocolsLoader hangingProtocolsLoader;
        hangingProtocolsLoader.loadDefaults();

        CustomWindowLevelsLoader customWindowLevelsLoader;
        customWindowLevelsLoader.loadDefaults();

        StudyLayoutConfigsLoader layoutConfigsLoader;
        layoutConfigsLoader.load();

        repositoriesLoaded = true;
    }
#endif

    // Creem el progress dialog que notificarà la càrrega de volums
    m_progressDialog = new QProgressDialog(this);
    m_progressDialog->setModal(true);
    m_progressDialog->setRange(0, 100);
    m_progressDialog->setMinimumDuration(1);
    m_progressDialog->setWindowTitle(tr("Loading"));
    m_progressDialog->setLabelText(tr("Loading data, please wait..."));
    m_progressDialog->setCancelButton(0);
    m_progressDialog->setValue(100);

#ifdef BETA_VERSION
    markAsBetaVersion();
#endif

    m_statsWatcher = new StatsWatcher("Menu triggering", this);
    m_statsWatcher->addTriggerCounter(m_fileMenu);
    m_statsWatcher->addTriggerCounter(m_visualizationMenu);
    m_statsWatcher->addTriggerCounter(m_toolsMenu);
    m_statsWatcher->addTriggerCounter(m_helpMenu);
    m_statsWatcher->addTriggerCounter(m_languageMenu);
    m_statsWatcher->addTriggerCounter(m_windowMenu);
}

QApplicationMainWindow::~QApplicationMainWindow()
{
    m_lastActiveMainWindows.removeOne(this);
    writeSettings();
    this->killBill();
    delete m_extensionWorkspace;
    delete m_extensionHandler;
}

void QApplicationMainWindow::checkNewVersionAndShowReleaseNotes()
{
    ApplicationVersionChecker::checkAndShowReleaseNotes();
}

void QApplicationMainWindow::createActions()
{
    m_newAction = new QAction(this);
    m_newAction->setText(tr("&New Window"));
    m_newAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::NewWindow));
    m_newAction->setStatusTip(tr("Open a new working window"));
    m_newAction->setIcon(QIcon(":/images/icons/window-new.svg"));
    connect(m_newAction, SIGNAL(triggered()), SLOT(openBlankWindow()));

    m_openAction = new QAction(this);
    m_openAction->setText(tr("&Open Files..."));
    m_openAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::OpenFile));
    m_openAction->setStatusTip(tr("Open one or several existing volume files"));
    m_openAction->setIcon(QIcon(":/images/icons/document-open.svg"));
    connect(m_openAction, &QAction::triggered, [this] { m_extensionHandler->request(ExtensionHandler::Request::OpenFiles); });

    m_openDirAction = new QAction(this);
    m_openDirAction->setText(tr("Open Files from a Directory..."));
    m_openDirAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::OpenDirectory));
    m_openDirAction->setStatusTip(tr("Open an existing DICOM folder"));
    m_openDirAction->setIcon(QIcon(":/images/icons/document-open.svg"));
    connect(m_openDirAction, &QAction::triggered, [this] { m_extensionHandler->request(ExtensionHandler::Request::OpenDirectory); });

    m_pacsAction = new QAction(this);
#ifdef STARVIEWER_LITE
    // El menú "PACS" es dirà "Exams"
    m_pacsAction->setText(tr("&Exams..."));
    m_pacsAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::OpenExams));
    m_pacsAction->setStatusTip(tr("Browse exams"));
#else
    m_pacsAction->setText(tr("&PACS..."));
    m_pacsAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::OpenPACS));
    m_pacsAction->setStatusTip(tr("Open PACS Query Screen"));

    m_localDatabaseAction = new QAction(this);
    m_localDatabaseAction->setText(tr("&Local Database Studies..."));
    m_localDatabaseAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::OpenLocalDatabaseStudies));
    m_localDatabaseAction->setStatusTip(tr("Browse local database studies"));
    m_localDatabaseAction->setIcon(QIcon(":/images/icons/database-local.svg"));
    connect(m_localDatabaseAction, &QAction::triggered, [this] { m_extensionHandler->request(ExtensionHandler::Request::LocalDatabase); });
#endif
    // TODO potser almenys per la versió Lite caldria canviar la icona
    m_pacsAction->setIcon(QIcon(":/images/icons/document-open-remote.svg"));
    connect(m_pacsAction, &QAction::triggered, [this] { m_extensionHandler->request(ExtensionHandler::Request::Pacs); });

    m_openDICOMDIRAction = new QAction(this);
    m_openDICOMDIRAction->setText(tr("Open DICOMDIR..."));
    m_openDICOMDIRAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::OpenDICOMDIR));
    m_openDICOMDIRAction->setStatusTip(tr("Open DICOMDIR from CD, DVD, USB flash drive or hard disk"));
    m_openDICOMDIRAction->setIcon(QIcon(":/images/icons/document-open-dicomdir.svg"));
    connect(m_openDICOMDIRAction, &QAction::triggered, [this] { m_extensionHandler->request(ExtensionHandler::Request::Dicomdir); });

    QStringList extensionsMediatorNames = ExtensionMediatorFactory::instance()->getFactoryIdentifiersList();
    foreach (const QString &name, extensionsMediatorNames)
    {
        ExtensionMediator *mediator = ExtensionMediatorFactory::instance()->create(name);

        if (mediator)
        {
            const DisplayableID &extensionId = mediator->getExtensionID();
            QAction *action = new QAction(this);
            action->setText(extensionId.getLabel());
            action->setStatusTip(tr("Open the %1 Application").arg(extensionId.getLabel()));
            action->setEnabled(false);
            connect(action, &QAction::triggered, [=] { m_extensionHandler->request(extensionId.getID()); });
            m_actionsList.append(action);

            delete mediator;
        }
        else
        {
            ERROR_LOG("Error carregant el mediator de " + name);
        }
    }

    m_maximizeAction = new QAction(this);
    m_maximizeAction->setText(tr("Maximize to Multiple Screens"));
    m_maximizeAction->setStatusTip(tr("Maximize the window to as many screens as possible"));
    m_maximizeAction->setCheckable(false);
    m_maximizeAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::MaximizeMultipleScreens));
    connect(m_maximizeAction, SIGNAL(triggered(bool)), this, SLOT(maximizeMultipleScreens()));

    m_moveToDesktopAction = new QWidgetAction(this);
    QScreenDistribution *screenDistribution = new QScreenDistribution(this);
    m_moveToDesktopAction->setDefaultWidget(screenDistribution);
    m_moveToDesktopAction->setText(tr("Move to Screen"));
    m_moveToDesktopAction->setStatusTip(tr("Move the window to the screen..."));
    m_moveToDesktopAction->setCheckable(false);
    connect(screenDistribution, SIGNAL(screenClicked(int)), this, SLOT(moveToDesktop(int)));
    
    m_moveToPreviousDesktopAction = new QAction(this);
    m_moveToPreviousDesktopAction->setText(tr("Move to previous screen"));
    m_moveToPreviousDesktopAction->setStatusTip(tr("Move the window to the previous screen"));
    m_moveToPreviousDesktopAction->setCheckable(false);
    m_moveToPreviousDesktopAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::MoveToPreviousDesktop));
    connect(m_moveToPreviousDesktopAction, SIGNAL(triggered(bool)), SLOT(moveToPreviousDesktop()));

    m_moveToNextDesktopAction = new QAction(this);
    m_moveToNextDesktopAction->setText(tr("Move to next screen"));
    m_moveToNextDesktopAction->setStatusTip(tr("Move the window to the next screen"));
    m_moveToNextDesktopAction->setCheckable(false);
    m_moveToNextDesktopAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::MoveToNextDesktop));
    connect(m_moveToNextDesktopAction, SIGNAL(triggered(bool)), SLOT(moveToNextDesktop()));

    m_openUserGuideAction = new QAction(this);
    m_openUserGuideAction->setText(tr("User Guide"));
    m_openUserGuideAction->setStatusTip(tr("Open user guide"));
    connect(m_openUserGuideAction, SIGNAL(triggered()), this, SLOT(openUserGuide()));

    m_openQuickStartGuideAction = new QAction(this);
    m_openQuickStartGuideAction->setText(tr("Quick Start Guide"));
    m_openQuickStartGuideAction->setStatusTip(tr("Open quick start guide"));
    connect(m_openQuickStartGuideAction, SIGNAL(triggered()), this, SLOT(openQuickStartGuide()));

    m_openShortcutsGuideAction = new QAction(this);
    m_openShortcutsGuideAction->setText(tr("Shortcuts Guide"));
    m_openShortcutsGuideAction->setStatusTip(tr("Open shortcuts guide"));
    connect(m_openShortcutsGuideAction, SIGNAL(triggered()), this, SLOT(openShortcutsGuide()));

#ifdef STARVIEWER_CE
    m_showMedicalDeviceInformationAction = new QAction(this);
    m_showMedicalDeviceInformationAction->setText(tr("Information about use as medical device"));
    connect(m_showMedicalDeviceInformationAction, &QAction::triggered, this, &QApplicationMainWindow::showMedicalDeviceInformationDialogUnconditionally);
#endif // STARVIEWER_CE

    m_logViewerAction = new QAction(this);
    m_logViewerAction->setText(tr("Show Log File"));
    m_logViewerAction->setStatusTip(tr("Show log file"));
    m_logViewerAction->setIcon(QIcon(":/images/icons/show-log.svg"));
    connect(m_logViewerAction, SIGNAL(triggered()), m_logViewer, SLOT(updateData()));
    connect(m_logViewerAction, SIGNAL(triggered()), m_logViewer, SLOT(exec()));

    m_openReleaseNotesAction = new QAction(this);
    m_openReleaseNotesAction->setText(tr("&Release Notes"));
    m_openReleaseNotesAction->setStatusTip(tr("Show the application's release notes for current version"));
    connect(m_openReleaseNotesAction, SIGNAL(triggered()), SLOT(openReleaseNotes()));

    m_aboutAction = new QAction(this);
    m_aboutAction->setText(tr("&About"));
    m_aboutAction->setStatusTip(tr("Show the application's About box"));
    m_aboutAction->setIcon(QIcon(":/images/logo/logo.ico"));
    connect(m_aboutAction, SIGNAL(triggered()), SLOT(about()));

    m_closeAction = new QAction(this);
    m_closeAction->setText(tr("&Close"));
    m_closeAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::CloseCurrentExtension));
    m_closeAction->setStatusTip(tr("Close current extension page"));
    m_closeAction->setIcon(QIcon(":/images/icons/project-development-close.svg"));
    connect(m_closeAction, SIGNAL(triggered()), m_extensionWorkspace, SLOT(closeCurrentApplication()));

    m_exitAction = new QAction(this);
    m_exitAction->setText(tr("E&xit"));
    m_exitAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::CloseApplication));
    m_exitAction->setStatusTip(tr("Exit the application"));
    m_exitAction->setIcon(QIcon(":/images/icons/application-exit.svg"));
    connect(m_exitAction, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    m_configurationAction = new QAction(this);
    m_configurationAction->setText(tr("&Configuration..."));
    m_configurationAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::Preferences));
    m_configurationAction->setStatusTip(tr("Modify %1 configuration").arg(ApplicationNameString));
    m_configurationAction->setIcon(QIcon(":/images/icons/configure.svg"));
    connect(m_configurationAction, SIGNAL(triggered()), SLOT(showConfigurationDialog()));

    m_runDiagnosisTestsAction = new QAction(this);
    m_runDiagnosisTestsAction->setText(tr("&Run Diagnosis Tests"));
    m_runDiagnosisTestsAction->setStatusTip(tr("Run %1 diagnosis tests").arg(ApplicationNameString));
    connect(m_runDiagnosisTestsAction, SIGNAL(triggered()), SLOT(showDiagnosisTestDialog()));

    m_showPatientIdentificationInWindowTitleAction = new QAction(this);
    m_showPatientIdentificationInWindowTitleAction->setText(tr("Show &patient identification in window title"));
    m_showPatientIdentificationInWindowTitleAction->setStatusTip(tr("Show patient identification in window title"));
    m_showPatientIdentificationInWindowTitleAction->setCheckable(true);
    m_showPatientIdentificationInWindowTitleAction->setChecked(true);
    connect(m_showPatientIdentificationInWindowTitleAction, &QAction::toggled, this, &QApplicationMainWindow::updateWindowTitle);
}

void QApplicationMainWindow::maximizeMultipleScreens()
{
    ScreenManager screenManager;
    screenManager.maximize(this);
}

void QApplicationMainWindow::moveToDesktop(int screenIndex)
{
    ScreenManager screenManager;
    screenManager.moveToDesktop(this, screenIndex);
}

void QApplicationMainWindow::moveToPreviousDesktop()
{
    ScreenManager screenManager;
    screenManager.moveToPreviousDesktop(this);
}

void QApplicationMainWindow::moveToNextDesktop()
{
    ScreenManager screenManager;
    screenManager.moveToNextDesktop(this);
}

void QApplicationMainWindow::showConfigurationDialog()
{
    QConfigurationDialog configurationDialog;
    configurationDialog.exec();
}

void QApplicationMainWindow::createMenus()
{
    // Menú d'arxiu
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_newAction);
#ifndef STARVIEWER_LITE
    m_fileMenu->addAction(m_localDatabaseAction);
#endif
    m_fileMenu->addAction(m_pacsAction);
    m_fileMenu->addAction(m_openDICOMDIRAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_openAction);
    m_fileMenu->addAction(m_openDirAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_closeAction);
    m_fileMenu->addAction(m_exitAction);

#ifdef STARVIEWER_LITE
    // No afegim els menús de visualització
#else
    // Accions relacionades amb la visualització
    m_visualizationMenu = menuBar()->addMenu(tr("&Visualization"));

    foreach (QAction *action, m_actionsList)
    {
        m_visualizationMenu->addAction(action);
    }
#endif

    // Menú tools
    m_toolsMenu = menuBar()->addMenu(tr("&Tools"));
    m_languageMenu = m_toolsMenu->addMenu(tr("&Language"));
    createLanguageMenu();
    m_toolsMenu->addAction(m_configurationAction);
    m_toolsMenu->addAction(m_runDiagnosisTestsAction);
    m_externalApplicationsMenu = 0;
    createExternalApplicationsMenu();
    connect(ExternalApplicationsManager::instance(), SIGNAL(onApplicationsChanged()), this, SLOT(createExternalApplicationsMenu()));
    m_toolsMenu->addAction(m_showPatientIdentificationInWindowTitleAction);

    // Menú 'window'
    m_windowMenu = menuBar()->addMenu(tr("&Window"));
    m_moveWindowToDesktopMenu = m_windowMenu->addMenu(tr("Move to Screen"));
    m_moveWindowToDesktopMenu->addAction(m_moveToDesktopAction);
    m_windowMenu->addAction(m_maximizeAction);
    m_windowMenu->addAction(m_moveToPreviousDesktopAction);
    m_windowMenu->addAction(m_moveToNextDesktopAction);
    
    menuBar()->addSeparator();

    // Menú d'ajuda i suport
    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpMenu->addAction(m_openUserGuideAction);
    m_helpMenu->addAction(m_openQuickStartGuideAction);
    m_helpMenu->addAction(m_openShortcutsGuideAction);
    m_helpMenu->addSeparator();
#ifdef STARVIEWER_CE
    m_helpMenu->addAction(m_showMedicalDeviceInformationAction);
    m_helpMenu->addSeparator();
#endif // STARVIEWER_CE
    m_helpMenu->addAction(m_logViewerAction);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(m_openReleaseNotesAction);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(m_aboutAction);
}

void QApplicationMainWindow::createLanguageMenu()
{
    QMap<QString, QString> languages;
    languages.insert("ca_ES", tr("Catalan"));
    languages.insert("es_ES", tr("Spanish"));
    languages.insert("en_GB", tr("English"));

    QActionGroup *actionGroup = new QActionGroup(this);

    QMapIterator<QString, QString> i(languages);
    while (i.hasNext())
    {
        i.next();

        const QString &key = i.key();
        QAction *action = createLanguageAction(i.value(), key);
        connect(action, &QAction::triggered, [=] { switchToLanguage(key); });

        actionGroup->addAction(action);
        m_languageMenu->addAction(action);
    }
}

void QApplicationMainWindow::createExternalApplicationsMenu()
{
    QList<ExternalApplication> externalApplications = ExternalApplicationsManager::instance()->getApplications();
    delete m_externalApplicationsMenu;

    if (externalApplications.length() == 0) //If no external applications are defined, do not create the menu;
    {
        m_externalApplicationsMenu = 0;
        return;
    }

    m_externalApplicationsMenu = m_toolsMenu->addMenu(tr("&External applications"));
    m_externalApplicationsMenu->setIcon(QIcon(":/images/icons/system-run.svg"));

    QVector<QList<QKeySequence>> shortcutVector(12);
    shortcutVector[0] = ShortcutManager::getShortcuts(Shortcuts::ExternalApplication1);
    shortcutVector[1] = ShortcutManager::getShortcuts(Shortcuts::ExternalApplication2);
    shortcutVector[2] = ShortcutManager::getShortcuts(Shortcuts::ExternalApplication3);
    shortcutVector[3] = ShortcutManager::getShortcuts(Shortcuts::ExternalApplication4);
    shortcutVector[4] = ShortcutManager::getShortcuts(Shortcuts::ExternalApplication5);
    shortcutVector[5] = ShortcutManager::getShortcuts(Shortcuts::ExternalApplication6);
    shortcutVector[6] = ShortcutManager::getShortcuts(Shortcuts::ExternalApplication7);
    shortcutVector[7] = ShortcutManager::getShortcuts(Shortcuts::ExternalApplication8);
    shortcutVector[8] = ShortcutManager::getShortcuts(Shortcuts::ExternalApplication9);
    shortcutVector[9] = ShortcutManager::getShortcuts(Shortcuts::ExternalApplication10);
    shortcutVector[10] = ShortcutManager::getShortcuts(Shortcuts::ExternalApplication11);
    shortcutVector[11] = ShortcutManager::getShortcuts(Shortcuts::ExternalApplication12);

    QListIterator<ExternalApplication> i(externalApplications);
    int position = 0;
    while (i.hasNext())
    {
        const ExternalApplication& extApp = i.next();
        QAction* action = new QAction(extApp.getName(), m_externalApplicationsMenu);
        if (position < shortcutVector.size())
        {
            action->setShortcuts(shortcutVector[position]);
        }

        m_externalApplicationsMenu->addAction(action);
        connect(action, &QAction::triggered, [=] { launchExternalApplication(position); });
        position++;
    }
}

QAction* QApplicationMainWindow::createLanguageAction(const QString &language, const QString &locale)
{
    Settings settings;
    QString defaultLocale = settings.getValue(CoreSettings::LanguageLocale).toString();

    QAction *action = new QAction(this);
    action->setText(language);
    action->setStatusTip(tr("Switch to %1 language").arg(language));
    action->setCheckable(true);
    action->setChecked(defaultLocale == locale);

    return action;
}

void QApplicationMainWindow::killBill()
{
    // Eliminem totes les extensions
    this->getExtensionWorkspace()->killThemAll();
    // TODO descarregar tots els volums que tingui el pacient en aquesta finestra
    // quan ens destruim alliberem tots els volums que hi hagi a memòria
    if (this->getCurrentPatient() != NULL)
    {
        foreach (Study *study, this->getCurrentPatient()->getStudies())
        {
            foreach (Series *series, study->getSeries())
            {
                foreach (Identifier id, series->getVolumesIDList())
                {
                    VolumeRepository::getRepository()->deleteVolume(id);
                }
            }
        }
    }
}

void QApplicationMainWindow::switchToLanguage(QString locale)
{
    Settings settings;
    settings.setValue(CoreSettings::LanguageLocale, locale);

    QMessageBox::information(this, tr("Language Switch"), tr("Changes will take effect the next time you start the application"));
}

void QApplicationMainWindow::launchExternalApplication(int i)
{
    QList<ExternalApplication> externalApplications = ExternalApplicationsManager::instance()->getApplications();
    if (i < 0 && i >= externalApplications.size())
    {
        ERROR_LOG("Trying to launch an unexistant external application");
    }
    const ExternalApplication &app = externalApplications.at(i);
    if (!ExternalApplicationsManager::instance()->launch(app))
    {
        //Launch failed.
        QMessageBox::critical(this, tr("External application launch error"), tr("There has been an error launching the external application."));
    }
}

QApplicationMainWindow* QApplicationMainWindow::setPatientInNewWindow(Patient *patient)
{
    QApplicationMainWindow *newMainWindow = openBlankWindow();
    newMainWindow->setPatient(patient);

    return newMainWindow;
}

QApplicationMainWindow* QApplicationMainWindow::openBlankWindow()
{
    QApplicationMainWindow *newMainWindow = new QApplicationMainWindow(0);
    newMainWindow->show();

    return newMainWindow;
}

void QApplicationMainWindow::setPatient(Patient *patient)
{
    // Si les dades de pacient són nules, no fem res
    if (!patient)
    {
        DEBUG_LOG("NULL Patient, maybe creating a blank new window");
        return;
    }

    if (this->getCurrentPatient())
    {
        // Primer ens carreguem el pacient
        this->killBill();
        delete m_patient;
        m_patient = NULL;
        DEBUG_LOG("Ja teníem un pacient, l'esborrem.");
    }

    m_patient = patient;
    connectPatientVolumesToNotifier(patient);

    updateWindowTitle();
    enableExtensions();
    m_extensionHandler->getContext().setPatient(patient);
    m_extensionHandler->openDefaultExtensions();
}

Patient* QApplicationMainWindow::getCurrentPatient()
{
    return m_patient;
}

unsigned int QApplicationMainWindow::getCountQApplicationMainWindow()
{
    unsigned int count = 0;
    foreach (QWidget *widget, QApplication::topLevelWidgets())
    {
        if (qobject_cast<QApplicationMainWindow*>(widget))
        {
            ++count;
        }
    }

    return count;
}

QList<QApplicationMainWindow*> QApplicationMainWindow::getQApplicationMainWindows()
{
    return m_lastActiveMainWindows;
}

QApplicationMainWindow* QApplicationMainWindow::getActiveApplicationMainWindow()
{
    return qobject_cast<QApplicationMainWindow*>(QApplication::activeWindow());
}

QApplicationMainWindow* QApplicationMainWindow::getLastActiveApplicationMainWindow()
{
    Q_ASSERT(!m_lastActiveMainWindows.isEmpty());
    return m_lastActiveMainWindows.last();
}

ExtensionWorkspace* QApplicationMainWindow::getExtensionWorkspace()
{
    return m_extensionWorkspace;
}

void QApplicationMainWindow::closeEvent(QCloseEvent *event)
{
    // \TODO aquí hauríem de controlar si l'aplicació està fent altres tasques pendents que s'haurien de finalitzar abans de tancar
    // l'aplicació com per exemple imatges en descàrrega del PACS o similar.
    // Caldria fer-ho de manera centralitzada.
    event->accept();
}

void QApplicationMainWindow::resizeEvent(QResizeEvent *event)
{
    if (m_isBetaVersion)
    {
        updateBetaVersionTextPosition();
    }
    QMainWindow::resizeEvent(event);
}

void QApplicationMainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    // Show the beta warning automatically only the first time the first window is shown
    static bool betaVersionDialogShown = false;

    if (m_isBetaVersion && !betaVersionDialogShown)
    {
        betaVersionDialogShown = true;
        QTimer::singleShot(100, this, &QApplicationMainWindow::showBetaVersionDialog);  // short delay to ensure that the window is already visible
    }
}

bool QApplicationMainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::WindowActivate)
    {
        // No mutex needed because this always runs in the main thread
        // Move this window to the end of the list
        m_lastActiveMainWindows.removeOne(this);
        m_lastActiveMainWindows.append(this);
    }

    return QMainWindow::event(event);
}

void QApplicationMainWindow::about()
{
    QAboutDialog *about = new QAboutDialog(this);
    about->exec();
}

void QApplicationMainWindow::writeSettings()
{
    Settings settings;
    settings.saveGeometry(InterfaceSettings::ApplicationMainWindowGeometry, this);
}

void QApplicationMainWindow::enableExtensions()
{
    foreach (QAction *action, m_actionsList)
    {
        action->setEnabled(true);
    }
}

void QApplicationMainWindow::markAsBetaVersion()
{
    m_isBetaVersion = true;
    m_betaVersionMenuText = new QLabel(menuBar());
    m_betaVersionMenuText->setText("<a href='beta'><img src=':/images/icons/emblem-warning.svg'></a>&nbsp;<a href='beta'>Beta Version</a>");
    m_betaVersionMenuText->setAlignment(Qt::AlignVCenter);
    connect(m_betaVersionMenuText, SIGNAL(linkActivated(const QString&)), SLOT(showBetaVersionDialog()));
    updateBetaVersionTextPosition();
}

void QApplicationMainWindow::updateBetaVersionTextPosition()
{
    m_betaVersionMenuText->move(this->size().width() - (m_betaVersionMenuText->sizeHint().width() + 10), 5);
}

void QApplicationMainWindow::showBetaVersionDialog()
{
    QMessageBox::warning(this, tr("Beta Version"),
                         tr("<h2>%1</h2>"
                            "<p align='justify'>This is a preview release of %1 used exclusively for testing purposes.</p>"
                            "<p align='justify'>This version is intended for radiologists and our test-team members. "
                            "Users of this version should not expect extensions to function properly.</p>"
                            "<p align='justify'>If you want to help us to improve %1, please report any found bug or "
                            "any feature request you may have by sending an e-mail to: <a href=\"mailto:%2\">%2</a></p>"
                            "<h3>We really appreciate your feedback!</h3>").arg(ApplicationNameString).arg(OrganizationEmailString));
}

void QApplicationMainWindow::readSettings()
{
    Settings settings;
    if (!settings.contains(InterfaceSettings::ApplicationMainWindowGeometry))
    {
        this->showMaximized();
    }
    else
    {
        settings.restoreGeometry(InterfaceSettings::ApplicationMainWindowGeometry, this);
    }
}

void QApplicationMainWindow::connectPatientVolumesToNotifier(Patient *patient)
{
    foreach (Study *study, patient->getStudies())
    {
        foreach (Series *series, study->getSeries())
        {
            foreach (Volume *volume, series->getVolumesList())
            {
                connect(volume, SIGNAL(progress(int)), SLOT(updateVolumeLoadProgressNotification(int)));
            }
        }
    }
}

void QApplicationMainWindow::viewStudy(const QString &studyInstanceUid)
{
    m_extensionHandler->getQueryScreen()->viewStudyFromDatabase(studyInstanceUid);
}

void QApplicationMainWindow::loadStudy(const QString &studyInstanceUid)
{
    m_extensionHandler->getQueryScreen()->loadStudyFromDatabase(studyInstanceUid);
}

#ifdef STARVIEWER_CE
void QApplicationMainWindow::showMedicalDeviceInformationDialog()
{
    Settings settings;

    if (!settings.getValue(InterfaceSettings::DontShowMedicalDeviceInformationDialog).toBool())
    {
        showMedicalDeviceInformationDialogUnconditionally();
    }
}

void QApplicationMainWindow::showMedicalDeviceInformationDialogUnconditionally()
{
    QMedicalDeviceInformationDialog *dialog = new QMedicalDeviceInformationDialog(this);
    dialog->exec();
}
#endif // STARVIEWER_CE

void QApplicationMainWindow::newCommandLineOptionsToRun()
{
    QPair<StarviewerApplicationCommandLine::StarviewerCommandLineOption, QString> optionValue;

    // Mentre quedin opcions per processar
    while (StarviewerSingleApplicationCommandLineSingleton::instance()->takeOptionToRun(optionValue))
    {
        switch (optionValue.first)
        {
            case StarviewerApplicationCommandLine::OpenBlankWindow:
                INFO_LOG("Rebut argument de linia de comandes per obrir nova finestra");
                openBlankWindow();
                break;
            case StarviewerApplicationCommandLine::RetrieveStudyByUid:
                INFO_LOG("Received command line argument to retrieve a study by its UID");
                ExternalStudyRequestMediator::instance()->requestStudyByUid(optionValue.second);
                break;
            case StarviewerApplicationCommandLine::RetrieveStudyByAccessionNumber:
                INFO_LOG("Rebut argument de linia de comandes per descarregar un estudi a traves del seu accession number");
                ExternalStudyRequestMediator::instance()->requestStudyByAccessionNumber(optionValue.second);
                break;
            default:
                INFO_LOG("Argument de linia de comandes invalid");
                break;
        }
    }
}

void QApplicationMainWindow::updateVolumeLoadProgressNotification(int progress)
{
    m_progressDialog->setValue(progress);
}

namespace {

QString getLocalePrefix()
{
    QString defaultLocale = QLocale().name();
    return "[" + defaultLocale.left(2).toLower() + "] ";
}

}

void QApplicationMainWindow::openUserGuide()
{
    QString filePath = QCoreApplication::applicationDirPath() + "/" + getLocalePrefix() + "User guide.pdf";
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void QApplicationMainWindow::openQuickStartGuide()
{
    QString filePath = QCoreApplication::applicationDirPath() + "/" + getLocalePrefix() + "Quick start guide.pdf";
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void QApplicationMainWindow::openShortcutsGuide()
{
    QString filePath = QCoreApplication::applicationDirPath() + "/" + getLocalePrefix() + "Shortcuts guide.pdf";
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void QApplicationMainWindow::showDiagnosisTestDialog()
{
    QDiagnosisTest qDiagnosisTest;
    qDiagnosisTest.execAndRunDiagnosisTest();
}

void QApplicationMainWindow::updateWindowTitle()
{
    if (m_patient && m_showPatientIdentificationInWindowTitleAction->isChecked())
    {
        this->setWindowTitle(m_patient->getID() + " : " + m_patient->getFullName());
    }
    else
    {
        this->setWindowTitle(ApplicationNameString);
    }
}

void QApplicationMainWindow::openReleaseNotes()
{
    ApplicationVersionChecker::showLocalReleaseNotes();
}

} // end namespace udg
