/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "queryscreen.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QMovie>

#include "qpacslist.h"
#include "inputoutputsettings.h"
#include "logging.h"
#include "qcreatedicomdir.h"
#include "dicommask.h"
#include "qoperationstatescreen.h"
#include "localdatabasemanager.h"
#include "patient.h"
#include "starviewerapplication.h"
#include "statswatcher.h"
#include "pacsdevice.h"
#include "risrequestmanager.h"
#include "pacsmanager.h"
#include "retrievedicomfilesfrompacsjob.h"
#include "portinuse.h"

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#endif

namespace udg {

QueryScreen::QueryScreen(QWidget *parent)
 : QDialog(parent)
{
    setupUi(this);
    setWindowFlags(Qt::Widget);

    // Inicialitzem les variables necessàries
    initialize();
    // Connectem signals i slots
    createConnections();
    // Fa les comprovacions necessaries per poder executar la QueryScreen de forma correcte
    checkRequirements();

    readSettings();
    // Fem que per defecte mostri els estudis de la cache
    m_qInputOutputLocalDatabaseWidget->queryStudy(DicomMask());

    // Configuració per Starviewer Lite
#ifdef STARVIEWER_LITE
    m_showPACSNodesPushButton->hide();
    m_operationListPushButton->hide();
    m_createDICOMDIRPushButton->hide();
    m_advancedSearchPushButton->hide();
    // Tab de "PACS" fora
    m_tab->removeTab(1);
#else
    // L'engeguem després d'haver fet els connects, no es pot fer abans, perquè per exemple en el cas que tinguem un error
    // perquè el port ja està en us, si l'engeguem abans es faria signal indicant error de port en ús i no hi hauria hagut
    // temps d'haver fet el connect del signal d'error, per tant el signal s'hauria perdut sense poder avisar de l'error
    if (Settings().getValue(InputOutputSettings::ListenToRISRequests).toBool())
    {
        m_risRequestManager->listen();
    }
#endif

    m_statsWatcher = new StatsWatcher("QueryScreen", this);
    m_statsWatcher->addClicksCounter(m_operationListPushButton);
    m_statsWatcher->addClicksCounter(m_showPACSNodesPushButton);
    m_statsWatcher->addClicksCounter(m_createDICOMDIRPushButton);
    m_statsWatcher->addClicksCounter(m_advancedSearchPushButton);
    m_statsWatcher->addClicksCounter(m_clearPushButton);
    m_statsWatcher->addClicksCounter(m_createDICOMDIRPushButton);
}

QueryScreen::~QueryScreen()
{
#ifndef STARVIEWER_LITE

    Settings settings;
    settings.setValue(InputOutputSettings::QueryScreenPACSListIsVisible, m_showPACSNodesPushButton->isChecked());

    if (PacsManagerSingleton::instance()->isExecutingPACSJob())
    {
        // Si hi ha PacsJob executant-se demanem cancel·lar
        PacsManagerSingleton::instance()->requestCancelAllPACSJobs();
    }

    delete m_risRequestManager;
#endif

    // Sinó fem un this.close i tenim la finestra queryscreen oberta al tancar l'starviewer, l'starviewer no finalitza
    // desapareixen les finestres, però el procés continua viu
    this->close();
}

RISRequestManager* QueryScreen::getRISRequestManager() const
{
    return m_risRequestManager;
}

void QueryScreen::initialize()
{
    m_qcreateDicomdir = new udg::QCreateDicomdir(this);
#ifndef STARVIEWER_LITE
    // Posem com a pare el pare de la queryscreen, d'aquesta manera quan es tanqui el pare de la queryscreen
    // el QOperationStateScreen també es tancarà
    m_operationStateScreen = new udg::QOperationStateScreen(this);

    if (Settings().getValue(InputOutputSettings::ListenToRISRequests).toBool())
    {
        m_risRequestManager = new RISRequestManager();
    }
    else
    {
        m_risRequestManager = NULL;
    }
#endif
    // Indiquem quin és la intefície encara de crear dicomdir per a que es puguin comunicar
    m_qInputOutputLocalDatabaseWidget->setQCreateDicomdir(m_qcreateDicomdir);

    QMovie *operationAnimation = new QMovie(this);
    operationAnimation->setFileName(":/images/animations/loader.gif");
    m_operationAnimation->setMovie(operationAnimation);
    operationAnimation->start();

    m_qadvancedSearchWidget->hide();
    m_operationAnimation->hide();
    m_labelOperation->hide();
    refreshTab(LocalDataBaseTab);

    m_PACSJobsPendingToFinish = 0;
}

void QueryScreen::createConnections()
{
    // Connectem els butons
    connect(m_searchButton, SIGNAL(clicked()), SLOT(searchStudy()));
    connect(m_clearPushButton, SIGNAL(clicked()), SLOT(clearTexts()));
#ifndef STARVIEWER_LITE
    connect(m_operationListPushButton, SIGNAL(clicked()), SLOT(showOperationStateScreen()));
    connect(m_showPACSNodesPushButton, SIGNAL(toggled(bool)), SLOT(updatePACSNodesVisibility()));
    connect(PacsManagerSingleton::instance(), &PacsManager::newPACSJobEnqueued, this, &QueryScreen::newPACSJobEnqueued);
    if (m_risRequestManager != NULL)
    {
        // Potser que no tinguem activat escoltar peticions del RIS
        connect(m_risRequestManager, SIGNAL(viewStudyRetrievedFromRISRequest(QString)), SLOT(viewStudyFromDatabase(QString)));
        connect(m_risRequestManager, SIGNAL(loadStudyRetrievedFromRISRequest(QString)), SLOT(loadStudyFromDatabase(QString)));
    }
#endif
    connect(m_createDICOMDIRPushButton, SIGNAL(clicked()), m_qcreateDicomdir, SLOT(show()));

    // Es canvia de pestanya del TAB
    connect(m_tab, SIGNAL(currentChanged(int)), SLOT(refreshTab(int)));

    // Amaga o ensenya la cerca avançada
    connect(m_advancedSearchPushButton, SIGNAL(toggled(bool)), SLOT(setAdvancedSearchVisible(bool)));

    connect(m_qInputOutputDicomdirWidget, SIGNAL(clearSearchTexts()), SLOT(clearTexts()));
    connect(m_qInputOutputDicomdirWidget, SIGNAL(viewPatients(QList<Patient*>)), SLOT(viewPatients(QList<Patient*>)));
    connect(m_qInputOutputDicomdirWidget, SIGNAL(studyRetrieved(QString)), m_qInputOutputLocalDatabaseWidget, SLOT(addStudyToQStudyTreeWidget(QString)));

    connect(m_qInputOutputLocalDatabaseWidget, SIGNAL(viewPatients(QList<Patient*>, bool)), SLOT(viewPatients(QList<Patient*>, bool)));

    connect(m_qInputOutputPacsWidget, SIGNAL(viewRetrievedStudy(QString)), SLOT(viewStudyFromDatabase(QString)));
    connect(m_qInputOutputPacsWidget, SIGNAL(loadRetrievedStudy(QString)), SLOT(loadStudyFromDatabase(QString)));

    /// Ens informa quan hi hagut un canvi d'estat en alguna de les operacions
    connect(m_qInputOutputPacsWidget, SIGNAL(studyRetrieveFinished(QString)), m_qInputOutputLocalDatabaseWidget, SLOT(addStudyToQStudyTreeWidget(QString)));

    connect(m_qInputOutputPacsWidget, SIGNAL(studyRetrieveFinished(QString)), SLOT(studyRetrieveFinishedSlot(QString)));
    connect(m_qInputOutputPacsWidget, SIGNAL(studyRetrieveFailed(QString)), SLOT(studyRetrieveFailedSlot(QString)));
    connect(m_qInputOutputPacsWidget, SIGNAL(studyRetrieveStarted(QString)), SLOT(studyRetrieveStartedSlot(QString)));
    connect(m_qInputOutputPacsWidget, SIGNAL(studyRetrieveCancelled(QString)), SLOT(studyRetrieveCancelledSlot(QString)));
}

void QueryScreen::checkRequirements()
{
    // Comprova que la base de dades d'imatges estigui consistent, comprovant que no haguessin quedat estudis a mig descarregar l'última
    // vegada que es va tancar l'starviewer, i si és així esborra les imatges i deixa la base de dades en un estat consistent
    checkDatabaseImageIntegrity();
    // Comprova que el port pel qual es reben les descàrregues d'objectes dicom del PACS no estigui ja en ús
    checkIncomingConnectionsPacsPortNotInUse();
}

void QueryScreen::checkIncomingConnectionsPacsPortNotInUse()
{
    int localPort = Settings().getValue(InputOutputSettings::IncomingDICOMConnectionsPort).toInt();

    if (PortInUse().isPortInUse(localPort))
    {
        QString message = tr("Port %1 for incoming connections from PACS is already in use by another application.").arg(localPort);
        message += "\n\n";
        message += tr("Unable to retrieve studies from PACS if the port is in use, please close the application that is using port %1 or change "
                      "the port for incoming connections from PACS in the configuration screen.").arg(localPort);

        QMessageBox::warning(this, ApplicationNameString, message);
    }
}

void QueryScreen::checkDatabaseImageIntegrity()
{
    LocalDatabaseManager localDatabaseManager;

    localDatabaseManager.deleteStudyBeingRetrieved();

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
        m_advancedSearchPushButton->setText(m_advancedSearchPushButton->text().replace(">>", "<<"));
    }
    else
    {
        m_qadvancedSearchWidget->clear();
        m_advancedSearchPushButton->setText(m_advancedSearchPushButton->text().replace("<<", ">>"));
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
    this->show();
    if (this->isMinimized())
    {
        this->setWindowState(this->windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
    }
    else
    {
        this->raise();
        this->activateWindow();
    }
}

void QueryScreen::showPACSTab()
{
    m_tab->setCurrentIndex(PACSQueryTab);
    bringToFront();
}

void QueryScreen::showLocalExams()
{
    m_tab->setCurrentIndex(LocalDataBaseTab);
    m_qbasicSearchWidget->clear();
    m_qbasicSearchWidget->setDefaultDate(QBasicSearchWidget::AnyDate);
    m_qadvancedSearchWidget->clear();
    m_qInputOutputLocalDatabaseWidget->queryStudy(DicomMask());
    bringToFront();
}

void QueryScreen::searchStudy()
{
    switch (m_tab->currentIndex())
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

void QueryScreen::viewStudyFromDatabase(QString studyInstanceUID)
{
    // Indiquem que volem veure un estudi que està guardat a la base de dades
    m_qInputOutputLocalDatabaseWidget->view(studyInstanceUID);
}

void QueryScreen::loadStudyFromDatabase(QString studyInstanceUID)
{
    // Indiquem que volem veure un estudi que està guardat a la base de dades
    m_qInputOutputLocalDatabaseWidget->view(studyInstanceUID, true);
}

void QueryScreen::sendDicomObjectsToPacs(PacsDevice pacsDevice, QList<Image*> images)
{
    m_qInputOutputLocalDatabaseWidget->sendDICOMFilesToPACS(pacsDevice, images);
}

void QueryScreen::refreshTab(int index)
{
    switch (index)
    {
        case LocalDataBaseTab:
                m_qbasicSearchWidget->setEnabledSeriesModality(true);
                m_qadvancedSearchWidget->setEnabled(false);
                break;

        case PACSQueryTab:
                m_qbasicSearchWidget->setEnabledSeriesModality(true);
                m_qadvancedSearchWidget->setEnabled(true);
                break;

        case DICOMDIRTab:
                m_qbasicSearchWidget->setEnabledSeriesModality(false);
                m_qadvancedSearchWidget->setEnabled(false);
                break;
    }
    updatePACSNodesVisibility();
}

void QueryScreen::updatePACSNodesVisibility()
{
    if (m_tab->currentIndex() == PACSQueryTab)
    {
        m_showPACSNodesPushButton->show();
        m_PACSNodes->setVisible(m_showPACSNodesPushButton->isChecked());
    }
    else
    {
        m_showPACSNodesPushButton->hide();
        m_PACSNodes->hide();
    }
}

void QueryScreen::viewPatients(QList<Patient*> listPatientsToView, bool loadOnly)
{
    // Si fem un "view" amagarem les finestres de la QueryScreen perquè
    // l'aplicació i les extensions siguin visibles, altrament no amagarem res
    if (!loadOnly)
    {
        // S'amaga per poder visualitzar la serie
        this->close();
    }

    emit selectedPatients(listPatientsToView, loadOnly);
}

#ifndef STARVIEWER_LITE
void QueryScreen::showOperationStateScreen()
{
    if (!m_operationStateScreen->isVisible())
    {
        m_operationStateScreen->setVisible(true);
    }
    else
    {
        m_operationStateScreen->raise();
        m_operationStateScreen->activateWindow();
    }
}
#endif

void QueryScreen::openDicomdir()
{
    if (m_qInputOutputDicomdirWidget->openDicomdir())
    {
        this->bringToFront();
        // Mostra el tab del dicomdir
        m_tab->setCurrentIndex(DICOMDIRTab);
    }
}

DicomMask QueryScreen::buildDicomMask()
{
    return m_qbasicSearchWidget->buildDicomMask() + m_qadvancedSearchWidget->buildDicomMask();
}

void QueryScreen::closeEvent(QCloseEvent *event)
{
    // Guardem els settings
    writeSettings();

    // TODO: Des del desctructor d'ExtensionHander quan no queda cap més QApplicationMaingWindow oberta s'invoca el mètode close de la QueryScreen perquè es
    // tanquin totes les finestres depenents de la QueryScreen, això provoca que es llenci el signal lastWindowClosed el qual hi responem invocant
    // el mètode quit des de main.cpp. Per això quan s'invoca el mètode close() de la QueryScreen és necessari tancar totes les finestres obertes
    // des de la QueryScreen perquè Starviewer es tanqui en cas que no hi ha hagi cap visor QApplicationMainWindow.
#ifndef STARVIEWER_LITE
    // Tanquem la QOperationStateScreen al tancar la QueryScreen
    m_operationStateScreen->close();
#endif
    m_qcreateDicomdir->close();

    event->accept();

    emit closed();
}

void QueryScreen::readSettings()
{
    Settings settings;
    settings.restoreGeometry(InputOutputSettings::QueryScreenGeometry, this);
    // Aquesta clau substitueix les obsoletes "queryScreenWindowPositionX", "queryScreenWindowPositionY", "queryScreenWindowWidth" i "queryScreenWindowHeight"
    // que tenien les claus /interface/queryscreen/ + windowPositionX, windowPositionY, windowWidth i windowHeigth respectivament
    // TODO fer neteja d'aquestes claus antigues amb la migració de dades

#ifndef STARVIEWER_LITE
    m_showPACSNodesPushButton->setChecked(settings.getValue(InputOutputSettings::QueryScreenPACSListIsVisible).toBool());
#endif
}

void QueryScreen::writeSettings()
{
    // Només guardem els settings quan la interfície ha estat visible, ja que hi ha settings com el QSplitter que si la primera vegada
    // que executem l'starviewer no obrim la QueryScreen retorna un valors incorrecte per això, el que fem és comprova que la QueryScreen
    // hagi estat visible per guardar el settings
    if (this->isVisible())
    {
        Settings settings;
        settings.saveGeometry(InputOutputSettings::QueryScreenGeometry, this);
    }
}

void QueryScreen::retrieveStudy(QInputOutputPacsWidget::ActionsAfterRetrieve actionAfterRetrieve, const PacsDevice &pacsDevice, Study *study)
{
    // QueryScreen rep un signal cada vegada que qualsevol estudis en el procés de descàrrega canvia d'estat,
    // en principi només ha de reemetre aquests signals cap a fora quan és un signal que afecta un estudi
    // sol·licitat a través d'aquest mètode públic, per això mantenim aquesta llista que ens indica els estudis
    // pendents de descarregar sol·licitats a partir d'aquest mètode
    m_studyRequestedToRetrieveFromPublicMethod.append(study->getInstanceUID());

    m_qInputOutputPacsWidget->retrieve(pacsDevice, actionAfterRetrieve, study);
}

void QueryScreen::studyRetrieveFailedSlot(QString studyInstanceUID)
{
    if (m_studyRequestedToRetrieveFromPublicMethod.contains(studyInstanceUID))
    {
        // És un estudi dels que ens han demanat des del mètode públic
        m_studyRequestedToRetrieveFromPublicMethod.removeOne(studyInstanceUID);

        emit studyRetrieveFailed(studyInstanceUID);
    }
}

void QueryScreen::studyRetrieveFinishedSlot(QString studyInstanceUID)
{
    if (m_studyRequestedToRetrieveFromPublicMethod.contains(studyInstanceUID))
    {
        // És un estudi dels que ens han demanat des del mètode públic
        m_studyRequestedToRetrieveFromPublicMethod.removeOne(studyInstanceUID);

        emit studyRetrieveFinished(studyInstanceUID);
    }
}

void QueryScreen::studyRetrieveStartedSlot(QString studyInstanceUID)
{
    if (m_studyRequestedToRetrieveFromPublicMethod.contains(studyInstanceUID))
    {
        // És un estudi dels que ens han demanat des del mètode públic
        emit studyRetrieveStarted(studyInstanceUID);
    }
}

void QueryScreen::studyRetrieveCancelledSlot(QString studyInstanceUID)
{
    if (m_studyRequestedToRetrieveFromPublicMethod.contains(studyInstanceUID))
    {
        // És un estudi dels que ens han demanat des del mètode públic
        m_studyRequestedToRetrieveFromPublicMethod.removeOne(studyInstanceUID);

        emit studyRetrieveCancelled(studyInstanceUID);
    }
}
void QueryScreen::newPACSJobEnqueued(PACSJobPointer pacsJob)
{
    if (pacsJob->getPACSJobType() == PACSJob::SendDICOMFilesToPACSJobType || pacsJob->getPACSJobType() == PACSJob::RetrieveDICOMFilesFromPACSJobType)
    {
        m_operationAnimation->show();
        m_labelOperation->show();
        connect(pacsJob.data(), SIGNAL(PACSJobFinished(PACSJobPointer)), SLOT(pacsJobFinishedOrCancelled(PACSJobPointer)));
        connect(pacsJob.data(), SIGNAL(PACSJobCancelled(PACSJobPointer)), SLOT(pacsJobFinishedOrCancelled(PACSJobPointer)));

        // Indiquem que tenim un PACSJob més pendent de finalitzar
        m_PACSJobsPendingToFinish++;
    }
}

void QueryScreen::pacsJobFinishedOrCancelled(PACSJobPointer) {
    // No podem utilitzar isExecutingPACSJob per controlar si hi ha jobs pendents d'executar, perquè algunes vegades ens
    // hem trobat que tot i no tenir cap job pendent d'executar, el mètode respón que hi ha algun job executant-se.
    // Això passa algunes vegades quan s'aten el signal PACSJobFinished d'un job de seguida i es pregunta al mètode isExecutingPACSJob
    // si hi ha jobs executant-se, semblaria que tot i haver finalitzar l'últim job pendent ThreadWeaver està acabant de fer
    // alguna acció i per això indica que hi ha jobs executant-se
    m_PACSJobsPendingToFinish--;

    if (m_PACSJobsPendingToFinish == 0)
    {
        m_operationAnimation->hide();
        m_labelOperation->hide();
    }
}

};
