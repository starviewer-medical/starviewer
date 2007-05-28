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
#include <QMessageBox>
// recursos
#include "volumerepository.h"
#include "extensionworkspace.h"
#include "qapplicationmainwindow.h"
#include "volumesourceinformation.h"

#include "extensionmediatorfactory.h"
#include "extensionfactory.h"
#include <QDebug>

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
#include "input.h"

namespace udg {

ExtensionHandler::ExtensionHandler( QApplicationMainWindow *mainApp , QObject *parent, const char *name)
 : QObject(parent )
{
    this->setObjectName( name );
    m_volumeRepository = VolumeRepository::getRepository();
    m_mainApp = mainApp;

    // Aquí en principi només farem l'inicialització
    m_importFileApp = new AppImportFile;
    m_queryScreen = new QueryScreen( m_mainApp );

    createConnections();
    registerExtensions();
}

ExtensionHandler::~ExtensionHandler()
{
}

void ExtensionHandler::createConnections()
{
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
        switch( m_importFileApp->open() )
        {
        case Input::NoError:
            // Si carreguem una imatge i ja en tenim una de carregada cal crear una finestra nova
            if( m_volumeID.isNull() )
            {
                m_mainApp->onVolumeLoaded( m_importFileApp->getVolumeIdentifier() );
                m_mainApp->setWindowTitle( m_importFileApp->getLastOpenedFilename() );
            }
            else
            {
            // ara com li diem que en la nova finestra volem que s'executi la petició d'importar arxiu?
                m_mainApp->newAndOpen();
            }
            break;

        case Input::InvalidFileName:
            break;

        case Input::SizeMismatch:
            break;
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
        }
    break;

    case 6:
        if( m_volumeID.isNull() )
        {
            // open dicom dir
            if( m_importFileApp->openDirectory() )
                m_mainApp->onVolumeLoaded( m_importFileApp->getVolumeIdentifier() );
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
        this->load2DViewerExtension();
        break;

    default:
        this->load2DViewerExtension();
        break;
    }
}

void ExtensionHandler::load2DViewerExtension()
{
    Q2DViewerExtension *defaultViewerExtension = new Q2DViewerExtension;
    defaultViewerExtension->setInput( m_volumeRepository->getVolume( m_volumeID ) );
    m_mainApp->m_extensionWorkspace->addApplication( defaultViewerExtension , tr("2D Viewer"));
//         defaultViewerExtension->populateToolBar( m_mainApp->getExtensionsToolBar() );
    connect( defaultViewerExtension , SIGNAL( newSerie() ) , this , SLOT( openSerieToCompare() ) );
    connect( this , SIGNAL( secondInput(Volume*) ) , defaultViewerExtension , SLOT( setSecondInput(Volume*) ) );
    connect( m_queryScreen, SIGNAL(viewKeyImageNote( const QString& )), defaultViewerExtension, SLOT(loadKeyImageNote( const QString& )));
    connect( m_queryScreen, SIGNAL(viewPresentationState(const QString &)),
             defaultViewerExtension, SLOT(loadPresentationState(const QString &) ));
    connect( m_importFileApp, SIGNAL(openKeyImageNote( const QString& )), defaultViewerExtension, SLOT(loadKeyImageNote( const QString& )));
    connect( m_importFileApp, SIGNAL(openPresentationState( const QString& )),
             defaultViewerExtension, SLOT(loadPresentationState( const QString& )));
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
    QueryScreen *queryScreen = new QueryScreen( m_mainApp );
    connect( queryScreen , SIGNAL( viewStudy(StudyVolum) ) , this , SLOT( viewStudyToCompare(StudyVolum) ) );
    queryScreen->show();
}



void ExtensionHandler::openPerfusionImage()
{
//     QueryScreen * queryScreen = new QueryScreen( m_mainApp );
    disconnect( m_queryScreen, SIGNAL( viewStudy(StudyVolum) ),
                this, SLOT( viewStudy(StudyVolum) ) );
    connect( m_queryScreen, SIGNAL( viewStudy(StudyVolum) ),
             this, SLOT( viewStudyForPerfusion(StudyVolum) ) );

    m_queryScreen->show();
}



void ExtensionHandler::request( const QString &who )
{
    QWidget *extension = ExtensionFactory::instance()->create(who);
    ExtensionMediator* mediator = ExtensionMediatorFactory::instance()->create(who);

    if (mediator && extension)
    {
        mediator->initializeExtension(extension, this, m_volumeID);
        m_mainApp->m_extensionWorkspace->addApplication(extension, mediator->getExtensionID().getLabel() );
    }
    else
    {
        qDebug() << "Error carregant " + who;
    }
}

void ExtensionHandler::onVolumeLoaded( Identifier id )
{
    m_volumeID = id;
    request( 8 );
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
    switch( input->readFiles( serie.getVectorImagePath() ) )
    {
    case Input::NoError:
    {
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
    break;

    case Input::InvalidFileName:
        QMessageBox::critical( m_mainApp, tr("Error"), tr("Invalid path or filename/s") );
        m_mainApp->setCursor( QCursor(Qt::ArrowCursor) );
        break;

    case Input::SizeMismatch:
        QMessageBox::critical( m_mainApp, tr("Error"), tr("Images of different size in the same serie. Open the images of the serie separately") );
        m_mainApp->setCursor( QCursor(Qt::ArrowCursor) );
        break;
    }

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
    switch( input->readFiles( serie.getVectorImagePath() ) )
    {
    case Input::NoError:
    {
        if( !m_compareVolumeID.isNull() )
        {
            m_volumeRepository->removeVolume( m_compareVolumeID );
        }
        Volume *dummyVolume = input->getData();
        m_compareVolumeID = m_volumeRepository->addVolume( dummyVolume );
        m_mainApp->setCursor( QCursor(Qt::ArrowCursor) );
        emit secondInput( dummyVolume );
    break;
    }
    case Input::InvalidFileName:
        QMessageBox::critical( m_mainApp, tr("Error"), tr("Invalid path or filename/s") );
        m_mainApp->setCursor( QCursor(Qt::ArrowCursor) );
        break;

    case Input::SizeMismatch:
        QMessageBox::critical( m_mainApp, tr("Error"), tr("Images of different size in the same serie. Open the images of the serie separately") );
        m_mainApp->setCursor( QCursor(Qt::ArrowCursor) );
        break;

    case Input::UnknownError:
        break;
    }
}

void ExtensionHandler::viewStudyForPerfusion( StudyVolum study )
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
    input->readFiles( serie.getVectorImagePath() );

    if( !m_compareVolumeID.isNull() )
    {
        m_volumeRepository->removeVolume( m_compareVolumeID );
    }

    Volume * dummyVolume = input->getData();
    m_compareVolumeID = m_volumeRepository->addVolume( dummyVolume );
    m_mainApp->setCursor( QCursor( Qt::ArrowCursor ) );

    emit perfusionImage( dummyVolume );

    disconnect( m_queryScreen , SIGNAL( viewStudy(StudyVolum) ),
                this, SLOT( viewStudyForPerfusion(StudyVolum) ) );
    connect( m_queryScreen, SIGNAL( viewStudy(StudyVolum) ),
             this, SLOT( viewStudy(StudyVolum) ) );
}

void ExtensionHandler::extensionChanged( int index )
{
    // quan canvia una extensió hem de canviar les toolbars, per tan hem de mirar com fer-ho perque no sabem quina extensió ( sí podem obtenir el widget ) en concret és la que tenim. Ho podríem fer mitjançant signals i slots. és a dir, quan es faci el canvi d'extensió s'enviarà alguna senyal que farà que netejem la toolbar d¡extensions i que s'ompli amb els nous botons i eines
    m_mainApp->m_extensionWorkspace->setLastIndex( index );
}

};  // end namespace udg
