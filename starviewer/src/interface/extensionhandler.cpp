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
#include "extensioncontext.h"

// Espai reservat pels include de les mini-apps
#include "appimportfile.h"
#include "q2dviewerextension.h"

// Fi de l'espai reservat pels include de les mini-apps

// PACS --------------------------------------------
#include "queryscreen.h"
#include "seriesvolum.h"
#include "input.h"
#include "patientfiller.h"

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
                {
                    m_mainApp->onVolumeLoaded( m_importFileApp->getVolumeIdentifier() );
                }
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
        ExtensionContext extensionContext;
        extensionContext.setMainVolumeID(m_volumeID);
        extensionContext.setPatient( m_mainApp->getCurrentPatient() );
        extensionContext.setDefaultSelectedStudies( QStringList(m_defaultStudyUID) );
        extensionContext.setDefaultSelectedSeries( QStringList(m_defaultSeriesUID) );

        mediator->initializeExtension(extension, extensionContext, this);
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

void ExtensionHandler::killBill()
{
    if( !m_volumeID.isNull() )
    {
        m_volumeRepository->removeVolume( m_volumeID );
    }
}

void ExtensionHandler::createConnections()
{
    connect( m_mainApp->m_extensionWorkspace , SIGNAL( currentChanged(int) ) , this , SLOT( extensionChanged(int) ) );
    connect( m_queryScreen, SIGNAL(processFiles(QStringList,QString,QString,QString)), this, SLOT(processInput(QStringList,QString,QString,QString)) );
}

void ExtensionHandler::load2DViewerExtension()
{
    ExtensionContext extensionContext;
    extensionContext.setMainVolumeID( m_volumeID );
    extensionContext.setPatient( m_mainApp->getCurrentPatient() );
    extensionContext.setDefaultSelectedStudies( QStringList(m_defaultStudyUID) );
    extensionContext.setDefaultSelectedSeries( QStringList(m_defaultSeriesUID) );

    Q2DViewerExtension *defaultViewerExtension = new Q2DViewerExtension;
    defaultViewerExtension->setInput( extensionContext.getDefaultVolume() );
    m_mainApp->m_extensionWorkspace->addApplication( defaultViewerExtension , tr("2D Viewer"));

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

QProgressDialog* ExtensionHandler::activateProgressDialog( Input *input )
{
    QProgressDialog *progressDialog = new QProgressDialog( m_mainApp );
    progressDialog->setModal( true );
    progressDialog->setRange( 0 , 100 );
    progressDialog->setMinimumDuration( 0 );
    progressDialog->setWindowTitle( tr("Serie loading") );
    // atenció: el missatge triga una miqueta a aparèixer...
    progressDialog->setLabelText( tr("Loading, please wait...") );
    progressDialog->setCancelButton( 0 );
    connect( input , SIGNAL( progress(int) ) , progressDialog , SLOT( setValue(int) ) );

    return progressDialog;
}

void ExtensionHandler::processInput( QStringList inputFiles, QString defaultStudyUID, QString defaultSeriesUID, QString defaultImageInstance )
{
    m_defaultStudyUID = defaultStudyUID;
    m_defaultSeriesUID = defaultSeriesUID;

    PatientFillerInput *fillerInput = new PatientFillerInput;
    fillerInput->setFilesList( inputFiles );

    QProgressDialog progressDialog( m_mainApp );
    progressDialog.setModal( true );
    progressDialog.setRange( 0 , 100 );
    progressDialog.setMinimumDuration( 0 );
    progressDialog.setWindowTitle( tr("Patient loading") );
    progressDialog.setLabelText( tr("Loading, please wait...") );
    progressDialog.setCancelButton( 0 );

    PatientFiller patientFiller;
    connect(&patientFiller, SIGNAL( progress(int) ), &progressDialog, SLOT( setValue(int) ));
    patientFiller.fill( fillerInput );

    unsigned int numberOfPatients = fillerInput->getNumberOfPatients();

    DEBUG_LOG( "Labels: " + fillerInput->getLabels().join("; "));
    DEBUG_LOG( QString("getNumberOfPatients: %1").arg( numberOfPatients ) );

    for( int i = 0; i < numberOfPatients; i++ )
    {
        DEBUG_LOG( QString("Patient #%1\n %2").arg(i).arg( fillerInput->getPatient(i)->toString() ) );
        this->addPatientData( fillerInput->getPatient(i) );
    }
}

void ExtensionHandler::addPatientData( Patient *patient )
{
    // TODO decidir que fem aquí, si mostrar diàlegs, etc
    m_mainApp->addPatient( patient );
}

void ExtensionHandler::openDefaultExtension()
{
    if( m_mainApp->getCurrentPatient() )
    {
        // TODO de moment simplement cridem el load2DViewerExtension
        load2DViewerExtension();
    }
    else
        DEBUG_LOG("No hi ha dades de pacient!");
}

};  // end namespace udg
