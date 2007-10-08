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
#include "patientfillerinput.h"

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

        case 8:
            m_queryScreen->openDicomdir();
            break;
    }
}

void ExtensionHandler::request( const QString &who )
{
    QWidget *extension = ExtensionFactory::instance()->create(who);
    ExtensionMediator* mediator = ExtensionMediatorFactory::instance()->create(who);

    if (mediator && extension)
    {
        mediator->initializeExtension(extension, m_extensionContext );
        m_mainApp->getExtensionWorkspace()->addApplication(extension, mediator->getExtensionID().getLabel() );
    }
    else
    {
        DEBUG_LOG( "Error carregant " + who );
    }
}

void ExtensionHandler::killBill()
{
    // eliminem totes les extensions
    int numberOfExtensions = m_mainApp->getExtensionWorkspace()->count();
    for( int i = 0; i < numberOfExtensions; i++ )
    {
        QWidget *currentExtension = m_mainApp->getExtensionWorkspace()->widget(i);
        delete currentExtension;
        m_mainApp->getExtensionWorkspace()->removeTab(i);
    }
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

void ExtensionHandler::setContext( const ExtensionContext &context )
{
    m_extensionContext = context;
}

ExtensionContext &ExtensionHandler::getContext()
{
    return m_extensionContext;
}

void ExtensionHandler::updateConfiguration(const QString &configuration)
{
    m_queryScreen->updateConfiguration(configuration);
}

void ExtensionHandler::createConnections()
{
    connect( m_queryScreen, SIGNAL(processFiles(QStringList,QString,QString,QString)), this, SLOT(processInput(QStringList,QString,QString,QString)) );
    connect( m_importFileApp,SIGNAL( selectedFiles(QStringList) ), SLOT(processInput(QStringList) ) );
}

void ExtensionHandler::processInput( QStringList inputFiles, QString defaultStudyUID, QString defaultSeriesUID, QString defaultImageInstance )
{
    PatientFillerInput *fillerInput = new PatientFillerInput;
    fillerInput->setFilesList( inputFiles );

    QProgressDialog progressDialog( m_mainApp );
    progressDialog.setModal( true );
    progressDialog.setRange( 0 , 100 );
    progressDialog.setMinimumDuration( 0 );
    progressDialog.setWindowTitle( tr("Patient loading") );
    progressDialog.setLabelText( tr("Loading, please wait...") );
    progressDialog.setCancelButton( 0 );
    progressDialog.setValue(0);
    qApp->processEvents();

    PatientFiller patientFiller;
    connect(&patientFiller, SIGNAL( progress(int) ), &progressDialog, SLOT( setValue(int) ));
    patientFiller.fill( fillerInput );

    unsigned int numberOfPatients = fillerInput->getNumberOfPatients();

    DEBUG_LOG( "Labels: " + fillerInput->getLabels().join("; "));
    DEBUG_LOG( QString("getNumberOfPatients: %1").arg( numberOfPatients ) );

    QList<int> correctlyLoadedPatients;

    for(unsigned int i = 0; i < numberOfPatients; i++ )
    {
        DEBUG_LOG( QString("Patient #%1\n %2").arg(i).arg( fillerInput->getPatient(i)->toString() ) );

        ExtensionContext extensionContext;
        extensionContext.setPatient( fillerInput->getPatient(i) );

        // marquem les series seleccionades
        Study *study = NULL;
        study = fillerInput->getPatient(i)->getStudy( defaultStudyUID );
        // Si no ens diuen un study seleccionat en seleccionem un nosaltres. Això pot ser perquè l'uid d'estudi sigui d'un altre pacient o perquè l'uid està buit
        if(!study)
        {
            QList<Study *> studyList = fillerInput->getPatient(i)->getStudies();
            if ( !studyList.isEmpty() )
            {
                study = studyList.first();
            }
        }

        bool error = true;
        if( study )
        {
            Series *series = NULL;
            series = fillerInput->getPatient(i)->getSeries( defaultSeriesUID );
            // si aquest ens "falla" és perquè possiblement l'UID és d'un altre estudi
            // o perquè l'uid és buit, per tant no tenim cap predilecció i escollim el primer
            if( !series ) // No tenim cap serie seleccionada, seleccionem per defecte la primera
            {
                QList<Series *> seriesList = study->getSeries();
                if( !seriesList.isEmpty() )
                {
                    series = seriesList.first();
                }
            }

            if (series)
            {
                series->select();
                error = false;
                correctlyLoadedPatients << i;
            }
            else
            {
                ERROR_LOG(fillerInput->getPatient(i)->toString());
                ERROR_LOG("Error carregant aquest pacient. La serie retornada és null. defaultSeriesUID: " + defaultSeriesUID);
            }
        }
        else
        {
            ERROR_LOG(fillerInput->getPatient(i)->toString());
            ERROR_LOG("Error carregant aquest pacient. L'study retornat és null. defaultStudyUID: " + defaultStudyUID);
        }

        if (!error)
        {
            m_mainApp->addPatientContext( extensionContext, false );
        }
    }

    QString patientsWithError;
    if (correctlyLoadedPatients.count() != numberOfPatients)
    {
        for (unsigned int i = 0; i < numberOfPatients; i++ )
        {
            if (!correctlyLoadedPatients.contains(i))
            {
                patientsWithError += "- " + fillerInput->getPatient(i)->getFullName() + "; ID: " + fillerInput->getPatient(i)->getID() + "<br>";
            }
        }
        QMessageBox::critical(0, tr("Starviewer"), tr("Sorry, an error ocurred while loading the data of patients:<br> %1").arg(patientsWithError) );
    }
}

void ExtensionHandler::openDefaultExtension()
{
    if( m_mainApp->getCurrentPatient() )
    {
        // TODO de moment simplement cridem el Q2DViewerExtension
        request("Q2DViewerExtension");
    }
    else
        DEBUG_LOG("No hi ha dades de pacient!");
}

};  // end namespace udg

