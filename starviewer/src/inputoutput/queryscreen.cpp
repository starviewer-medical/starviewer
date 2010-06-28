/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "queryscreen.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QMovie>

#include "qpacslist.h"
#include "inputoutputsettings.h"
#include "pacsdevicemanager.h"
#include "logging.h"
#include "status.h"
#include "qcreatedicomdir.h"
#include "dicommask.h"
#include "qoperationstatescreen.h"
#include "localdatabasemanager.h"
#include "patient.h"
#include "starviewerapplication.h"
#include "utils.h"
#include "statswatcher.h"
#include "pacsdevice.h"
#include "risrequestmanager.h"
#include "pacsmanager.h"
#include "retrievedicomfilesfrompacsjob.h"

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#endif

namespace udg {

QueryScreen::QueryScreen( QWidget *parent )
 : QDialog(parent )
{
    setupUi( this );
    setWindowFlags( Qt::Widget );

    initialize();//inicialitzem les variables necessàries
    //connectem signals i slots
    createConnections();
    //Fa les comprovacions necessaries per poder executar la QueryScreen de forma correcte   
    checkRequeriments();

    readSettings();
    //fem que per defecte mostri els estudis de la cache
    m_qInputOutputLocalDatabaseWidget->queryStudy(DicomMask());

    // Configuració per Starviewer Lite
#ifdef STARVIEWER_LITE
    m_showPACSNodesToolButton->hide();
    m_operationListToolButton->hide();
    m_createDICOMDIRToolButton->hide();
    m_advancedSearchToolButton->hide();
    m_tab->removeTab(1); // tab de "PACS" fora
#else
    /*L'engeguem després d'haver fet els connects, no es pot fer abans, perquè per exemple en el cas que tinguem un error
     *perquè el port ja està en us, si l'engeguem abans es faria signal indicant error de port en ús i no hi hauria hagut 
     *temps d'haver fet el connect del signal d'error, per tant el signal s'hauria perdut sense poder avisar de l'error
     */
    if (Settings().getValue(InputOutputSettings::ListenToRISRequests).toBool()) 
    {
        m_risRequestManager->listen();
    }

#endif

    m_statsWatcher = new StatsWatcher("QueryScreen",this);
    m_statsWatcher->addClicksCounter( m_operationListToolButton );
    m_statsWatcher->addClicksCounter( m_showPACSNodesToolButton );
    m_statsWatcher->addClicksCounter( m_createDICOMDIRToolButton );
    m_statsWatcher->addClicksCounter( m_advancedSearchToolButton );
    m_statsWatcher->addClicksCounter( m_clearToolButton );
    m_statsWatcher->addClicksCounter( m_createDICOMDIRToolButton );
}

QueryScreen::~QueryScreen()
{
    /*sinó fem un this.close i tenim la finestra queryscreen oberta al tancar l'starviewer, l'starviewer no finalitza
     *desapareixen les finestres, però el procés continua viu
     */
    this->close();

#ifndef STARVIEWER_LITE
    delete m_risRequestManager;
#endif
}

void QueryScreen::initialize()
{
#ifndef STARVIEWER_LITE
    m_pacsManager = new PacsManager();
#endif
    //indiquem que la llista de Pacs no es mostra
    m_showPACSNodes = false;
    m_PACSNodes->setVisible(false);
    m_qcreateDicomdir = new udg::QCreateDicomdir( this );
    /* Posem com a pare el pare de la queryscreen, d'aquesta manera quan es tanqui el pare de la queryscreen
     * el QOperationStateScreen també es tancarà
     */
    m_operationStateScreen = new udg::QOperationStateScreen( this );

    m_qInputOutputLocalDatabaseWidget->setPacsManager(m_pacsManager);
    m_qInputOutputPacsWidget->setPacsManager(m_pacsManager);
    m_operationStateScreen->setPacsManager(m_pacsManager);

    //Indiquem quin és la intefície encara de crear dicomdir per a que es puguin comunicar
    m_qInputOutputLocalDatabaseWidget->setQCreateDicomdir(m_qcreateDicomdir);

    QMovie *operationAnimation = new QMovie(this);
    operationAnimation->setFileName(":/images/loader.gif");
    m_operationAnimation->setMovie(operationAnimation);
    operationAnimation->start();

    m_qadvancedSearchWidget->hide();
    m_operationAnimation->hide();
    m_labelOperation->hide();
    refreshTab( LocalDataBaseTab );
    
#ifndef STARVIEWER_LITE
    if (Settings().getValue(InputOutputSettings::ListenToRISRequests).toBool()) 
    {
        m_risRequestManager = new RISRequestManager();
    }
    else
    {
        m_risRequestManager = NULL;
    }

#endif

}

void QueryScreen::createConnections()
{
    //connectem els butons
    connect( m_searchButton, SIGNAL( clicked() ), SLOT( searchStudy() ) );
    connect( m_clearToolButton, SIGNAL( clicked() ), SLOT( clearTexts() ) );
    connect( m_operationListToolButton, SIGNAL( clicked() ) , SLOT( showOperationStateScreen() ) );
    connect( m_showPACSNodesToolButton, SIGNAL( toggled(bool) ), m_PACSNodes, SLOT( setVisible(bool) ) );

    connect( m_createDICOMDIRToolButton, SIGNAL( clicked() ), m_qcreateDicomdir, SLOT( show() ) );

    //es canvia de pestanya del TAB
    connect( m_tab , SIGNAL( currentChanged( int ) ), SLOT( refreshTab( int ) ) );

    //Amaga o ensenya la cerca avançada
    connect( m_advancedSearchToolButton, SIGNAL( toggled( bool ) ), SLOT( setAdvancedSearchVisible( bool ) ) );

    #ifndef STARVIEWER_LITE
    if (m_risRequestManager != NULL)
    {
        connect(m_risRequestManager, SIGNAL(retrieveStudyFromRISRequest(QString, Study*)), SLOT(retrieveStudyFromRISRequest(QString, Study*)));
    }

    connect(m_pacsManager, SIGNAL(newPACSJobEnqueued(PACSJob *)), SLOT(newPACSJobEnqueued(PACSJob*)));
    #endif

    connect(m_qInputOutputDicomdirWidget, SIGNAL(clearSearchTexts()), SLOT(clearTexts()));
    connect(m_qInputOutputDicomdirWidget, SIGNAL(viewPatients(QList<Patient*>)), SLOT(viewPatients(QList<Patient*>)));
    connect(m_qInputOutputDicomdirWidget, SIGNAL(studyRetrieved(QString)), m_qInputOutputLocalDatabaseWidget, SLOT(addStudyToQStudyTreeWidget(QString)));

    connect(m_qInputOutputLocalDatabaseWidget, SIGNAL(viewPatients(QList<Patient*>,bool)), SLOT(viewPatients(QList<Patient*>,bool)));

    connect(m_qInputOutputPacsWidget, SIGNAL(viewRetrievedStudy(QString)), SLOT(viewRetrievedStudyFromPacs(QString)));
    connect(m_qInputOutputPacsWidget, SIGNAL(loadRetrievedStudy(QString)), SLOT(loadRetrievedStudyFromPacs(QString)));

    ///Ens informa quan hi hagut un canvi d'estat en alguna de les operacions
    connect(m_qInputOutputPacsWidget, SIGNAL(studyRetrieveFinished(QString)), m_qInputOutputLocalDatabaseWidget, SLOT(addStudyToQStudyTreeWidget(QString)));

    connect(m_qInputOutputPacsWidget, SIGNAL(studyRetrieveFinished(QString)), SLOT(studyRetrieveFinishedSlot(QString)));
    connect(m_qInputOutputPacsWidget, SIGNAL(studyRetrieveFailed(QString)), SLOT(studyRetrieveFailedSlot(QString)));
    connect(m_qInputOutputPacsWidget, SIGNAL(studyRetrieveStarted(QString)), SLOT(studyRetrieveStartedSlot(QString)));
}

void QueryScreen::checkRequeriments()
{
    //Comprova que la base de dades d'imatges estigui consistent, comprovant que no haguessin quedat estudis a mig descarregar l'última vegada que es va tancar l'starviewer, i si és així esborra les imatges i deixa la base de dades en un estat consistent
    checkDatabaseImageIntegrity();
    //Comprova que el port pel qual es reben les descàrregues d'objectes dicom del PACS no estigui ja en ús
    checkIncomingConnectionsPacsPortNotInUse();
}

void QueryScreen::checkIncomingConnectionsPacsPortNotInUse()
{
    Settings settings;
    int localPort = PacsDevice::getIncomingDICOMConnectionsPort();

    if ( Utils::isPortInUse( localPort ) )
    {
        QString message = tr("Port %1 for incoming connections from PACS is already in use by another application.").arg(localPort);
        message += tr("\n\n%1 couldn't retrieve studies from PACS if the port is in use, please close the application that is using port %2 or change Starviewer port for incoming connections from PACS in the configuration screen.").arg(ApplicationNameString).arg(localPort);

        QMessageBox::warning(this, ApplicationNameString, message);
    }
}

void QueryScreen::checkDatabaseImageIntegrity()
{
    LocalDatabaseManager localDatabaseManager;

    localDatabaseManager.checkNoStudiesRetrieving();

    if (localDatabaseManager.getLastError() != LocalDatabaseManager::Ok)
    {
        ERROR_LOG("S'ha produït un error esborrant un estudi que no s'havia acabat de descarregar en la última execució");
    }
}

void QueryScreen::setAdvancedSearchVisible(bool visible)
{
    m_qadvancedSearchWidget->setVisible(visible);

    if (visible)
    {
        m_advancedSearchToolButton->setText( m_advancedSearchToolButton->text().replace(">>","<<") );
    }
    else
    {
        m_qadvancedSearchWidget->clear();
        m_advancedSearchToolButton->setText( m_advancedSearchToolButton->text().replace("<<",">>") );
    }
}

void QueryScreen::clearTexts()
{
    m_qbasicSearchWidget->clear();
    m_qadvancedSearchWidget->clear();
}

void QueryScreen::updateConfiguration(const QString &configuration)
{
    if (configuration == "Pacs/ListChanged")
    {
        m_PACSNodes->refresh();
    }
    else if (configuration == "Pacs/CacheCleared")
    {
        m_qInputOutputLocalDatabaseWidget->clear();
    }
}

void QueryScreen::bringToFront()
{
    if( this->isMinimized() )
    {
        this->setWindowState( this->windowState() & ~Qt::WindowMinimized | Qt::WindowActive );
    }
    else
    {
        this->raise();
        this->activateWindow();
    }
    this->show();
}

void QueryScreen::showPACSTab()
{
    m_tab->setCurrentIndex( PACSQueryTab );
    bringToFront();
}

void QueryScreen::showLocalExams()
{
    m_tab->setCurrentIndex( LocalDataBaseTab );
    m_qbasicSearchWidget->clear();
    m_qbasicSearchWidget->setDefaultDate( QBasicSearchWidget::AnyDate );
    m_qadvancedSearchWidget->clear();
    m_qInputOutputLocalDatabaseWidget->queryStudy(DicomMask());
    bringToFront();
}

void QueryScreen::searchStudy()
{
    switch ( m_tab->currentIndex() )
    {
        case LocalDataBaseTab:
            m_qInputOutputLocalDatabaseWidget->queryStudy(buildDicomMask());
            break;

        case PACSQueryTab:
            m_qInputOutputPacsWidget->queryStudy(buildDicomMask(), m_PACSNodes->getSelectedPacs());
        break;

        case DICOMDIRTab:
            m_qInputOutputDicomdirWidget->queryStudy(buildDicomMask());
            break;
    }
}

void QueryScreen::viewRetrievedStudyFromPacs(QString studyInstanceUID)
{
    QStringList studyUIDList;
    studyUIDList << studyInstanceUID;

    //Indiquem que volem veure un estudi que està guardat a la base de dades
    m_qInputOutputLocalDatabaseWidget->view(studyUIDList, "");
}

void QueryScreen::loadRetrievedStudyFromPacs(QString studyInstanceUID)
{
    QStringList studyUIDList;
    studyUIDList << studyInstanceUID;

    //Indiquem que volem veure un estudi que està guardat a la base de dades
    m_qInputOutputLocalDatabaseWidget->view(studyUIDList, "", true);
}

void QueryScreen::sendDicomObjectsToPacs(PacsDevice pacsDevice, QList<Image*> images)
{
    m_qInputOutputLocalDatabaseWidget->sendDICOMFilesToPACS(pacsDevice, images);
}

void QueryScreen::refreshTab( int index )
{
    switch ( index )
    {
        case LocalDataBaseTab:
                m_qbasicSearchWidget->setEnabledSeriesModality(false);
                m_qadvancedSearchWidget->setEnabled(false);
                break;

        case PACSQueryTab:
                m_qbasicSearchWidget->setEnabledSeriesModality(true);
                m_qadvancedSearchWidget->setEnabled( true );
                break;

        case DICOMDIRTab:
                m_qbasicSearchWidget->setEnabledSeriesModality(false);
                m_qadvancedSearchWidget->setEnabled(false);
                break;
    }
}

void QueryScreen::viewPatients(QList<Patient*> listPatientsToView, bool loadOnly )
{
    // Si fem un "view" amagarem les finestres de la QueryScreen perquè
    // l'aplicació i les extensions siguin visibles, altrament no amagarem res
    if( !loadOnly )
    {
        this->close();//s'amaga per poder visualitzar la serie

        if (m_operationStateScreen->isVisible())
        {
            m_operationStateScreen->close();//s'amaga per poder visualitzar la serie
        }
    }

    emit selectedPatients(listPatientsToView,loadOnly);
}

void QueryScreen::showOperationStateScreen()
{
    if ( !m_operationStateScreen->isVisible() )
    {
        m_operationStateScreen->setVisible( true );
    }
    else
    {
        m_operationStateScreen->raise();
        m_operationStateScreen->activateWindow();
    }
}

void QueryScreen::openDicomdir()
{
    if( m_qInputOutputDicomdirWidget->openDicomdir() )
    {
        this->bringToFront();
        m_tab->setCurrentIndex( DICOMDIRTab ); // mostre el tab del dicomdir
    }
}

DicomMask QueryScreen::buildDicomMask()
{
    return m_qbasicSearchWidget->buildDicomMask() + m_qadvancedSearchWidget->buildDicomMask();
}

void QueryScreen::closeEvent( QCloseEvent* event )
{
    writeSettings(); // guardem els settings

    m_operationStateScreen->close(); //Tanquem la QOperationStateScreen al tancar la QueryScreen

    event->accept();
}

void QueryScreen::readSettings()
{
    Settings settings;
    settings.restoreGeometry(InputOutputSettings::QueryScreenGeometry,this);
    // Aquesta clau substitueix les obsoletes "queryScreenWindowPositionX", "queryScreenWindowPositionY", "queryScreenWindowWidth" i "queryScreenWindowHeight"
    // que tenien les claus /interface/queryscreen/ + windowPositionX, windowPositionY, windowWidth i windowHeigth respectivament
    // TODO fer neteja d'aquestes claus antigues amb la migració de dades
}

void QueryScreen::writeSettings()
{
    /* Només guardem els settings quan la interfície ha estat visible, ja que hi ha settings com el QSplitter que si la primera vegada
     * que executem l'starviewer no obrim la QueryScreen retorna un valors incorrecte per això, el que fem és comprova que la QueryScreen
     * hagi estat visible per guardar el settings
     */
    if (this->isVisible())
    {
        Settings settings;
        settings.saveGeometry(InputOutputSettings::QueryScreenGeometry, this);
    }
}

void QueryScreen::retrieveStudyFromRISRequest(QString pacsID, Study *study)
{
    DicomMask maskStudyToRetrieve;

    maskStudyToRetrieve.setStudyInstanceUID(study->getInstanceUID());
    QInputOutputPacsWidget::ActionsAfterRetrieve actionAfterRetrieve;
    if( Settings().getValue( InputOutputSettings::RisRequestViewOnceRetrieved ).toBool() )
    {
        actionAfterRetrieve = QInputOutputPacsWidget::View;
    }
    else
    {
        actionAfterRetrieve = QInputOutputPacsWidget::None;
    }

    m_qInputOutputPacsWidget->retrieve(pacsID, study, maskStudyToRetrieve, QInputOutputPacsWidget::View);
}

void QueryScreen::retrieveStudy(QInputOutputPacsWidget::ActionsAfterRetrieve actionAfterRetrieve, QString pacsID, Study *study)
{
    DicomMask maskStudyToRetrieve;

    maskStudyToRetrieve.setStudyInstanceUID(study->getInstanceUID());

    /*QueryScreen rep un signal cada vegada que qualsevol estudis en el procés de descàrrega canvia d'estat, en principi només ha de reemetre aquests signals
      cap a fora quan és un signal que afecta un estudi sol·licitat a través d'aquest mètode públic, per això mantenim aquesta llista que ens indica els estudis 
      pendents de descarregar sol·licitats a partir d'aquest mètode */

    m_studyRequestedToRetrieveFromPublicMethod.append(study->getInstanceUID());

    m_qInputOutputPacsWidget->retrieve(pacsID, study, maskStudyToRetrieve, actionAfterRetrieve);
}

void QueryScreen::studyRetrieveFailedSlot(QString studyInstanceUID)
{
    if (m_studyRequestedToRetrieveFromPublicMethod.contains(studyInstanceUID))
    {
        //és un estudi dels que ens han demanat des del mètode públic

        m_studyRequestedToRetrieveFromPublicMethod.removeOne(studyInstanceUID);

        emit studyRetrieveFailed(studyInstanceUID);
    }
}
	
void QueryScreen::studyRetrieveFinishedSlot(QString studyInstanceUID)
{
    if (m_studyRequestedToRetrieveFromPublicMethod.contains(studyInstanceUID))
    {
        //és un estudi dels que ens han demanat des del mètode públic

        m_studyRequestedToRetrieveFromPublicMethod.removeOne(studyInstanceUID);

        emit studyRetrieveFinished(studyInstanceUID);
    }
}

void QueryScreen::studyRetrieveStartedSlot(QString studyInstanceUID)
{
    if (m_studyRequestedToRetrieveFromPublicMethod.contains(studyInstanceUID))
    {
        //és un estudi dels que ens han demanat des del mètode públic

        emit studyRetrieveStarted(studyInstanceUID);
    }
}

void QueryScreen::newPACSJobEnqueued(PACSJob *pacsJob)
{
    if (pacsJob->getPACSJobType() == PACSJob::SendDICOMFilesToPACSJobType || pacsJob->getPACSJobType() == PACSJob::RetrieveDICOMFilesFromPACSJobType)
    {
        m_operationAnimation->show();
        m_labelOperation->show();
        connect(pacsJob, SIGNAL(PACSJobFinished(PACSJob*)), SLOT(pacsJobFinished(PACSJob*)));
    }
}

void QueryScreen::pacsJobFinished(PACSJob *)
{
    if (!m_pacsManager->isExecutingPACSJob(PACSJob::SendDICOMFilesToPACSJobType) && 
        !m_pacsManager->isExecutingPACSJob(PACSJob::RetrieveDICOMFilesFromPACSJobType))
    {
        m_operationAnimation->hide();
        m_labelOperation->hide();
    }
}

};

