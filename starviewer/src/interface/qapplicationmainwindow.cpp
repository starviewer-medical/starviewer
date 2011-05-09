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
#include "qstarviewersapwrapper.h"
#include "coresettings.h" // pel LanguageLocale
#include "inputoutputsettings.h"
#include "applicationversionchecker.h"
#include "screenmanager.h"
#include "qscreendistribution.h"
#include "volumerepository.h"
#include "applicationstylehelper.h"

// amb starviewer lite no hi haurà hanging protocols, per tant no els carregarem
#ifndef STARVIEWER_LITE
#include "hangingprotocolsloader.h"
#include "customwindowlevelsloader.h"
#endif

// Qt
#include <QAction>
#include <QSignalMapper>
#include <QMenuBar>
#include <QCloseEvent>
#include <QMessageBox>
#include <QApplication>
#include <QLocale>
#include <QProgressDialog>
#include <QDesktopServices>
#include <QPair>
#include <QWidgetAction>
#include <QShortcut>
//Shortucts
#include "shortcuts.h"
#include "shortcutmanager.h"

namespace udg {

/*Per processar les opcions entrades per línia de comandes hem d'utilitzar un Singleton de StarviewerApplicationCommandLine, això ve degut a que
  d'instàncies de QApplicationMainWindow en tenim tantes com finestres obertes d'Starviewer tinguem. Instàncies deQApplicationMainWindow es crees
  i es destrueixen  a mesura que s'obre una nova finestra o es tanca una finestra d'Starviewer per tant no podem responsabilitzar a cap
  QApplicationMainWindow que  s'encarregui de antendre les peticions rebudes via arguments o rebudes d'altres instàncies d'Starviewer a través
  de QtSingleApplication, perquè no podem  garantir que cap QApplicationMainWindow estigui viva durant tota l'execució d'Starviewer, per encarregar-se
  de processar els arugments de línia de comandes.

  Per això el que s'ha fet és que totes les QApplicationMainWindow es connectin a un signal de la mateixa instància de
  StarviewerSingleApplicationCommandLineSingleton, aquest signal és newOptionsToRun() que s'emet cada vegada que es reben nous arguments ja
  procedeixin de la mateixa instància al iniciar-la o d'altres instàncies via QtSingleApplication. Una vegada s'ha emés el signal les instàncies
  de QApplicationMainWindow a mesura que responen al signal amb el mètode takeOptionToRun() van processan tots els arguments fins que no en
  queda cap per processar.

  L'opció que processa una instància de QApplicationMainWindow obtinguda a través del mètode takeOptionToRun() desapereix de la llista d'opcions
  per processar de StarviewerApplicationCommandLine, de manera que tot i que totes les instàncies de QApplicationMainWindow poden processar
  opcions rebuts, cada opció només serà processat per la primera instància que l'agafi a través del mètode takeOptionToRun().
 */

typedef SingletonPointer<StarviewerApplicationCommandLine> StarviewerSingleApplicationCommandLineSingleton;

QApplicationMainWindow::QApplicationMainWindow(QWidget *parent)
    : QMainWindow(parent), m_patient(0), m_isBetaVersion(false)
{
    connect(StarviewerSingleApplicationCommandLineSingleton::instance(), SIGNAL(newOptionsToRun()), SLOT(newCommandLineOptionsToRun()));

    this->setAttribute(Qt::WA_DeleteOnClose);
    m_extensionWorkspace = new ExtensionWorkspace(this);
    this->setCentralWidget(m_extensionWorkspace);

    DatabaseInstallation databaseInstallation;
    if (!databaseInstallation.checkStarviewerDatabase())
    {
        QString errorMessage = databaseInstallation.getErrorMessage();
        QMessageBox::critical(0, ApplicationNameString, tr("There have been some errors:\n").append(errorMessage).append(tr("\n\nYou can resolve this error by Tools>Configuration>Local Database menu.")));
    }

    m_extensionHandler = new ExtensionHandler(this);

    m_logViewer = new QLogViewer(this);

    createActions();
    createMenus();

    m_applicationVersionChecker = new ApplicationVersionChecker(this);
    m_applicationVersionChecker->checkReleaseNotes();

    // Llegim les configuracions de l'aplicació, estat de la finestra, posicio,etc
    readSettings();
    // icona de l'aplicació
    this->setWindowIcon(QIcon(":/images/starviewer.png"));
    this->setWindowTitle(ApplicationNameString);

// amb starviewer lite no hi haurà hanging protocols, per tant no els carregarem
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

        repositoriesLoaded = true;
    }
#endif

