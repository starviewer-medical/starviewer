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
// recursos
#include "volumerepository.h"
#include "input.h"
#include "output.h"
#include "extensionworkspace.h"
#include "qapplicationmainwindow.h"

// aplicacions
#include "extensionfactory2.h"
#include "qmprextensioncreator.h"
#include "qmpr3dextensioncreator.h"
#include "qmpr3d2dextensioncreator.h"
#include "qdefaultviewerextensioncreator.h"

// Espai reservat pels include de les mini-apps
#include "appimportfile.h"
#include "qmprextension.h"
#include "qmpr3dextension.h"
#include "qmpr3d2dextension.h"
#include "qdefaultviewerextension.h"

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
//     connect( m_importFileApp , SIGNAL( newVolume( Identifier ) ) , this , SLOT( onVolumeLoaded( Identifier ) ) );
//     connect( m_importFileApp , SIGNAL( newVolume( Identifier ) ) , m_mainApp , SLOT( onVolumeLoaded( Identifier ) ) );
    
    connect( m_queryScreen , SIGNAL(viewStudy(StudyVolum)) , this , SLOT(viewStudy(StudyVolum)) );
    connect( m_mainApp->m_extensionWorkspace , SIGNAL( currentChanged(int) ) , this , SLOT( extensionChanged(int) ) );
}

void ExtensionHandler::registerExtensions()
{
    // creem totes les instàncies dels creadors d'extensions
    m_qMPRExtensionCreator = new QMPRExtensionCreator( this );
    m_qMPR3DExtensionCreator = new QMPR3DExtensionCreator( this );
    m_qMPR3D2DExtensionCreator = new QMPR3D2DExtensionCreator( this );
    m_qDefaultViewerExtensionCreator = new QDefaultViewerExtensionCreator( this );
    
    // al crear-se el handler inicialitzem el factory amb totes les aplicacions
    m_extensionFactory = new ExtensionFactory(this);
    m_extensionFactory->registerExtension( "2D MPR Extension" , m_qMPRExtensionCreator );
    m_extensionFactory->registerExtension( "3D MPR Extension" , m_qMPR3DExtensionCreator );
    m_extensionFactory->registerExtension( "3D-2D MPR Extension" , m_qMPR3D2DExtensionCreator );
    m_extensionFactory->registerExtension( "Default Viewer Extension" , m_qDefaultViewerExtensionCreator );
}

void ExtensionHandler::request( int who )
{
// \TODO: crear l'extensió amb el factory ::createExtension, no com està ara
    QMPRExtension *mprExtension = new QMPRExtension( 0 );
    QMPR3DExtension *mpr3DExtension = new QMPR3DExtension( 0 );
    QMPR3D2DExtension *mpr3D2DExtension = new QMPR3D2DExtension( 0 );
    QDefaultViewerExtension *defaultViewerExtension;
    /// \TODO la numeració és completament temporal!!! s'haurà de canviar aquest sistema
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

    /// Default viewer
    case 8:
        defaultViewerExtension = new QDefaultViewerExtension;
        defaultViewerExtension->setInput( m_volumeRepository->getVolume( m_volumeID ) );
        m_mainApp->m_extensionWorkspace->addApplication( defaultViewerExtension , tr("Default Viewer"));
//         m_mainApp->addToolBar( defaultViewerExtension->getToolsToolBar() );
        defaultViewerExtension->populateToolBar( m_mainApp->getExtensionsToolBar() );
        connect( defaultViewerExtension , SIGNAL( newSerie() ) , this , SLOT( openSerieToCompare() ) );
        connect( this , SIGNAL( secondInput(Volume*) ) , defaultViewerExtension , SLOT( setSecondInput(Volume*) ) );
    break;
    
    default:
        defaultViewerExtension = new QDefaultViewerExtension;
        defaultViewerExtension->setInput( m_volumeRepository->getVolume( m_volumeID ) );
        m_mainApp->m_extensionWorkspace->addApplication( defaultViewerExtension , tr("Default Viewer"));
//         m_mainApp->addToolBar( defaultViewerExtension->getToolsToolBar() );
        defaultViewerExtension->populateToolBar( m_mainApp->getExtensionsToolBar() );
    break;
    }
}

void ExtensionHandler::killBill()
{
    if( !m_volumeID.isNull() )
    {
        m_volumeRepository->removeVolume( m_volumeID );
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
                request(5);
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
                request(5);
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
    }
    else
    {
        m_volumeID = m_volumeRepository->addVolume( dummyVolume );
        m_mainApp->onVolumeLoaded( m_volumeID );
    }    
    m_mainApp->setCursor( QCursor(Qt::ArrowCursor) );
}

void ExtensionHandler::viewStudyToCompare( StudyVolum study )
{
    Input *input = new Input;
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
    
    Volume *dummyVolume = input->getData();
    m_volumeID = m_volumeRepository->addVolume( dummyVolume );
    m_mainApp->setCursor( QCursor(Qt::ArrowCursor) );
    emit secondInput( dummyVolume );    
}

void ExtensionHandler::extensionChanged( int index )
{
    // quan canvia una extensió hem de canviar les toolbars, per tan hem de mirar com fer-ho perque no sabem quina extensió ( sí podem obtenir el widget ) en concret és la que tenim. Ho podríem fer mitjançant signals i slots. és a dir, quan es faci el canvi d'extensió s'enviarà alguna senyal que farà que netejem la toolbar d¡extensions i que s'ompli amb els nous botons i eines
    m_mainApp->clearExtensionsToolBar();
    m_mainApp->m_extensionWorkspace->setLastIndex( index );
}

};  // end namespace udg 
