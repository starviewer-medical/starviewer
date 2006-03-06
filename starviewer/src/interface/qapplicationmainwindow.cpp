/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

// qt
#include <qaction.h>
#include <qapplication.h>
#include <qfiledialog.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qsettings.h>
#include <qstatusbar.h>
#include <qdockwindow.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qmessagebox.h>
#include <qprogressdialog.h>
#include <qfileinfo.h> //Per m_workingDirectory
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qwidgetlist.h>
#include <qsignalmapper.h>
#include <qcursor.h>

// els nostres widgets/elements de la plataforma
#include "qapplicationmainwindow.h"
#include "volumerepository.h"
#include "identifier.h"
#include "volume.h"
#include "input.h"
#include "output.h"
#include "extensionhandler.h"
#include "extensionworkspace.h"

// Mini - aplicacions
#include "qtabaxisview.h"

// ------------------------------------------------------------------------------------
// Inici zona de codi editable [ #include's dels forward delcaration ]
// ------------------------------------------------------------------------------------

// Tot això acabarà fora en una mini-aplicació a part
// include's afegits
// #include "mutualinformationdirector.h"
// #include "mutualinformationparameters.h"
// #include "mutualinformationinputparametersform.h"

// ------------------------------------------------------------------------------------
// Fi zona de codi editable
// ------------------------------------------------------------------------------------
#include "queryscreen.h" // temporal!
namespace udg{



QApplicationMainWindow::QApplicationMainWindow(QWidget *parent, const char *name)
    : QMainWindow(parent, name, WDestructiveClose)
{
    m_extensionWorkspace = new ExtensionWorkspace(this);
    setCentralWidget( m_extensionWorkspace );

    m_extensionHandler = new ExtensionHandler( this );
    
    m_queryScreen = new QueryScreen( 0 );
    connect( m_queryScreen , SIGNAL(viewStudy(StudyVolum)) , this , SLOT(viewStudy(StudyVolum)) );
    // ------------------------------------------------------------------------------------
    // aquí creem el repositori de volums i l'objecte input per poder accedir als arxius
    // ------------------------------------------------------------------------------------
     
    m_volumeRepository = udg::VolumeRepository::getRepository();
    m_inputReader = new udg::Input;
    m_outputWriter = new udg::Output;
    // aquesta barra de progrés és provisional. Això anirà més lligat dins de les mini-apps
    m_progressDialog = new QProgressDialog(tr("Loading image data...") , tr("Abort") , 100 , this , tr("Progress") , TRUE  );
    m_progressDialog->setMinimumDuration( 0 );
    m_progressDialog->setCaption( tr("Caption") );
    m_progressDialog->setAutoClose( TRUE );

    /** Aquí podríem tenir un showProgress per cada tipu d'acció, la de carregar i la de desar
    ja que el missatge del progress dialog podria ser diferent, per exemple.
    Podríem tenir un showLoadProgress i un showSaveProgress
    */ 
    connect( m_inputReader , SIGNAL( progress(int) ) , this , SLOT( showProgress(int) ) );
    connect( m_outputWriter , SIGNAL( progress(int) ) , this , SLOT( showProgress(int) ) );
    
    createActions();    
    createMenus();
    createToolBars();
    createStatusBar();
    
    // Llegim les configuracions de l'aplicació, estat de la finestra, posicio, últims
    // arxius oberts etc amb QSettings
    readSettings();
    // icona de l'aplicació
    setIcon( QPixmap::fromMimeSource("icon.png") );

    setCaption( tr("StarViewer") );
    m_openFileFilters = tr("MetaIO Images (*.mhd);;DICOM Images (*.dcm);;All Files (*)");
    m_exportFileFilters = tr("JPEG Images (*.jpg);;MetaIO Images (*.mhd);;DICOM Images (*.dcm);;All Files (*)");
    
    m_exportToJpegFilter = tr("JPEG Images (*.jpg)");
    m_exportToMetaIOFilter = tr("MetaIO Images (*.mhd)");
    m_exportToPngFilter = tr("PNG Images (*.png)");
    m_exportToTiffFilter = tr("TIFF Images (*.tiff)");
    m_exportToBmpFilter = tr("BMP Images (*.bmp)");
    m_exportToDicomFilter = tr("DICOM Images (*.dcm)");

    m_modified = false;
    m_self = this;                                    
    
    emit containsVolume( FALSE );
}

QApplicationMainWindow::~QApplicationMainWindow()
{
    if( !m_volumeID.isNull() )
    {
        m_volumeRepository->removeVolume( m_volumeID );
    }
}

void QApplicationMainWindow::viewStudy( StudyVolum study )
{
    Input *input = new Input;
    SeriesVolum serie;
    
    this->setCursor( QCursor(Qt::WaitCursor) );
    study.firstSerie();
    while ( !study.end() )
    {
        if ( study.getDefaultSeriesUID() == study.getSeriesVolum().getSeriesUID() )
        {
            break;
        }
        study.nextSerie();
    }
    if ( study.end() )
    { 
        //si no l'hem trobat per defecte mostrarem la primera serie
        study.firstSerie();
    }
    
    serie = study.getSeriesVolum();

    input->readSeries( serie.getSeriesPath().c_str() );
    
    udg::Volume *dummyVolume = input->getData();
    m_volumeID = m_volumeRepository->addVolume( dummyVolume );
    m_extensionHandler->setVolumeID( m_volumeID );    
    m_extensionHandler->request( 2 );

    this->setCursor( QCursor(Qt::ArrowCursor) );    

}

void QApplicationMainWindow::createActions()
{
    QSignalMapper* signalMapper = new QSignalMapper(this);
    connect( signalMapper, SIGNAL( mapped(int) ), m_extensionHandler , SLOT( request(int) ) );
    connect( signalMapper, SIGNAL( mapped( const QString) ), m_extensionHandler , SLOT( request(const QString) ) );
    
    m_newAction = new QAction(this);
    m_newAction->setMenuText(tr("&New"));
    m_newAction->setAccel( tr("Ctrl+N") );
    m_newAction->setStatusTip(tr("Open a new working window"));
    m_newAction->setIconSet(QPixmap::fromMimeSource("new.png"));
    connect( m_newAction, SIGNAL( activated() ), this, SLOT( newFile()) );

    m_openAction = new QAction(this);
    m_openAction->setMenuText(tr("&Open..."));
    m_openAction->setAccel( tr("Ctrl+O") );
    m_openAction->setStatusTip(tr("Open an existing volume file"));
    m_openAction->setIconSet(QPixmap::fromMimeSource("open.png"));
    
    signalMapper->setMapping( m_openAction , 1 );
    signalMapper->setMapping( m_openAction , "Open File" );
    connect( m_openAction , SIGNAL( activated() ) , signalMapper , SLOT( map() ) );
    
    m_pacsAction = new QAction(this);
    m_pacsAction->setMenuText(tr("&PACS..."));
    m_pacsAction->setAccel( tr("Ctrl+P") );
    m_pacsAction->setStatusTip(tr("Open PACS Query Screen"));
    m_pacsAction->setIconSet( QPixmap::fromMimeSource("find.png") );
    connect( m_pacsAction, SIGNAL(activated()), this, SLOT( pacsQueryScreen() ) );

    m_mpr2DAction = new QAction( this );
    m_mpr2DAction->setMenuText( tr("2D &MPR Viewer") );
    m_mpr2DAction->setAccel( tr("Ctrl+M") );
    m_mpr2DAction->setStatusTip( tr("Open the 2D MPR Application Viewer") );
    signalMapper->setMapping( m_mpr2DAction , 2 );
    signalMapper->setMapping( m_mpr2DAction , "2D MPR" );
    connect( m_mpr2DAction , SIGNAL( activated() ) , signalMapper , SLOT( map() ) );

    m_mpr3DAction = new QAction( this );
    m_mpr3DAction->setMenuText( tr("3D M&PR Viewer") );
    m_mpr3DAction->setAccel( tr("Ctrl+P") );
    m_mpr3DAction->setStatusTip( tr("Open the 3D MPR Application Viewer") );
    signalMapper->setMapping( m_mpr3DAction , 3 );
    signalMapper->setMapping( m_mpr3DAction , "3D MPR" );
    connect( m_mpr3DAction , SIGNAL( activated() ) , signalMapper , SLOT( map() ) );

    m_mpr3D2DAction = new QAction( this );
    m_mpr3D2DAction->setMenuText( tr("3D-2D MP&R Viewer") );
    m_mpr3D2DAction->setAccel( tr("Ctrl+R") );
    m_mpr3D2DAction->setStatusTip( tr("Open the 3D-2D MPR Application Viewer") );
    signalMapper->setMapping( m_mpr3D2DAction , 4 );
    signalMapper->setMapping( m_mpr3D2DAction , "3D-2D MPR" );
    connect( m_mpr3D2DAction , SIGNAL( activated() ) , signalMapper , SLOT( map() ) );
    
    m_exportToJpegAction = new QAction( this );
    m_exportToJpegAction->setMenuText(tr("Export to JPEG"));
    m_exportToJpegAction->setAccel( 0 );
    m_exportToJpegAction->setStatusTip( tr("Export the volume to jpeg format") );
    connect( m_exportToJpegAction , SIGNAL( activated() ) , this , SLOT( exportToJpeg() ) );
    connect( this , SIGNAL( containsVolume(bool) ), m_exportToJpegAction, SLOT( setEnabled(bool) ) );
    
    m_exportToMetaIOAction = new QAction( this );
    m_exportToMetaIOAction->setMenuText(tr("Export to MetaIO"));
    m_exportToMetaIOAction->setAccel( 0 );
    m_exportToMetaIOAction->setStatusTip( tr("Export the volume to MetaIO format") );
    connect( m_exportToMetaIOAction , SIGNAL( activated() ) , this , SLOT( exportToMetaIO() ) );
    connect( this , SIGNAL( containsVolume(bool) ), m_exportToMetaIOAction, SLOT( setEnabled(bool) ) );
    
    m_exportToPngAction = new QAction( this );
    m_exportToPngAction->setMenuText(tr("Export to PNG"));
    m_exportToPngAction->setAccel( 0 );
    m_exportToPngAction->setStatusTip( tr("Export the volume to png format") );
    connect( m_exportToPngAction , SIGNAL( activated() ) , this , SLOT( exportToPng() ) );
    connect( this , SIGNAL( containsVolume(bool) ), m_exportToPngAction, SLOT( setEnabled(bool) ) );
    
    m_exportToTiffAction = new QAction( this );
    m_exportToTiffAction->setMenuText(tr("Export to TIFF"));
    m_exportToTiffAction->setAccel( 0 );
    m_exportToTiffAction->setStatusTip( tr("Export the volume to tiff format") );
    connect( m_exportToTiffAction , SIGNAL( activated() ) , this , SLOT( exportToTiff() ) );
    connect( this , SIGNAL( containsVolume(bool) ), m_exportToTiffAction, SLOT( setEnabled(bool) ) );
    
    m_exportToBmpAction = new QAction( this );
    m_exportToBmpAction->setMenuText(tr("Export to BMP"));
    m_exportToBmpAction->setAccel( 0 );
    m_exportToBmpAction->setStatusTip( tr("Export the volume to bmp format") );
    connect( m_exportToBmpAction , SIGNAL( activated() ) , this , SLOT( exportToBmp() ) );
    connect( this , SIGNAL( containsVolume(bool) ), m_exportToBmpAction, SLOT( setEnabled(bool) ) );
    
    m_aboutAction = new QAction(this);
    m_aboutAction->setMenuText(tr("&About") );
    m_aboutAction->setAccel( 0 );
    m_aboutAction->setStatusTip(tr("Show the application's About box"));
    connect(m_aboutAction, SIGNAL(activated()), this, SLOT(about()));

    m_aboutQtAction = new QAction(this);
    m_aboutQtAction->setMenuText( tr("About &Qt") );
    m_aboutQtAction->setAccel(0);
    m_aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
    connect(m_aboutQtAction, SIGNAL(activated()), qApp, SLOT(aboutQt()));
    
    m_closeAction = new QAction(tr("&Close"), tr("Ctrl+W"), this);
    m_closeAction->setMenuText( tr("&Close") );
    m_closeAction->setAccel( tr("Ctrl+W") );
    m_closeAction->setStatusTip(tr("Close the current volume"));
    m_closeAction->setIconSet(QPixmap::fromMimeSource("fileclose.png"));    
    connect( m_closeAction, SIGNAL( activated() ), this, SLOT( close() ) );            
    connect( this , SIGNAL( containsVolume(bool) ), m_closeAction, SLOT( setEnabled(bool) ) );
    
    m_exitAction = new QAction(this);
    m_exitAction->setMenuText( tr("E&xit") );
    m_exitAction->setAccel(tr("Ctrl+Q") );
    m_exitAction->setStatusTip(tr("Exit the application"));
    m_exitAction->setIconSet(QPixmap::fromMimeSource("exit.png"));    
    connect(m_exitAction, SIGNAL(activated()), qApp, SLOT(closeAllWindows()));
    
}

void QApplicationMainWindow::insertApplicationAction( QAction *action , OperationsType operation , bool toToolBar )
{
    // pre: s'han creat els menús sinó petarà!
    switch( operation )
    {
    case Segmentation:
        action->addTo( m_segmentationMenu );
        if( toToolBar ) action->addTo( m_segmentationToolBar );
    break;
    case Registration:
        action->addTo( m_registrationMenu );
        if( toToolBar ) action->addTo( m_registrationToolBar );
    break;
    case Clustering:
        action->addTo( m_clusteringMenu );
        if( toToolBar ) action->addTo( m_clusteringToolBar );
    break;
    case Color:
        action->addTo( m_colorMenu );
        if( toToolBar ) action->addTo( m_colorToolBar );
    break;
    }
}

void QApplicationMainWindow::exportFile( int type )
{
    switch( type )
    {
    case QApplicationMainWindow::JpegExport:
        exportToJpeg();
    break;
    case QApplicationMainWindow::MetaIOExport:
        exportToMetaIO();
    break;
    case QApplicationMainWindow::PngExport:
        exportToPng();
    break;
    case QApplicationMainWindow::TiffExport:
        exportToTiff();
    break;
    case QApplicationMainWindow::BmpExport:
        exportToBmp();
    break;
    }
}

void QApplicationMainWindow::exportToJpeg( )
{
    QString fileName =
            QFileDialog::getSaveFileName( m_exportWorkingDirectory, m_exportToJpegFilter, this, tr("Export file dialog"),tr("Chose an image filename") );
    if ( !fileName.isEmpty() )
    {
        std::cout << "Extension::" << QFileInfo( fileName ).extension() << std::endl;
        if( QFileInfo( fileName ).extension() != "jpg" )
        {
            fileName += ".jpg";
        }
        
        Output *out = new Output();
        // aquí cladria recòrrer les llesques per guardar per separat en un fitxer cadascuna
        out->setInput( m_volumeRepository->getVolume( this->getVolumeID() ) );
        out->saveSeries( fileName.latin1() );
        m_exportWorkingDirectory = QFileInfo( fileName ).dirPath();
    }

}

void QApplicationMainWindow::exportToPng( )
{
    QString fileName =
            QFileDialog::getSaveFileName( m_exportWorkingDirectory, m_exportToPngFilter, this, tr("Export file dialog"),tr("Chose an image filename") );
    if ( !fileName.isEmpty() )
    {
        if( QFileInfo( fileName ).extension() != "png" )
        {
            fileName += ".png";
        }      
        Output *out = new Output();
        // aquí cladria recòrrer les llesques per guardar per separat en un fitxer cadascuna
        out->setInput( m_volumeRepository->getVolume( this->getVolumeID() ) );
        out->saveSeries( fileName.latin1() );
        m_exportWorkingDirectory = QFileInfo( fileName ).dirPath();
    }
}

void QApplicationMainWindow::exportToTiff( )
{
    QString fileName =
            QFileDialog::getSaveFileName( m_exportWorkingDirectory, m_exportToTiffFilter, this, tr("Export file dialog"),tr("Chose an image filename") );
    if ( !fileName.isEmpty() )
    {
        if( QFileInfo( fileName ).extension() != "tiff" )
        {
            fileName += ".tiff";
        }
        
        Output *out = new Output();
        // aquí cladria recòrrer les llesques per guardar per separat en un fitxer cadascuna
        out->setInput( m_volumeRepository->getVolume( this->getVolumeID() ) );
        out->saveSeries( fileName.latin1() );
        m_exportWorkingDirectory = QFileInfo( fileName ).dirPath();
    }
}

void QApplicationMainWindow::exportToBmp( )
{
    QString fileName =
            QFileDialog::getSaveFileName( m_exportWorkingDirectory, m_exportToBmpFilter, this, tr("Export file dialog"),tr("Chose an image filename") );
    if ( !fileName.isEmpty() )
    {
        if( QFileInfo( fileName ).extension() != "bmp" )
        {
            fileName += ".bmp";
        }
        
        Output *out = new Output();
        // aquí caldria recòrrer les llesques per guardar per separat en un fitxer cadascuna
        out->setInput( m_volumeRepository->getVolume( this->getVolumeID() ) );
        out->saveSeries( fileName.latin1() );
        m_exportWorkingDirectory = QFileInfo( fileName ).dirPath();
    }
}

void QApplicationMainWindow::exportToMetaIO( )
{
    QString fileName =
            QFileDialog::getSaveFileName( m_exportWorkingDirectory, m_exportToMetaIOFilter, this, tr("Export file dialog"),tr("Chose an image filename") );
    if (!fileName.isEmpty())
    {
        if( QFileInfo( fileName ).extension() != "mhd" )
        {
            fileName += ".mhd";
        }
        Output *out = new Output();
        out->setInput( m_volumeRepository->getVolume( this->getVolumeID() ) );
        out->saveFile( fileName.latin1() );
        m_exportWorkingDirectory = QFileInfo( fileName ).dirPath();
    }
}

void QApplicationMainWindow::createMenus()
{
// Menú d'arxiu: aquest es correspondrà a l'accés directe al sistema de fitxers per adquirir un volum, com pot ser un arxiu *.mhd
    m_fileMenu = new QPopupMenu(this);
    m_newAction->addTo(m_fileMenu);
    m_openAction->addTo(m_fileMenu); 
    m_pacsAction->addTo( m_fileMenu );
    m_fileMenu->insertSeparator();
    
    m_importFilesMenu = new QPopupMenu(this);
    m_fileMenu->insertItem( tr("&Import"), m_importFilesMenu );    
    
    m_exportFilesMenu = new QPopupMenu(this);
    m_fileMenu->insertItem( tr("&Export"), m_exportFilesMenu );    
    m_exportToJpegAction->addTo( m_exportFilesMenu );
    m_exportToMetaIOAction->addTo( m_exportFilesMenu );
    m_exportToPngAction->addTo( m_exportFilesMenu );
    m_exportToBmpAction->addTo( m_exportFilesMenu );
    // l'export al tipus Tiff falla, pot ser cosa de les itk o del suport del sistema a aquest tipu de fitxer
    m_exportToTiffAction->addTo( m_exportFilesMenu );
    
    m_fileMenu->insertSeparator();
    
    m_recentFilesMenu = new QPopupMenu(this);
    m_fileMenu->insertItem( tr("&Recent files"), m_recentFilesMenu );    
    
    m_fileMenu->insertSeparator();
    m_closeAction->addTo(m_fileMenu);
    m_exitAction->addTo(m_fileMenu);

    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        m_recentFileIds[i] = -1;
    }
// aquest menú es correspondrà amb la connexió al pacs
    m_databaseMenu = new QPopupMenu( this );
// accions relacionades amb la segmentació
    m_segmentationMenu = new QPopupMenu( this );
// accions relacionades amb clustering
    m_clusteringMenu = new QPopupMenu( this );
// accions relacionades amb el registre
    m_registrationMenu = new QPopupMenu( this );
// accions relacionades amb la visualització
    m_visualizationMenu = new QPopupMenu(this);
    m_mpr2DAction->addTo( m_visualizationMenu );
    m_mpr3DAction->addTo( m_visualizationMenu );
    m_mpr3D2DAction->addTo( m_visualizationMenu );
// accions relacionades amb tractament de color, funcions de transferència
    m_colorMenu = new QPopupMenu(this);
// accions relacionades amb tools
    m_toolsMenu = new QPopupMenu(this);
// menú finestra, controla organització de la workingarea i permet canviar a altres finestres obertes amb els pacients
    m_windowMenu = new QPopupMenu(this);
// menú d'opcions del programa
    m_optionsMenu = new QPopupMenu(this);
    //m_showToolBoxAction->addTo(m_optionsMenu);
// menú per escollir idioma
    m_languageMenu = new QPopupMenu(this);
    createLanguageMenu();  
// menú d'ajuda, ara només hi ha els típic abouts  
    m_helpMenu = new QPopupMenu(this);
    m_aboutAction->addTo(m_helpMenu);
    m_aboutQtAction->addTo(m_helpMenu);