    // creem el progress dialog que notificarà la càrrega de volums
    m_progressDialog = new QProgressDialog(this);
    m_progressDialog->setModal(true);
    m_progressDialog->setRange(0, 100);
    m_progressDialog->setMinimumDuration(0);
    m_progressDialog->setWindowTitle(tr("Loading"));
    m_progressDialog->setLabelText(tr("Loading data, please wait..."));
    m_progressDialog->setCancelButton(0);

#ifdef BETA_VERSION
    markAsBetaVersion();
    showBetaVersionDialog();
#endif

    computeDefaultToolTextSize();

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
    writeSettings();
    this->killBill();
    delete m_extensionWorkspace;
    delete m_extensionHandler;
}

void QApplicationMainWindow::createActions()
{
    m_signalMapper = new QSignalMapper(this);
    connect(m_signalMapper, SIGNAL(mapped(int)), m_extensionHandler, SLOT(request(int)));
    connect(m_signalMapper, SIGNAL(mapped(const QString)), m_extensionHandler, SLOT(request(const QString)));

    m_newAction = new QAction(this);
    m_newAction->setText(tr("&New"));
    m_newAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::NewWindow));
    m_newAction->setStatusTip(tr("Open a new working window"));
    m_newAction->setIcon(QIcon(":/images/new.png"));
    connect(m_newAction, SIGNAL(triggered()), SLOT(openBlankWindow()));

    m_openAction = new QAction(this);
    m_openAction->setText(tr("&Open file..."));
    m_openAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::OpenFile));
    m_openAction->setStatusTip(tr("Open an existing volume file"));
    m_openAction->setIcon(QIcon(":/images/open.png"));
    m_signalMapper->setMapping(m_openAction, 1);
    connect(m_openAction, SIGNAL(triggered()), m_signalMapper, SLOT(map()));

    m_openDirAction = new QAction(this);
    m_openDirAction->setText(tr("Open files from a directory..."));
    m_openDirAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::OpenDirectory));
    m_openDirAction->setStatusTip(tr("Open an existing DICOM folder"));
    m_openDirAction->setIcon(QIcon(":/images/openDicom.png"));
    m_signalMapper->setMapping(m_openDirAction, 6);
    connect(m_openDirAction, SIGNAL(triggered()), m_signalMapper, SLOT(map()));

    m_pacsAction = new QAction(this);
#ifdef STARVIEWER_LITE // el menú "PACS" es dirà "Exams"
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
    m_localDatabaseAction->setStatusTip(tr("Browse Local Database Studies"));
    m_localDatabaseAction->setIcon(QIcon(":/images/database.png"));
    m_signalMapper->setMapping(m_localDatabaseAction, 10);
    connect(m_localDatabaseAction, SIGNAL(triggered()), m_signalMapper, SLOT(map()));
