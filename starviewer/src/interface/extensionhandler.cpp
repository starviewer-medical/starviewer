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
#include "logging.h"
#include "extensionworkspace.h"
#include "qapplicationmainwindow.h"
#include "volumerepository.h" // per esborrar els volums
#include "extensionmediatorfactory.h"
#include "extensionfactory.h"
#include "extensioncontext.h"

// Espai reservat pels include de les mini-apps
#include "appimportfile.h"

// Fi de l'espai reservat pels include de les mini-apps

// PACS --------------------------------------------
#include "queryscreen.h"
#include "patientfiller.h"

namespace udg {

ExtensionHandler::ExtensionHandler( QApplicationMainWindow *mainApp , QObject *parent, QString name)
 : QObject(parent )
{
    this->setObjectName( name );
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
            m_importFileApp->open();
            break;

        case 6:
            m_importFileApp->openDirectory();
            break;

        case 7:
            m_queryScreen->show();
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
        extensionContext.setPatient( m_mainApp->getCurrentPatient() );
        extensionContext.setDefaultSelectedStudies( QStringList(m_defaultStudyUID) );
        extensionContext.setDefaultSelectedSeries( QStringList(m_defaultSeriesUID) );

        mediator->initializeExtension(extension, extensionContext);
        m_mainApp->getExtensionWorkspace()->addApplication(extension, mediator->getExtensionID().getLabel() );
    }
    else
    {
        DEBUG_LOG( "Error carregant " + who );
    }
}

void ExtensionHandler::killBill()
{
    // TODO descarregar tots els volums que tingui el pacient en aquesta finestra
    // quan ens destruim alliberem tots els volums que hi hagi a memòria
    if (m_mainApp->getCurrentPatient() != NULL)
    {
        foreach( Study *study, m_mainApp->getCurrentPatient()->getStudies() )
        {
            foreach( Series *series, study->getSeries() )
            {
                foreach( Identifier id, series->getVolumesIDList()  )
                {
                    VolumeRepository::getRepository()->removeVolume( id );
                }
            }
        }
    }
}

void ExtensionHandler::createConnections()
{
    connect( m_queryScreen, SIGNAL(processFiles(QStringList,QString,QString,QString)), this, SLOT(processInput(QStringList,QString,QString,QString)) );
    connect( m_importFileApp,SIGNAL( selectedFiles(QStringList) ), SLOT(processInput(QStringList) ) );
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
        request("Q2DViewerExtension");
    }
    else
        DEBUG_LOG("No hi ha dades de pacient!");
}

};  // end namespace udg