    menuBar()->insertItem(tr("&File"), m_fileMenu);
    menuBar()->insertItem(tr("&Database"), m_databaseMenu);
    menuBar()->insertItem(tr("&Segmentation"), m_segmentationMenu);
    menuBar()->insertItem(tr("&Clustering"), m_clusteringMenu);
    menuBar()->insertItem(tr("&Registration"), m_registrationMenu);
    menuBar()->insertItem(tr("&Visualization"), m_visualizationMenu);
    menuBar()->insertItem(tr("Co&lor"), m_colorMenu);
    menuBar()->insertItem(tr("&Tools"), m_toolsMenu);
    menuBar()->insertItem(tr("&Options"), m_optionsMenu);
    menuBar()->insertItem(tr("&Window"), m_windowMenu);
    menuBar()->insertItem(tr("&Language"), m_languageMenu);
    menuBar()->insertSeparator();
    menuBar()->insertItem(tr("&Help"), m_helpMenu);
}

void QApplicationMainWindow::createToolBars()
{
    m_fileToolBar = new QToolBar(tr("File"), this);
    m_newAction->addTo(m_fileToolBar);
    m_openAction->addTo(m_fileToolBar);
    m_pacsAction->addTo(m_fileToolBar);
    
    m_databaseToolBar = new QToolBar( tr("Database") , this );
    
    m_optionsToolBar = new QToolBar( tr("Options"), this );    

    m_extensionsToolBar = new QToolBar( tr("Extensions") , this );
}


