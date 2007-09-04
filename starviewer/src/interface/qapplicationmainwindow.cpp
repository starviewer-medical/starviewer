/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
// Qt
#include <QAction>
#include <QSignalMapper>
#include <QSettings>
#include <QMenuBar>
#include <QCloseEvent>
#include <QMessageBox>
#include <QApplication>
#include <QLocale>
// els nostres widgets/elements de la plataforma
#include "qapplicationmainwindow.h"
#include "extensionhandler.h"
#include "extensionworkspace.h"
#include "logging.h"
#include "qlogviewer.h"
#include "patient.h"

// Mini - aplicacions
#include "cacheinstallation.h"

#include "extensionfactory.h"
#include "extensionmediatorfactory.h"

#include <QDebug>

namespace udg{

QApplicationMainWindow::QApplicationMainWindow( QWidget *parent, QString name )
    : QMainWindow( parent ), m_patient(0)
{
    this->setAttribute( Qt::WA_DeleteOnClose );
    this->setObjectName( name );
    m_extensionWorkspace = new ExtensionWorkspace( this );
    this->setCentralWidget( m_extensionWorkspace );

    CacheInstallation cacheInstallation;
    cacheInstallation.checkInstallationCacheImagePath();
    cacheInstallation.checkInstallationCacheDatabase();

    m_extensionHandler = new ExtensionHandler( this );

    m_logViewer = new QLogViewer( 0 );

    createActions();
    createMenus();

    // Llegim les configuracions de l'aplicació, estat de la finestra, posicio,etc
    readSettings();
    // icona de l'aplicació
    this->setWindowIcon( QPixmap(":/images/starviewer.png") );
    this->setWindowTitle( tr("Starviewer") );
}

QApplicationMainWindow::~QApplicationMainWindow()
{
    delete m_extensionWorkspace;
    m_extensionHandler->killBill();
}

void QApplicationMainWindow::createActions()
{
    m_signalMapper = new QSignalMapper( this );
    connect( m_signalMapper, SIGNAL( mapped(int) ), m_extensionHandler , SLOT( request(int) ) );
    connect( m_signalMapper, SIGNAL( mapped( const QString) ), m_extensionHandler , SLOT( request(const QString) ) );

    m_newAction = new QAction( this );
    m_newAction->setText( tr("&New") );
    m_newAction->setShortcut( tr("Ctrl+N") );
    m_newAction->setStatusTip(tr("Open a new working window") );
    m_newAction->setIcon( QIcon(":/images/new.png") );
    connect( m_newAction , SIGNAL( triggered() ), this, SLOT( openNewWindow() ) );

    m_openAction = new QAction( this );
    m_openAction->setText( tr("&Open...") );
    m_openAction->setShortcut( tr("Ctrl+O") );
    m_openAction->setStatusTip(tr("Open an existing volume file"));
    m_openAction->setIcon( QIcon(":/images/open.png") );
    m_signalMapper->setMapping( m_openAction , 1 );
    connect( m_openAction , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );

    m_openDirAction = new QAction( this );
    m_openDirAction->setText( tr("Open &DICOM Directory") );
    m_openDirAction->setShortcut( tr("Ctrl+D") );
    m_openDirAction->setStatusTip(tr("Open an existing DICOM folder"));
    m_openDirAction->setIcon( QIcon(":/images/openDicom.png") );
    m_signalMapper->setMapping( m_openDirAction , 6 );
    connect( m_openDirAction , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );

    m_pacsAction = new QAction( this );
    m_pacsAction->setText(tr("&PACS...") );
    m_pacsAction->setShortcut( tr("Ctrl+P") );
    m_pacsAction->setStatusTip( tr("Open PACS Query Screen") );
    m_pacsAction->setIcon( QIcon(":/images/pacsQuery.png") );
    m_signalMapper->setMapping( m_pacsAction , 7 );
    connect( m_pacsAction , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );

    QList<QString> extensionsMediatorNames = ExtensionMediatorFactory::instance()->getFactoryNamesList();
    foreach(QString name, extensionsMediatorNames)
    {
        ExtensionMediator* mediator = ExtensionMediatorFactory::instance()->create(name);

        if (mediator)
        {
            QAction *action = new QAction(this);
            action->setText( mediator->getExtensionID().getLabel() );
            action->setStatusTip( tr("Open the %1 Application").arg( mediator->getExtensionID().getLabel() ));
            action->setEnabled( false );
            m_signalMapper->setMapping( action , mediator->getExtensionID().getID() );
            connect( action , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );
            m_actionsList.append(action);
        }
        else
        {
            DEBUG_LOG( "Error carregant el mediator de " + name );
        }
    }

    m_fullScreenAction = new QAction( this );
    m_fullScreenAction->setText( tr("Show Full Screen") );
    m_fullScreenAction->setStatusTip( tr("Switch To Full Screen") );
    m_fullScreenAction->setShortcut( Qt::CTRL + Qt::Key_Return );
    m_fullScreenAction->setShortcutContext( Qt::ApplicationShortcut );
    m_fullScreenAction->setIcon( QIcon(":/images/fullscreen.png") );
    m_fullScreenAction->setCheckable( true );
    connect( m_fullScreenAction , SIGNAL( toggled(bool) ) , this , SLOT( switchFullScreen(bool) ) );

    m_logViewerAction = new QAction( this );
    m_logViewerAction->setText( tr("Show log file") );
    m_logViewerAction->setStatusTip( tr("Show log file") );
    m_logViewerAction->setIcon( QIcon(":/images/logs.png") );
    connect( m_logViewerAction , SIGNAL( triggered() ) , m_logViewer , SLOT( updateData() ) );
    connect( m_logViewerAction , SIGNAL( triggered() ) , m_logViewer , SLOT( exec() ) );


    m_aboutAction = new QAction( this );
    m_aboutAction->setText(tr("&About") );
    m_aboutAction->setShortcut( 0 );
    m_aboutAction->setStatusTip(tr("Show the application's About box"));
    m_aboutAction->setIcon( QIcon(":/images/info.png"));
    connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    m_closeAction = new QAction( this );
    m_closeAction->setText( tr("&Close") );
    m_closeAction->setShortcut( tr("Ctrl+W") );
    m_closeAction->setStatusTip(tr("Close the current extension page"));
    m_closeAction->setIcon( QIcon(":/images/fileclose.png"));
    connect( m_closeAction, SIGNAL( triggered() ), m_extensionWorkspace , SLOT( closeCurrentApplication() ) );

    m_exitAction = new QAction( this );
    m_exitAction->setText( tr("E&xit") );
    m_exitAction->setShortcut(tr("Ctrl+Q") );
    m_exitAction->setStatusTip(tr("Exit the application"));
    m_exitAction->setIcon( QIcon(":/images/exit.png") );
    connect(m_exitAction, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
}

void QApplicationMainWindow::switchFullScreen( bool full )
{
    if( full )
    {
        this->showFullScreen();
        m_fullScreenAction->setText( tr("Retract Full Screen") );
        m_fullScreenAction->setStatusTip( tr("Switch To Normal Screen") );
        m_fullScreenAction->setIcon( QIcon(":/images/retractFullscreen.png") );
    }
    else
    {
        this->showNormal();
        m_fullScreenAction->setText( tr("Show Full Screen") );
        m_fullScreenAction->setStatusTip( tr("Switch To Full Screen") );
        m_fullScreenAction->setIcon( QIcon(":/images/fullscreen.png") );
    }
}

void QApplicationMainWindow::createMenus()
{
    // Menú d'arxiu
    m_fileMenu = menuBar()->addMenu( tr("&File") );
    m_fileMenu->addAction( m_newAction );
    m_fileMenu->addAction( m_openAction );
    m_fileMenu->addAction( m_openDirAction );
    m_fileMenu->addAction( m_pacsAction );
    m_fileMenu->addSeparator();
    m_fileMenu->addAction( m_closeAction );
    m_fileMenu->addAction( m_exitAction );

    // accions relacionades amb la visualització
    m_visualizationMenu = menuBar()->addMenu( tr("&Visualization") );
//     m_visualizationMenu->addAction( m_2DViewerAction );

    foreach(QAction *action, m_actionsList)
    {
        m_visualizationMenu->addAction(action);
    }

    // menú per escollir idioma
    m_languageMenu = menuBar()->addMenu( tr("&Language") );
    createLanguageMenu();

    // Menú 'window'
    m_windowMenu = menuBar()->addMenu( tr("&Window") );
    m_windowMenu->addAction( m_fullScreenAction );

    menuBar()->addSeparator();

    // menú d'ajuda i suport
    m_helpMenu = menuBar()->addMenu( tr("&Help") );
    m_helpMenu->addAction( m_logViewerAction );
    m_helpMenu->addSeparator();
    m_helpMenu->addAction( m_aboutAction );
}

void QApplicationMainWindow::createLanguageMenu()
{
    QSettings settings;
    settings.beginGroup("Starviewer-Language");
    QString defaultLocale = settings.value( "languageLocale", "interface_" + QLocale::system().name() ).toString();
    settings.endGroup();

    QSignalMapper* signalMapper = new QSignalMapper( this );
    connect( signalMapper, SIGNAL( mapped(int) ), this , SLOT( switchToLanguage(int) ) );

    m_catalanAction = new QAction( this );
    m_catalanAction->setText( "Català" );
    m_catalanAction->setStatusTip( tr("Switch to Catalan Language") );
    m_catalanAction->setCheckable( true );
    if( defaultLocale == QString("interface_ca_ES") )
        m_catalanAction->setChecked( true );
    else
        m_catalanAction->setChecked( false );

    signalMapper->setMapping( m_catalanAction , 0 );
    connect( m_catalanAction , SIGNAL( triggered() ) , signalMapper , SLOT( map() ) );

    m_spanishAction = new QAction( this );
    m_spanishAction->setText( "Castellano" );
    m_spanishAction->setStatusTip( tr("Switch to Spanish Language") );
    m_spanishAction->setCheckable( true );
    if( defaultLocale == QString("interface_es_ES") )
        m_spanishAction->setChecked( true );
    else
        m_spanishAction->setChecked( false );
    signalMapper->setMapping( m_spanishAction , 1 );
    connect( m_spanishAction , SIGNAL( triggered() ) , signalMapper , SLOT( map() ) );

    m_englishAction = new QAction( this );
    m_englishAction->setText( "English" );
    m_englishAction->setStatusTip( tr("Switch to English Language") );
    m_englishAction->setCheckable( true );
    if( defaultLocale == QString("interface_en_GB") )
        m_englishAction->setChecked( true );
    else
        m_englishAction->setChecked( false );
    signalMapper->setMapping( m_englishAction , 2 );
    connect( m_englishAction , SIGNAL( triggered() ) , signalMapper , SLOT( map() ) );

    m_languageMenu->addAction( m_catalanAction );
    m_languageMenu->addAction( m_spanishAction );
    m_languageMenu->addAction( m_englishAction );
}

void QApplicationMainWindow::switchToLanguage( int id )
{
    QString locale = "interface_";

    switch( id )
    {
    case 0:
        locale += "ca_ES";
        m_catalanAction->setChecked( true );
        m_spanishAction->setChecked( false );
        m_englishAction->setChecked( false );
    break;

    case 1:
        locale += "es_ES";
        m_catalanAction->setChecked( false );
        m_spanishAction->setChecked( true );
        m_englishAction->setChecked( false );
    break;

    case 2:
        locale += "en_GB";
        m_catalanAction->setChecked( false );
        m_spanishAction->setChecked( false );
        m_englishAction->setChecked( true );
    break;

    default:
    break;
    }

    if( id < 3 && id > -1 )
    {
        QSettings settings;
        settings.beginGroup("Starviewer-Language");
        settings.setValue( "languageLocale", locale );
        settings.endGroup();
    }
    QMessageBox::information( this , tr("Language Switch") , tr("The changes will take effect after restarting the application") );
}

void QApplicationMainWindow::openNewWindow( Patient *patient )
{
    QString windowName;
    if( patient )
    {
        windowName = patient->getID() + " : " + patient->getFullName();
    }
    else
    {
        windowName = QString("No Patient Data [%1]").arg( getCountQApplicationMainWindow() + 1 );
    }

    QApplicationMainWindow *newMainWindow = new QApplicationMainWindow( 0, windowName );
    newMainWindow->addPatient( patient );
    newMainWindow->show();
}

void QApplicationMainWindow::addPatient( Patient *patient )
{
    if( !patient ) // si les dades de pacient són nules, no fem res
    {
        DEBUG_LOG("NULL Patient, maybe creating a blank new window");
        return;
    }

    if( !m_patient )
    {
        m_patient = patient;
        this->setWindowTitle( m_patient->getID() + " : " + m_patient->getFullName() );
        enableExtensions();
        // si són les primeres dades que es carreguen, cal obrir l'extensió per defecte
        m_extensionHandler->openDefaultExtension();
        DEBUG_LOG("No teníem cap pacient, assignem el que ens donen");
    }
    else if( m_patient->isSamePatient( patient ) )
    {
        *m_patient += *patient;
        DEBUG_LOG("Ja teníem dades d'aquest pacient. Fusionem informació");
    }
    else
    {
        // és un pacient diferent, cal obrir-lo apart en una nova finestra
        openNewWindow( patient );
        DEBUG_LOG("Creem nou pacient");
    }
}

Patient *QApplicationMainWindow::getCurrentPatient()
{
    return m_patient;
}

unsigned int QApplicationMainWindow::getCountQApplicationMainWindow()
{
    QWidgetList list( QApplication::topLevelWidgets() );
    unsigned int count = 0;
    for ( int i = 0; i < list.size(); ++i )
    {
        if ( QWidget *mainWin = qobject_cast<QWidget *>( list.at(i) ) )
        {
            if( mainWin->metaObject()->className() == "udg::QApplicationMainWindow" )
                count++;
        }
    }
    return count;
}

QApplicationMainWindow* QApplicationMainWindow::getActiveApplicationMainWindow()
{
    return qobject_cast<QApplicationMainWindow*>( QApplication::activeWindow() );
}

ExtensionWorkspace* QApplicationMainWindow::getExtensionWorkspace()
{
    return m_extensionWorkspace;
}

void QApplicationMainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    // \TODO aquí hauríem de controlar si l'aplicació està fent altres tasques pendents que s'haurien de finalitzar abans de tancar
    // l'aplicació com per exemple imatges en descàrrega del PACS o similar
    event->accept();
}

void QApplicationMainWindow::about()
{
    QMessageBox::about(this, tr("About Starviewer"),
            tr("<h2>Starviewer</h2>"
               "<p>Copyright &copy; 2005-2007 Grup de Gràfics Girona"
               "<p align='justify'>Starviewer is an image processing software dedicated to DICOM images produced by medical equipment (MRI,"
               " CT, PET, PET-CT...) It can also read many other file formats especified by the MetaIO estandard ( *.mhd files ). It is "
               "fully compliant with the DICOM standard for image comunication and image file formats. Starviewer is able to receive images "
               "transferred by DICOM communication protocol from any PACS or medical imaging modality (STORE SCP - Service Class Provider, "
               "STORE SCU - Service Class User, and Query/Retrieve)."
               "<p align='justify'>Starviewer has been specifically designed for navigation and visualization of multimodality and"
               " multidimensional images: 2D Viewer, 2D MPR ( Multiplanar reconstruction ) Viewer , 3D MPR Viewer and Hybrid MPR Viewer and"
               " Maximum Intensity Projection(MIP)."
               "<p align='justify'>Starviewer is at the same time a DICOM PACS workstation for medical imaging and an image processing"
               " software for medical research (radiology and nuclear imaging), functional imaging, 3D imaging, confocal microscopy and"
               " molecular imaging."
               "<p>Version : %1 </p>").arg( "Pre 0.4.0 (devel)" )
               );
}

void QApplicationMainWindow::writeSettings()
{
    QSettings settings;
    settings.beginGroup("Starviewer");

    settings.setValue( "position", pos() );
    settings.setValue( "size", size() );

    settings.endGroup();
}

void QApplicationMainWindow::enableExtensions()
{
    foreach(QAction *action, m_actionsList)
    {
        action->setEnabled( true );
    }
}

void QApplicationMainWindow::readSettings()
{
    QSettings settings;
    settings.beginGroup("Starviewer");

    move( settings.value( "position", QPoint(200, 200)).toPoint() );
    resize( settings.value( "size", QSize(400, 400)).toSize() );

    settings.endGroup();
}

}; // end namespace udg