#endif
    // TODO potser almenys per la versió Lite caldria canviar la icona
    m_pacsAction->setIcon(QIcon(":/images/pacsQuery.png"));
    m_signalMapper->setMapping(m_pacsAction, 7);
    connect(m_pacsAction, SIGNAL(triggered()), m_signalMapper, SLOT(map()));

    m_openDICOMDIRAction = new QAction(this);
    m_openDICOMDIRAction->setText(tr("Open DICOMDIR..."));
    m_openDICOMDIRAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::OpenDICOMDIR));
    m_openDICOMDIRAction->setStatusTip(tr("Open DICOMDIR from CD, DVD, Pendrive or hard disk"));
    m_openDICOMDIRAction->setIcon(QIcon(":/images/openDICOMDIR.png"));
    m_signalMapper->setMapping(m_openDICOMDIRAction, 8);
    connect(m_openDICOMDIRAction, SIGNAL(triggered()), m_signalMapper, SLOT(map()));

    QList<QString> extensionsMediatorNames = ExtensionMediatorFactory::instance()->getFactoryNamesList();
    foreach (QString name, extensionsMediatorNames)
    {
        ExtensionMediator* mediator = ExtensionMediatorFactory::instance()->create(name);

        if (mediator)
        {
            QAction *action = new QAction(this);
            action->setText(mediator->getExtensionID().getLabel());
            action->setStatusTip(tr("Open the %1 Application").arg(mediator->getExtensionID().getLabel()));
            action->setEnabled(false);
            m_signalMapper->setMapping(action, mediator->getExtensionID().getID());
            connect(action, SIGNAL(triggered()), m_signalMapper, SLOT(map()));
            m_actionsList.append(action);

            delete mediator;
        }
        else
        {
            ERROR_LOG("Error carregant el mediator de " + name);
        }
    }

    m_maximizeAction = new QAction(this);
    m_maximizeAction->setText(tr("Maximize To Multiple Screen"));
    m_maximizeAction->setStatusTip(tr("Maximize The Window To As Many Screens As Possible"));
    m_maximizeAction->setCheckable(false);
    m_maximizeAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::MaximizeMultipleScreens));
    connect(m_maximizeAction, SIGNAL(triggered(bool)), this, SLOT(maximizeMultipleScreens()));

    m_moveToDesktopAction = new QWidgetAction(this);
    QScreenDistribution *screenDistribution = new QScreenDistribution(this);
    m_moveToDesktopAction->setDefaultWidget(screenDistribution);
    m_moveToDesktopAction->setText(tr("Move To Screen"));
    m_moveToDesktopAction->setStatusTip(tr("Move The Window To The Screen ..."));
    m_moveToDesktopAction->setCheckable(false);
    connect(screenDistribution, SIGNAL(screenClicked(int)), this, SLOT(moveToDesktop(int)));
    // Shortcuts de MoveToDesktop
    QList<QKeySequence> keySequence = ShortcutManager::getShortcuts(Shortcuts::MoveToPreviousDesktop);
    if (keySequence.count() > 0)
    {
        m_moveToPreviousDesktopShortcut = new QShortcut(keySequence[0], this);
        connect(m_moveToPreviousDesktopShortcut, SIGNAL(activated()), this, SLOT(moveToPreviousDesktop()));
    }
    keySequence = ShortcutManager::getShortcuts(Shortcuts::MoveToNextDesktop);
    if (keySequence.count() > 0)
    {
        m_moveToNextDesktopShortcut = new QShortcut(keySequence[0], this);
        connect(m_moveToNextDesktopShortcut, SIGNAL(activated()), this, SLOT(moveToNextDesktop()));
    }

    m_openUserGuideAction = new QAction(this);
    m_openUserGuideAction->setText(tr("User guide"));
    m_openUserGuideAction->setStatusTip(tr("Open User guide"));
    connect(m_openUserGuideAction, SIGNAL(triggered()), this, SLOT(openUserGuide()));

    m_openQuickStartGuideAction = new QAction(this);
    m_openQuickStartGuideAction->setText(tr("Quick Start guide"));
    m_openQuickStartGuideAction->setStatusTip(tr("Open Quick Start guide"));
    connect(m_openQuickStartGuideAction, SIGNAL(triggered()), this, SLOT(openQuickStartGuide()));

    m_openShortcutsGuideAction = new QAction(this);
    m_openShortcutsGuideAction->setText(tr("Shortcuts guide"));
    m_openShortcutsGuideAction->setStatusTip(tr("Open Shortcuts guide"));
    connect(m_openShortcutsGuideAction, SIGNAL(triggered()), this, SLOT(openShortcutsGuide()));

    m_logViewerAction = new QAction(this);
    m_logViewerAction->setText(tr("Show log file"));
    m_logViewerAction->setStatusTip(tr("Show log file"));
    m_logViewerAction->setIcon(QIcon(":/images/logs.png"));
    connect(m_logViewerAction, SIGNAL(triggered()), m_logViewer, SLOT(updateData()));
    connect(m_logViewerAction, SIGNAL(triggered()), m_logViewer, SLOT(exec()));

    m_openReleaseNotesAction = new QAction(this);
    m_openReleaseNotesAction->setText(tr("&Release notes"));
    m_openReleaseNotesAction->setStatusTip(tr("Show the application's release notes for current version"));
    connect(m_openReleaseNotesAction, SIGNAL(triggered()), SLOT(openReleaseNotes()));

    m_aboutAction = new QAction(this);
    m_aboutAction->setText(tr("&About"));
    m_aboutAction->setStatusTip(tr("Show the application's About box"));
    m_aboutAction->setIcon(QIcon(":/images/info.png"));
    connect(m_aboutAction, SIGNAL(triggered()), SLOT(about()));

    m_closeAction = new QAction(this);
    m_closeAction->setText(tr("&Close"));
    m_closeAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::CloseCurrentExtension));
    m_closeAction->setStatusTip(tr("Close the current extension page"));
    m_closeAction->setIcon(QIcon(":/images/fileclose.png"));
    connect(m_closeAction, SIGNAL(triggered()), m_extensionWorkspace, SLOT(closeCurrentApplication()));

    m_exitAction = new QAction(this);
    m_exitAction->setText(tr("E&xit"));
    m_exitAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::CloseApplication));
    m_exitAction->setStatusTip(tr("Exit the application"));
    m_exitAction->setIcon(QIcon(":/images/exit.png"));
    connect(m_exitAction, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    m_configurationAction = new QAction(this);
    m_configurationAction->setText(tr("&Configuration..."));
    m_configurationAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::Preferences));
    m_configurationAction->setStatusTip(tr("Modify %1 configuration").arg(ApplicationNameString));
    m_configurationAction->setIcon(QIcon(":/images/preferences.png"));
    connect(m_configurationAction, SIGNAL(triggered()), SLOT(showConfigurationDialog()));
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
    connect(&configurationDialog, SIGNAL(configurationChanged(const QString&)), m_extensionHandler, SLOT(updateConfiguration(const QString&)));
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
    //no afegim els menús de visualització