void QApplicationMainWindow::showProgress( int value )
{
    if( value == -1 )
    {
    // hi ha hagut una excepció en la lectura del fitxer, mostrar alerta
        m_progressDialog->cancel();
        QMessageBox::critical( this , tr("Error") , tr("Exception while reading file. Cannot open the specified file.") );
    }
    else
    {
        m_progressDialog->setProgress( value );
    }
}

void QApplicationMainWindow::newFile()
{
    QString windowName;    
    QApplicationMainWindow *newMainWindow = new QApplicationMainWindow( 0, windowName.sprintf( "NewWindow[%d]" ,getCountQApplicationMainWindow() + 1 ) );
    newMainWindow->show();
}

void QApplicationMainWindow::pacsQueryScreen()
{
    m_queryScreen->show();
}

void QApplicationMainWindow::close()
{
    if( !m_volumeID.isNull() )
    {
        // tancar el volum, és a dir treure'l del repositori, matar les mini-aplicacions que el fan servir o almenys alertar de si realment es vol fer ja que hi ha mini-apps que el faran servir conjuntament amb altres com el registre. Bona part d'això serà feina del mini-app handler
    }
    else
    {
        // error que no s'hauria de donar
    }
}

void QApplicationMainWindow::createLanguageMenu()
{
// // agafat de l'exemple de la pàg. 331 de "C++ GUI Programming with qt3"
//     QDir dir( m_qmPath );
//     QStringList fileNames = dir.entryList("starviewer_*.qm");
//     
//     std::cout << "filenames size :: " << fileNames.size() << std::endl;
//     
//     for( int i = 0; i < fileNames.size(); i++ )
//     {
//         QTranslator translator;
//         translator.load( fileNames[i], m_qmPath );
//         
//         QTranslatorMessage message = translator.findMessage("QApplicationMainWindow","English");
//         QString language = message.translation();
//         int id = m_languageMenu->insertItem( tr("&%1 %2").arg( i+1 ).arg(language) , this , SLOT( switchToLanguage( int ) ) );
//         m_languageMenu->setItemParameter( id , i );
//         if( language == "English" ) 
//         {
//             m_languageMenu->setItemChecked( id , true );
//         }
//         
//         QString locale = fileNames[i];
//         locale = locale.mid( locale.find('_') + 1 );
//         locale.truncate( locale.find('.') );
//         std::cout << locale << std::endl;
//         m_locales.push_back( locale );
//             
// //         if( locale == m_defaultLocale )
// //             m_languageMenu->setItemChecked( id , true );
//     }
}

