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

// Espai reservat pels include de les mini-apps
#include "appimportfile.h"
#include "q2dviewerextension.h"

// Fi de l'espai reservat pels include de les mini-apps

// PACS --------------------------------------------
#include "queryscreen.h"
#include "seriesvolum.h"
#include "input.h"
#include "patientfiller.h"
#include "patientfillerinput.h"

namespace udg {

ExtensionHandler::ExtensionHandler( QApplicationMainWindow *mainApp , QObject *parent, QString name)
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
        DEBUG_LOG( "Error carregant " + who );
    }
}

void ExtensionHandler::onVolumeLoaded( Identifier id )
{
    m_volumeID = id;
    request( 8 );
}

void ExtensionHandler::viewPatient(PatientFillerInput patientFillerInput)
{
    // Proves per comprovar que funciona el tema dels FilterSteps sense molestar a la resta de la gent.
    // Descomentar per activar la càrrega de Patient
    PatientFiller patientFiller;
    patientFiller.fill( &patientFillerInput );
    DEBUG_LOG( "Labels: " + patientFillerInput.getLabels().join("; )"));
    DEBUG_LOG( "getNumberOfPatients: " + patientFillerInput.getNumberOfPatients());
    DEBUG_LOG( patientFillerInput.getPatient(0)->toString() );
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
    bool found = false;
    int i = 0;

    m_mainApp->setCursor( QCursor(Qt::WaitCursor) );
    while( i < study.getNumberOfSeries() && !found )
    {
        if ( study.getDefaultSeriesUID() == study.getSeriesVolum(i).getSeriesUID() )
        {
            found = true;
            serie = study.getSeriesVolum(i);
        }
        i++;
    }
    if( !found ) //si no l'hem trobat per defecte mostrarem la primera serie
        serie = study.getSeriesVolum(0);

    switch( input->readFiles( serie.getImagesPathList() ) )
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

    bool found = false;
    int i = 0;
    SeriesVolum serie;

    m_mainApp->setCursor( QCursor(Qt::WaitCursor) );
    while( i < study.getNumberOfSeries() && !found )
    {
        if ( study.getDefaultSeriesUID() == study.getSeriesVolum(i).getSeriesUID() )
        {
            found = true;
            serie = study.getSeriesVolum(i);
        }
        i++;
    }
    if( !found ) //si no l'hem trobat per defecte mostrarem la primera serie
        serie = study.getSeriesVolum(0);

    switch( input->readFiles( serie.getImagesPathList() ) )
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

    bool found = false;
    int i = 0;
    SeriesVolum serie;

    m_mainApp->setCursor( QCursor(Qt::WaitCursor) );
    while( i < study.getNumberOfSeries() && !found )
    {
        if ( study.getDefaultSeriesUID() == study.getSeriesVolum(i).getSeriesUID() )
        {
            found = true;
            serie = study.getSeriesVolum(i);
        }
        i++;
    }
    if( !found ) //si no l'hem trobat per defecte mostrarem la primera serie
        serie = study.getSeriesVolum(0);

    input->readFiles( serie.getImagesPathList() );
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

void ExtensionHandler::openPerfusionImage()
{
    disconnect( m_queryScreen, SIGNAL( viewStudy(StudyVolum) ),
                this, SLOT( viewStudy(StudyVolum) ) );
    connect( m_queryScreen, SIGNAL( viewStudy(StudyVolum) ),
             this, SLOT( viewStudyForPerfusion(StudyVolum) ) );

    m_queryScreen->show();
}

void ExtensionHandler::openSerieToCompare()
{
    QueryScreen *queryScreen = new QueryScreen( m_mainApp );
    connect( queryScreen , SIGNAL( viewStudy(StudyVolum) ) , this , SLOT( viewStudyToCompare(StudyVolum) ) );
    queryScreen->show();
}

void ExtensionHandler::registerExtensions()
{
}

void ExtensionHandler::createConnections()
{
    connect( m_queryScreen , SIGNAL(viewStudy(StudyVolum)) , this , SLOT(viewStudy(StudyVolum)) );
    connect( m_mainApp->m_extensionWorkspace , SIGNAL( currentChanged(int) ) , this , SLOT( extensionChanged(int) ) );
    connect( m_queryScreen, SIGNAL(viewPatient(PatientFillerInput)), this, SLOT(viewPatient(PatientFillerInput)));
}

void ExtensionHandler::load2DViewerExtension()
{
    Q2DViewerExtension *defaultViewerExtension = new Q2DViewerExtension;
    defaultViewerExtension->setInput( m_volumeRepository->getVolume( m_volumeID ) );
    m_mainApp->m_extensionWorkspace->addApplication( defaultViewerExtension , tr("2D Viewer"));
    // defaultViewerExtension->populateToolBar( m_mainApp->getExtensionsToolBar() );
    connect( defaultViewerExtension , SIGNAL( newSerie() ) , this , SLOT( openSerieToCompare() ) );
    connect( this , SIGNAL( secondInput(Volume*) ) , defaultViewerExtension , SLOT( setSecondInput(Volume*) ) );
    connect( m_queryScreen, SIGNAL(viewKeyImageNote( const QString& )), defaultViewerExtension, SLOT(loadKeyImageNote( const QString& )));
    connect( m_queryScreen, SIGNAL(viewPresentationState(const QString &)),
             defaultViewerExtension, SLOT(loadPresentationState(const QString &) ));
    connect( m_importFileApp, SIGNAL(openKeyImageNote( const QString& )), defaultViewerExtension, SLOT(loadKeyImageNote( const QString& )));
    connect( m_importFileApp, SIGNAL(openPresentationState( const QString& )),
             defaultViewerExtension, SLOT(loadPresentationState( const QString& )));
}

void ExtensionHandler::extensionChanged( int index )
{
    // quan canvia una extensió hem de canviar les toolbars, per tan hem de mirar com fer-ho perque no sabem quina extensió ( sí podem
    // obtenir el widget ) en concret és la que tenim. Ho podríem fer mitjançant signals i slots. és a dir, quan es faci el canvi
    // d'extensió s'enviarà alguna senyal que farà que netejem la toolbar d¡extensions i que s'ompli amb els nous botons i eines
    m_mainApp->m_extensionWorkspace->setLastIndex( index );
}

};  // end namespace udg