#else
    // accions relacionades amb la visualització
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

    // Menú 'window'
    m_windowMenu = menuBar()->addMenu(tr("&Window"));
    m_windowMenu->addAction(m_maximizeAction);
    m_moveWindowToDesktopMenu = m_windowMenu->addMenu(tr("Move To Screen"));
    m_moveWindowToDesktopMenu->addAction(m_moveToDesktopAction);

    menuBar()->addSeparator();

    // menú d'ajuda i suport
    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpMenu->addAction(m_openUserGuideAction);
    m_helpMenu->addAction(m_openQuickStartGuideAction);
    m_helpMenu->addAction(m_openShortcutsGuideAction);
    m_helpMenu->addSeparator();
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

    QSignalMapper* signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(switchToLanguage(QString)));

    QActionGroup *actionGroup = new QActionGroup(this);

    QMapIterator<QString, QString> i(languages);
    while (i.hasNext())
    {
        i.next();

        QAction *action = createLanguageAction(i.value(), i.key());
        signalMapper->setMapping(action, i.key());
        connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));

        actionGroup->addAction(action);
        m_languageMenu->addAction(action);
    }
}

QAction* QApplicationMainWindow::createLanguageAction(const QString &language, const QString &locale)
{
    Settings settings;
    QString defaultLocale = settings.getValue(CoreSettings::LanguageLocale).toString();

    QAction *action = new QAction(this);
    action->setText(language);
    action->setStatusTip(tr("Switch to %1 Language").arg(language));
    action->setCheckable(true);
    action->setChecked(defaultLocale == locale);

    return action;
}