unsigned int QApplicationMainWindow::getCountQApplicationMainWindow()
{
    QWidgetList *list = QApplication::topLevelWidgets();
    QWidgetListIt it( *list );
    QWidget *widget;
    unsigned int count = 0;
    
    while ( (widget = it.current()) != 0 )
    {
        if ( widget->isA("udg::QApplicationMainWindow") )
        {
            count++;
        }
        ++it;
    }
    delete list;
    
    return count;
}

void QApplicationMainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void QApplicationMainWindow::setCurrentFile(const QString &fileName)
{
    m_currentFile = fileName;
    m_modified = false;

    if (m_currentFile.isEmpty()) 
    {
        setCaption(tr("StarViewer"));
    } 
    else 
    {
        setCaption(tr("%1 - %2").arg(strippedName(m_currentFile))
                                .arg(tr("StarViewer")));
        m_recentFiles.remove(m_currentFile);
        m_recentFiles.push_front(m_currentFile);
        // això es fa perquè s'actualitzi a totes les qapplicationmainwindow que hàgim oberts
        QWidgetList *list = QApplication::topLevelWidgets();
        QWidgetListIt it( *list );
        QWidget *widget;
        while( widget = it.current() )
        {
            if( widget->inherits("QApplicationMainWindow")  )
                ( ( QApplicationMainWindow * ) widget )->updateRecentFileItems();
        }
    }
}

