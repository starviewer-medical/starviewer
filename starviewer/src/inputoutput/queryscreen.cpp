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
#include "starviewersettings.h"
#include "operation.h"
#include "pacsmanager.h"
#include "logging.h"
#include "status.h"
#include "qcreatedicomdir.h"
#include "dicommask.h"
#include "qoperationstatescreen.h"
#include "localdatabasemanager.h"
#include "patient.h"
#include "starviewerapplication.h"
#include "parsexmlrispierrequest.h"
#include "utils.h"
#include "statswatcher.h"
#include "multiplequerystudy.h"
#include "pacsparameters.h"

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
    if (StarviewerSettings().getListenRisRequests()) 
        m_listenRISRequestThread->listen(); 
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
    delete m_listenRISRequestThread;
#endif
}

void QueryScreen::initialize()
{
    StarviewerSettings settings;

    //indiquem que la llista de Pacs no es mostra
    m_showPACSNodes = false;
    m_PACSNodes->setVisible(false);

    /* Posem com a pare el pare de la queryscreen, d'aquesta manera quan es tanqui el pare de la queryscreen
     * el QOperationStateScreen també es tancarà
     */
    m_operationStateScreen = new udg::QOperationStateScreen( this );

    m_qInputOutputPacsWidget->setQOperationStateScreen(m_operationStateScreen);

    m_qcreateDicomdir = new udg::QCreateDicomdir( this );

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
    m_listenRISRequestThread = new ListenRISRequestThread(this);
    if (settings.getListenRisRequests()) m_qpopUpRisRequestsScreen = new QPopUpRisRequestsScreen();
    #endif
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
    StarviewerSettings settings;

    if (Utils::isPortInUse(settings.getLocalPort().toInt()))
    {
        QString message = tr("Port %1 for incoming connections from PACS is already in use by another application.").arg(settings.getLocalPort());
        message += tr("\n\n%1 couldn't retrieve studies from PACS if the port is in use, please close the application that is using port %2 or change Starviewer port for incoming connections from PACS in the configuration screen.").arg(ApplicationNameString, settings.getLocalPort());
        message += tr("\n\nIf the error has ocurred when openned new %1's windows, close this window. To open new %1 window you have to choose the 'New' option from the File menu.").arg(ApplicationNameString);

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

void QueryScreen::updateOperationsInProgressMessage()
{
    if (m_operationStateScreen->getActiveOperationsCount() > 0)
    {
        m_operationAnimation->show();
        m_labelOperation->show();
    }
    else
    {
        m_operationAnimation->hide();
        m_labelOperation->hide();
    }
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
    connect(m_listenRISRequestThread, SIGNAL(requestRetrieveStudy(DicomMask)), SLOT(retrieveStudyFromRISRequest(DicomMask)));
    connect(m_listenRISRequestThread, SIGNAL(errorListening(ListenRISRequestThread::ListenRISRequestThreadError)), SLOT(showListenRISRequestThreadError(ListenRISRequestThread::ListenRISRequestThreadError)));
    #endif

    connect(m_qInputOutputDicomdirWidget, SIGNAL(clearSearchTexts()), SLOT(clearTexts()));
    connect(m_qInputOutputDicomdirWidget, SIGNAL(viewPatients(QList<Patient*>)), SLOT(viewPatients(QList<Patient*>)));
    connect(m_qInputOutputDicomdirWidget, SIGNAL(studyRetrieved(QString)), m_qInputOutputLocalDatabaseWidget, SLOT(addStudyToQStudyTreeWidget(QString)));

    connect(m_qInputOutputLocalDatabaseWidget, SIGNAL(viewPatients(QList<Patient*>)), SLOT(viewPatients(QList<Patient*>)));

    connect(m_qInputOutputPacsWidget, SIGNAL(viewRetrievedStudy(QString)), SLOT(viewRetrievedStudyFromPacs(QString)));
    ///Ens informa quan hi hagut un canvi d'estat en alguna de les operacions
    connect(m_qInputOutputPacsWidget, SIGNAL(operationStateChange()), SLOT(updateOperationsInProgressMessage()));
    connect(m_qInputOutputPacsWidget, SIGNAL(studyRetrieved(QString)), m_qInputOutputLocalDatabaseWidget, SLOT(addStudyToQStudyTreeWidget(QString)));
    connect(m_qInputOutputPacsWidget, SIGNAL(studyWillBeDeletedFromDatabase(QString)), m_qInputOutputLocalDatabaseWidget , SLOT(removeStudyFromQStudyTreeWidget(QString)));
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

void QueryScreen::readSettings()
{
    StarviewerSettings settings;
    this->restoreGeometry( settings.getQueryScreenGeometry() );
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
    this->show();
    this->raise();
    this->activateWindow();
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

Status QueryScreen::queryMultiplePacs(DicomMask searchMask, QList<PacsParameters> listPacsToQuery, MultipleQueryStudy *multipleQueryStudy)
{
    QList<PacsParameters> filledPacsParameters;
    StarviewerSettings settings;

    //TODO PacsParameters no hauria de contenir el AETitle i el timeout
    //Hem d'afegir a les dades de pacs parameters el nostre aetitle i timeout
    foreach(PacsParameters pacs, listPacsToQuery)
    {
        pacs.setAELocal(settings.getAETitleMachine());
        pacs.setTimeOut(settings.getTimeout().toInt());
        filledPacsParameters.append(pacs);
    }

    multipleQueryStudy->setMask( searchMask ); //assignem la mascara
    multipleQueryStudy->setPacsList(filledPacsParameters);
    return multipleQueryStudy->StartQueries();
}

void QueryScreen::viewRetrievedStudyFromPacs(QString studyInstanceUID)
{
    QStringList studyUIDList;
    studyUIDList << studyInstanceUID;

    //Indiquem que volem veure un estudi que està guardat a la base de dades
    m_qInputOutputLocalDatabaseWidget->view(studyUIDList, "");
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

void QueryScreen::viewPatients(QList<Patient*> listPatientsToView)
{
    this->close();//s'amaga per poder visualitzar la serie

    if (m_operationStateScreen->isVisible())
    {
        m_operationStateScreen->close();//s'amaga per poder visualitzar la serie
    }

    emit selectedPatients(listPatientsToView);
}

void QueryScreen::closeEvent( QCloseEvent* event )
{
    writeSettings(); // guardem els settings

    m_operationStateScreen->close(); //Tanquem la QOperationStateScreen al tancar la QueryScreen

    event->accept();
    m_qcreateDicomdir->clearTemporaryDir();
}

void QueryScreen::writeSettings()
{
    /* Només guardem els settings quan la interfície ha estat visible, ja que hi ha settings com el QSplitter que si la primera vegada
     * que executem l'starviewer no obrim la QueryScreen retorna un valors incorrecte per això, el que fem és comprova que la QueryScreen
     * hagi estat visible per guardar el settings
     */
    if (this->isVisible())
    {
        StarviewerSettings settings;

        settings.saveQueryScreenGeometry( this->saveGeometry() );
    }
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
    m_qInputOutputDicomdirWidget->openDicomdir();

    this->bringToFront();
    m_tab->setCurrentIndex( DICOMDIRTab ); // mostre el tab del dicomdir
}

void QueryScreen::errorConnectingPacs( QString IDPacs )
{
    PacsManager pacsManager;
    PacsParameters errorPacs;
    QString errorMessage;

    errorPacs = pacsManager.queryPacs(IDPacs);

    errorMessage = tr( "Can't connect to PACS %1 from %2.\nBe sure that the IP and AETitle of the PACS are correct." )
        .arg( errorPacs.getAEPacs() )
        .arg( errorPacs.getInstitution()
    );

    QMessageBox::critical( this , ApplicationNameString , errorMessage );
}

void QueryScreen::errorQueringStudiesPacs(QString PacsID)
{
    PacsManager pacsManager;
    PacsParameters errorPacs;
    QString errorMessage;

    errorPacs = pacsManager.queryPacs(PacsID);
    errorMessage = tr( "Can't query PACS %1 from %2\nBe sure that the IP and AETitle of this PACS are correct" )
        .arg( errorPacs.getAEPacs() )
        .arg( errorPacs.getInstitution()
    );

    QMessageBox::critical( this , ApplicationNameString , errorMessage );
}

DicomMask QueryScreen::buildDicomMask()
{
    return m_qbasicSearchWidget->buildDicomMask() + m_qadvancedSearchWidget->buildDicomMask();
}

void QueryScreen::retrieveStudyFromRISRequest(DicomMask maskRisRequest)
{
    MultipleQueryStudy multipleQueryStudy;
    DicomMask maskStudyToRetrieve;
    StarviewerSettings settings;

    m_qpopUpRisRequestsScreen->setAccessionNumber(maskRisRequest.getAccessionNumber()); //Mostrem el popUP amb l'accession number

    m_qpopUpRisRequestsScreen->show();

    connect ( &multipleQueryStudy, SIGNAL( errorConnectingPacs( QString ) ), SLOT( errorConnectingPacs( QString ) ) );
    connect ( &multipleQueryStudy, SIGNAL( errorQueringStudiesPacs( QString ) ), SLOT( errorQueringStudiesPacs( QString ) ) );

    Status state = queryMultiplePacs(maskRisRequest, PacsManager().queryDefaultPacs(), &multipleQueryStudy);

    //Fem els connects per tracta els possibles errors que es poden donar

    if (!state.good())
    {
        QMessageBox::critical(this , ApplicationNameString , tr("An error ocurred querying default PACS, can't process the RIS request."));
        return;
    }

    if (multipleQueryStudy.getPatientStudyList().isEmpty())
    {
        QString message = tr("%2 can't execute the RIS request, because hasn't found the Study with accession number %1 in the default PACS.").arg(maskRisRequest.getAccessionNumber(), ApplicationNameString);
        QMessageBox::information(this , ApplicationNameString , message);
        return;
    }

    foreach (Patient *patient, multipleQueryStudy.getPatientStudyList())
    {
        foreach(Study *study, patient->getStudies())
        {
            QString pacsID = multipleQueryStudy.getHashTablePacsIDOfStudyInstanceUID()[study->getInstanceUID()];

            maskStudyToRetrieve.setStudyUID(study->getInstanceUID());
            m_qInputOutputPacsWidget->retrieve(settings.getViewAutomaticallyAStudyRetrievedFromRisRequest(), pacsID, maskStudyToRetrieve, study);
        }
    }
}

void QueryScreen::showListenRISRequestThreadError(ListenRISRequestThread::ListenRISRequestThreadError error)
{
    QString message;
    StarviewerSettings settings;

    switch(error)
    {
        case ListenRISRequestThread::risPortInUse :
            message = tr("Can't listen RIS requests on port %1, the port is in use by another application.").arg(settings.getListenPortRisRequests());
            message += tr("\n\nIf the error has ocurred when openned new %1's windows, close this window. To open new %1 window you have to choose the 'New' option from the File menu.").arg(ApplicationNameString);
            break;
        case ListenRISRequestThread::unknowNetworkError :
            message = tr("Can't listen RIS requests on port %1, an unknown network error has produced.").arg(settings.getListenPortRisRequests());
            message += tr("\nIf the problem persist contact with an administrator.");
            break;
    }
    
    QMessageBox::critical(this, ApplicationNameString, message);
}

};