void QApplicationMainWindow::killBill()
{
    // eliminem totes les extensions
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

    QMessageBox::information(this, tr("Language Switch"), tr("The changes will take effect the next time you startup the application"));
}

void QApplicationMainWindow::setPatientInNewWindow(Patient *patient)
{
    QApplicationMainWindow *newMainWindow = new QApplicationMainWindow(0);
    newMainWindow->setPatient(patient);
    newMainWindow->show();
}

void QApplicationMainWindow::openBlankWindow()
{
    QApplicationMainWindow *newMainWindow = new QApplicationMainWindow(0);
    newMainWindow->show();
}

void QApplicationMainWindow::setPatient(Patient *patient)
{
    if (!patient) // si les dades de pacient són nules, no fem res
    {
        DEBUG_LOG("NULL Patient, maybe creating a blank new window");
        return;
    }

    if (this->getCurrentPatient())
    {
        // primer ens carreguem el pacient
        this->killBill();
        delete m_patient;
        m_patient = NULL;
        DEBUG_LOG("Ja teníem un pacient, l'esborrem.");
    }

    m_patient = patient;
    connectPatientVolumesToNotifier(patient);

    this->setWindowTitle(m_patient->getID() + " : " + m_patient->getFullName());
    enableExtensions();
    m_extensionHandler->getContext().setPatient(patient);
    m_extensionHandler->openDefaultExtension();
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

QApplicationMainWindow* QApplicationMainWindow::getActiveApplicationMainWindow()
{
    return qobject_cast<QApplicationMainWindow*>(QApplication::activeWindow());
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
    m_applicationVersionChecker->showIfCorrect();
    QMainWindow::showEvent(event);
}

void QApplicationMainWindow::about()
{
    QString aboutMessage = tr("<h2>%1</h2>"
        "<p>Copyright &copy; 2005-%2 Graphics & Imaging Laboratory (GILab), Girona"
        "<p align='justify'>%1 is a basic but fully featured image review software dedicated to DICOM images produced by medical equipment (MRI,"
        " CT, PET, PET-CT, CR, MG,...) fully compliant with the DICOM standard for image communication and image file formats. It can also read"
        " many other file formats specified by the MetaIO standard (*.mhd files). ").arg(ApplicationNameString).arg(2011);
    // TODO repassar els textos de l'about
#ifdef STARVIEWER_LITE
    aboutMessage += tr("<p align='justify'>%1 has been specifically designed for navigation and visualization of multimodality and"
               " multidimensional images").arg(ApplicationNameString);
#else
    aboutMessage += tr("%1 is able to receive images "
               "transferred by DICOM communication protocol from any PACS or medical imaging modality (STORE SCP - Service Class Provider, "
               "STORE SCU - Service Class User, and Query/Retrieve)."
               "<p align='justify'>%1 enables navigation and visualization of multimodality and multidimensional images through"
               " a complete 2D Viewer which integrates advanced reconstruction techniques such as Thick Slab (including Maximum"
               " Intensity Projection (MIP), Minimum Intensity Projection (MinIP) and average projection), fast orthogonal reconstruction"
               " and 3D navigation tools such as 3D-Cursor. It also incorporates Multi-Planar Reconstruction (MPR) and 3D Viewer for volume rendering."
               "<p align='justify'>%1 is at the same time a DICOM workstation for medical imaging and an image processing"
               " software for medical research (radiology and nuclear imaging), functional imaging and 3D imaging.").arg(ApplicationNameString);
#endif

    aboutMessage += tr("<p>Version: %1 </p>").arg(StarviewerVersionString);
    aboutMessage += tr("<p>Support email: <a href=\"mailto:%1\">%1</a></p>").arg(OrganizationEmailString);
    aboutMessage += tr("<p>Web: <a href=\"%1\">%1</a></p>").arg(OrganizationWebURL);

    QMessageBox::about(this, tr("About %1").arg(ApplicationNameString), aboutMessage);
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
    m_betaVersionMenuText->setText("<a href='beta'><img src=':/images/beta-warning.png'></a>&nbsp;<a href='beta'>Beta Version</a>");
    m_betaVersionMenuText->setAlignment(Qt::AlignVCenter);
    connect(m_betaVersionMenuText, SIGNAL(linkActivated(const QString &)), SLOT(showBetaVersionDialog()));
    updateBetaVersionTextPosition();
}

void QApplicationMainWindow::updateBetaVersionTextPosition()
{
    m_betaVersionMenuText->move(this->size().width() - (m_betaVersionMenuText->sizeHint().width() + 10), 5);
}

void QApplicationMainWindow::showBetaVersionDialog()
{
    QMessageBox::warning(this, tr("Beta version"),
                         tr("<h2>%1</h2>"
                            "<p align='justify'>This is a preview release of %1 used exclusively for testing purposes.</p>"
                            "<p align='justify'>This version is intended for radiologists and our test-team members. "
                            "Users of this version should not expect the extensions function properly.</p>"
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

void QApplicationMainWindow::newCommandLineOptionsToRun()
{
    QPair<StarviewerApplicationCommandLine::StarviewerCommandLineOption, QString> optionValue;

    //Mentre quedin opcions per processar
    while (StarviewerSingleApplicationCommandLineSingleton::instance()->takeOptionToRun(optionValue))
    {
        switch (optionValue.first)
        {
            case StarviewerApplicationCommandLine::openBlankWindow:
                INFO_LOG("Rebut argument de linia de comandes per obrir nova finestra");
                openBlankWindow();
                break;
            case StarviewerApplicationCommandLine::retrieveStudyFromAccessioNumber:
                INFO_LOG("Rebut argument de linia de comandes per descarregar un estudi a traves del seu accession number");
                sendRequestRetrieveStudyWithAccessionNumberToLocalStarviewer(optionValue.second);
                break;
            default:
                INFO_LOG("Argument de linia de comandes invalid");
                break;
        }
    }
}

void QApplicationMainWindow::sendRequestRetrieveStudyWithAccessionNumberToLocalStarviewer(QString accessionNumber)
{
    Settings settings;
    if (settings.getValue(udg::InputOutputSettings::ListenToRISRequests).toBool())
    {
        QStarviewerSAPWrapper().sendRequestToLocalStarviewer(accessionNumber);
    }
    else
    {
        //TODO:S'hauria de fer un missatge més genèric
        QMessageBox::information(this, ApplicationNameString, tr("Please activate \"Listen RIS Request\" option in %1 configuration to retrieve studies from SAP.").arg(ApplicationNameString));
    }
}

void QApplicationMainWindow::updateVolumeLoadProgressNotification(int progress)
{
    m_progressDialog->setValue(progress);
}

void QApplicationMainWindow::openUserGuide()
{
    QString userGuideFilePath = QCoreApplication::applicationDirPath() + "/Starviewer_User_guide.pdf";
    QDesktopServices::openUrl(QUrl::fromLocalFile(userGuideFilePath));
}

void QApplicationMainWindow::openQuickStartGuide()
{
    QString userGuideFilePath = QCoreApplication::applicationDirPath() + "/Starviewer_Quick_start_guide.pdf";
    QDesktopServices::openUrl(QUrl::fromLocalFile(userGuideFilePath));
}

void QApplicationMainWindow::openShortcutsGuide()
{
    QString userGuideFilePath = QCoreApplication::applicationDirPath() + "/Starviewer_Shortcuts_guide.pdf";
    QDesktopServices::openUrl(QUrl::fromLocalFile(userGuideFilePath));
}

void QApplicationMainWindow::openReleaseNotes()
{
    m_applicationVersionChecker->showReleaseNotes();
}

void QApplicationMainWindow::computeDefaultToolTextSize()
{
    ApplicationStyleHelper().recomputeStyleToScreenOfWidget(this);
}
}; // end namespace udg