QString QApplicationMainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void QApplicationMainWindow::updateRecentFileItems()
{
    while ((int)m_recentFiles.size() > MaxRecentFiles)
        m_recentFiles.pop_back();

    for (int i = 0; i < (int)m_recentFiles.size(); ++i) 
    {
        QString text = tr("&%1 %2").arg(i + 1).arg(m_recentFiles[i]);
        if (m_recentFileIds[i] == -1) 
        {
            m_recentFileIds[i] = m_recentFilesMenu->insertItem(text, this, SLOT(openRecentFile(int)), 0, -1, m_recentFilesMenu->count() );
            m_recentFilesMenu->setItemParameter(m_recentFileIds[i], i);
        }
        else 
        {
            m_recentFilesMenu->changeItem(m_recentFileIds[i], text);
        }
    }
    
}

void QApplicationMainWindow::openRecentFile(int param)
{
//     if (maybeSave())
//         loadFile(m_recentFiles[param]);
}

void QApplicationMainWindow::createStatusBar()
{
    QLabel *locationLabel;
    QLabel *formulaLabel;
    QLabel *modLabel;
    
    locationLabel = new QLabel(" W999 ", this);
    locationLabel->setAlignment(AlignHCenter);
    locationLabel->setMinimumSize(locationLabel->sizeHint());

    formulaLabel = new QLabel(this);

    modLabel = new QLabel(tr(" MOD "), this);
    modLabel->setAlignment(AlignHCenter);
    modLabel->setMinimumSize(modLabel->sizeHint());
    modLabel->clear();

    statusBar()->addWidget(locationLabel);
    statusBar()->addWidget(formulaLabel, 1);
    statusBar()->addWidget(modLabel);
}

