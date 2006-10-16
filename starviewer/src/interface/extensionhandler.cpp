/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "extensionhandler.h"

// qt
#include <QFileInfo>
#include <QDir>
#include <QProgressDialog>
// recursos
#include "volumerepository.h"
#include "input.h"
#include "output.h"
#include "extensionworkspace.h"
#include "qapplicationmainwindow.h"
#include "volumesourceinformation.h"

// Espai reservat pels include de les mini-apps
#include "appimportfile.h"
#include "qmprextension.h"
#include "qmpr3dextension.h"
#include "qmpr3d2dextension.h"
#include "q2dviewerextension.h"

// Fi de l'espai reservat pels include de les mini-apps

// PACS --------------------------------------------
#include "queryscreen.h"
#include "seriesvolum.h"

namespace udg {

ExtensionHandler::ExtensionHandler( QApplicationMainWindow *mainApp , QObject *parent, const char *name)
 : QObject(parent )
{
    this->setObjectName( name );
    m_volumeRepository = VolumeRepository::getRepository();
    m_inputReader = new Input;
    m_outputWriter = new Output;
    m_mainApp = mainApp;    
    
    // Aquí en principi només farem l'inicialització
    m_importFileApp = new AppImportFile;
    m_queryScreen = new QueryScreen( 0 );

    createConnections();
    registerExtensions();
}

ExtensionHandler::~ExtensionHandler()
{
}

void ExtensionHandler::createConnections()
{
    connect( m_importFileApp , SIGNAL( newVolume( Identifier ) ) , m_mainApp , SLOT( onVolumeLoaded( Identifier ) ) );
    
    connect( m_queryScreen , SIGNAL(viewStudy(StudyVolum)) , this , SLOT(viewStudy(StudyVolum)) );
    connect( m_mainApp->m_extensionWorkspace , SIGNAL( currentChanged(int) ) , this , SLOT( extensionChanged(int) ) );
}

void ExtensionHandler::registerExtensions()
{
}

void ExtensionHandler::request( int who )
{
// \TODO: crear l'extensió amb el factory ::createExtension, no com està ara
// \TODO la numeració és completament temporal!!! s'haurà de canviar aquest sistema
    switch( who )
    {
    
    case 1:
        if( m_volumeID.isNull() )
        {
        // open!
            m_importFileApp->open();
            m_importFileApp->finish();
        }
        else
        {
            // ara com li diem que en la nova finestra volem que s'executi la petició d'importar arxiu?
            m_mainApp->newAndOpen();
        }
    break;
    
    /// 2D MPR VIEW
    case 2:
        if( !m_volumeID.isNull() )
        {
            QMPRExtension *mprExtension = new QMPRExtension( 0 );
            mprExtension->setInput( m_volumeRepository->getVolume( m_volumeID ) );
            m_mainApp->m_extensionWorkspace->addApplication( mprExtension , tr("2D MPR") );
        }
        else
        {
            // ara com li diem que en la nova finestra volem que s'executi la petició d'importar arxiu?
        }
    break;
    
    /// MPR 3D VIEW
    case 3:
        if( !m_volumeID.isNull() )
        {
            QMPR3DExtension *mpr3DExtension = new QMPR3DExtension( 0 );
            mpr3DExtension->setInput( m_volumeRepository->getVolume( m_volumeID ) );
            m_mainApp->m_extensionWorkspace->addApplication( mpr3DExtension , tr("3D MPR") );
        }
        else
        {
            // ara com li diem que en la nova finestra volem que s'executi la petició d'importar arxiu?
        }
    break;
    
    /// MPR 3D-2D VIEW
    case 4:
        if( !m_volumeID.isNull() )
        {
            QMPR3D2DExtension *mpr3D2DExtension = new QMPR3D2DExtension( 0 );
            mpr3D2DExtension->setInput( m_volumeRepository->getVolume( m_volumeID ) );
            m_mainApp->m_extensionWorkspace->addApplication( mpr3D2DExtension , tr("3D-2D MPR") );
        }
        else
        {
            // ara com li diem que en la nova finestra volem que s'executi la petició d'importar arxiu?
        }
    break;

    case 6:
        if( m_volumeID.isNull() )
        {
            // open dicom dir
            m_importFileApp->openDirectory();
            m_importFileApp->finish();
        }
        else
        {
            m_mainApp->newAndOpenDir();
        }
    break;
    
    case 7:
        m_queryScreen->show();
    break;

    /// Default viewer: 2D Viewer
    case 8:
    {
        Q2DViewerExtension *defaultViewerExtension = new Q2DViewerExtension;
        defaultViewerExtension->setInput( m_volumeRepository->getVolume( m_volumeID ) );
        m_mainApp->m_extensionWorkspace->addApplication( defaultViewerExtension , tr("2D Viewer"));
//         defaultViewerExtension->populateToolBar( m_mainApp->getExtensionsToolBar() );
        connect( defaultViewerExtension , SIGNAL( newSerie() ) , this , SLOT( openSerieToCompare() ) );
        connect( this , SIGNAL( secondInput(Volume*) ) , defaultViewerExtension , SLOT( setSecondInput(Volume*) ) );
    
        break;
    }
    default:
    {
        Q2DViewerExtension *defaultViewerExtension2 = new Q2DViewerExtension;
        defaultViewerExtension2->setInput( m_volumeRepository->getVolume( m_volumeID ) );
        m_mainApp->m_extensionWorkspace->addApplication( defaultViewerExtension2 , tr("2D Viewer"));
//         defaultViewerExtension2->populateToolBar( m_mainApp->getExtensionsToolBar() );
        connect( defaultViewerExtension2 , SIGNAL( newSerie() ) , this , SLOT( openSerieToCompare() ) );
        connect( this , SIGNAL( secondInput(Volume*) ) , defaultViewerExtension2 , SLOT( setSecondInput(Volume*) ) );
    
        break;
    }
    }
}

void ExtensionHandler::killBill()
{
    if( !m_volumeID.isNull() )
    {
        m_volumeRepository->removeVolume( m_volumeID );
    }
    if( !m_compareVolumeID.isNull() )
    {
        m_volumeRepository->removeVolume( m_compareVolumeID );
    }
}

void ExtensionHandler::openSerieToCompare()
{
    QueryScreen *queryScreen = new QueryScreen;
    connect( queryScreen , SIGNAL( viewStudy(StudyVolum) ) , this , SLOT( viewStudyToCompare(StudyVolum) ) );
    queryScreen->show();
}

void ExtensionHandler::request( const QString &who )
{
}

void ExtensionHandler::onVolumeLoaded( Identifier id )
{
    m_volumeID = id;
    request( 8 );
}

bool ExtensionHandler::open( QString fileName )
{
    bool ok = true; 
    
    if ( m_volumeID.isNull() )
    {
        // indiquem que ens obri el fitxer
        if( QFileInfo( fileName ).suffix() == "dcm") // petita prova per provar lectura de DICOM's
        {
            if( m_inputReader->readSeries( QFileInfo(fileName).dir().absolutePath().toLatin1() ) )
            { 
                // creem el volum
                Volume *dummyVolume = m_inputReader->getData();
                // afegim el nou volum al repositori
                m_volumeID = m_volumeRepository->addVolume( dummyVolume );            
                request(8);
            }
            else
            {
                // no s'ha pogut obrir l'arxiu per algun motiu
                ok = false;
            }
        }
        else
        {
            if( m_inputReader->openFile( fileName.toLatin1() ) )
            { 
                // creem el volum
                Volume *dummyVolume = m_inputReader->getData();
                // afegim el nou volum al repositori
                m_volumeID = m_volumeRepository->addVolume( dummyVolume );            
                request(8);
            }
            else
            {
                // no s'ha pogut obrir l'arxiu per algun motiu
                ok = false;
            }
        }        
    }
    else
    {
        //Si ja tenim obert un model, obrim una finestra nova ???
    } 
    return ok;  
}

void ExtensionHandler::viewStudy( StudyVolum study )
{
    Input *input = new Input;
    QProgressDialog progressDialog( m_mainApp );
    progressDialog.setModal( true );
    progressDialog.setRange( 0 , 100 );
    progressDialog.setMinimumDuration( 0 );
    progressDialog.setWindowTitle( tr("Serie loading") );
    // atenció: el missatge triga una miqueta a aparèixer...
    progressDialog.setLabelText( tr("Loading, please wait...") );
    progressDialog.setCancelButton( 0 );
    connect( input , SIGNAL( progress(int) ) , &progressDialog , SLOT( setValue(int) ) );
    SeriesVolum serie;
    
    m_mainApp->setCursor( QCursor(Qt::WaitCursor) );
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

	if ( serie.getNumberOfImages() > 1)
	{
    	input->readSeries( serie.getSeriesPath().c_str() );
    }
	else if ( serie.getNumberOfImages() == 1 )
	{   //en el cas que nomes tingui una imatge la serie, fem retornar el vector amb el Path i agafem el de la primera imatge
		input->openFile( serie.getVectorSeriesPath()[0].c_str() );
	}

    Volume *dummyVolume = input->getData();
    if( !m_volumeID.isNull() )
    {
        Identifier id;
        id = m_volumeRepository->addVolume( dummyVolume );
        // obrir nova finestra
        QString windowName;    
        QApplicationMainWindow *newMainWindow = new QApplicationMainWindow( 0, qPrintable(windowName.sprintf( "NewWindow[%d]" , m_mainApp->getCountQApplicationMainWindow() + 1 ) ) );
        newMainWindow->show();
        newMainWindow->onVolumeLoaded( id );
        newMainWindow->setWindowTitle( dummyVolume->getVolumeSourceInformation()->getPatientName() + QString( " : " ) + dummyVolume->getVolumeSourceInformation()->getPatientID() );
    }
    else
    {
        m_volumeID = m_volumeRepository->addVolume( dummyVolume );
        m_mainApp->onVolumeLoaded( m_volumeID );
        m_mainApp->setWindowTitle( dummyVolume->getVolumeSourceInformation()->getPatientName() + QString( " : " ) + dummyVolume->getVolumeSourceInformation()->getPatientID() );
    }    
    m_mainApp->setCursor( QCursor(Qt::ArrowCursor) );
}

void ExtensionHandler::viewStudyToCompare( StudyVolum study )
{
    Input *input = new Input;
    QProgressDialog progressDialog;
    progressDialog.setRange( 0 , 100 );
    progressDialog.setMinimumDuration( 0 );
    progressDialog.setWindowTitle( tr("Serie loading") );
    // atenció: el missatge triga una miqueta a aparèixer...
    progressDialog.setLabelText( tr("Loading, please wait...") );
    progressDialog.setCancelButton( 0 );
    connect( input , SIGNAL( progress(int) ) , &progressDialog , SLOT( setValue(int) ) );
    SeriesVolum serie;
    
    m_mainApp->setCursor( QCursor(Qt::WaitCursor) );
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

	if ( serie.getNumberOfImages() > 1 )
	{
    	input->readSeries( serie.getSeriesPath().c_str() );
    }
	else if ( serie.getNumberOfImages() == 1 )
	{   //en el cas que nomes tingui una imatge la serie, fem retornar el vector amb el Path i agafem el de la primera imatge
		input->openFile( serie.getVectorSeriesPath()[0].c_str() );
	}

    if( !m_compareVolumeID.isNull() )
    {
        m_volumeRepository->removeVolume( m_compareVolumeID );
    }
    Volume *dummyVolume = input->getData();
    m_compareVolumeID = m_volumeRepository->addVolume( dummyVolume );
    m_mainApp->setCursor( QCursor(Qt::ArrowCursor) );
    emit secondInput( dummyVolume );    
}

void ExtensionHandler::extensionChanged( int index )
{
    // quan canvia una extensió hem de canviar les toolbars, per tan hem de mirar com fer-ho perque no sabem quina extensió ( sí podem obtenir el widget ) en concret és la que tenim. Ho podríem fer mitjançant signals i slots. és a dir, quan es faci el canvi d'extensió s'enviarà alguna senyal que farà que netejem la toolbar d¡extensions i que s'ompli amb els nous botons i eines
    m_mainApp->m_extensionWorkspace->setLastIndex( index );
}

};  // end namespace udg 