void QApplicationMainWindow::about()
{
    QMessageBox::about(this, tr("About StarViewer"),
            tr("<h2>StarViewer 2006 þBetaþ</h2>"
               "<p>Copyright &copy; 2004 Universitat de Girona"
               "<p>StarViewer is a small application that "
               "lets you view <b>DICOM</b>, <b>MHD's</b>,... "
               "files and manipulate them."
               "<p>Last Redisign Version : in early development")
               );
}

void QApplicationMainWindow::writeSettings()
{
    QSettings settings;
    settings.setPath("software-inc.com", "StarViewer");
    settings.beginGroup("/StarViewer");
    settings.writeEntry("/geometry/x", x());
    settings.writeEntry("/geometry/y", y());
    settings.writeEntry("/geometry/width", width());
    settings.writeEntry("/geometry/height", height());
    settings.writeEntry("/recentFiles", m_recentFiles);
    settings.writeEntry("/workingDirectory", m_workingDirectory );
    settings.writeEntry("/exportWorkingDirectory", m_exportWorkingDirectory );
    settings.writeEntry("/defaultLocale", m_defaultLocale );
    
    settings.endGroup();
}

void QApplicationMainWindow::readSettings()
{
    QSettings settings;
    settings.setPath("software-inc.com", "StarViewer");
    settings.beginGroup("/StarViewer");

    int x = settings.readNumEntry("/geometry/x", 200);
    int y = settings.readNumEntry("/geometry/y", 200);
    int w = settings.readNumEntry("/geometry/width", 400);
    int h = settings.readNumEntry("/geometry/height", 400);
    move(x, y);
    resize(w, h);

    m_recentFiles = settings.readListEntry("/recentFiles");
    updateRecentFileItems();

    m_workingDirectory = settings.readEntry("/workingDirectory", ".");
    m_exportWorkingDirectory = settings.readEntry("/exportWorkingDirectory", ".");
    m_defaultLocale = settings.readEntry("/defaultLocale", "en_GB" );
    
    settings.endGroup();
}

}; // end namespace udg
