/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qinputoutputpacswidget.h"

#include <QMessageBox>
#include <QString>
#include <QFileDialog>
#include <QFileInfo>
#include <QShortcut>

#include "starviewersettings.h"
#include "status.h"
#include "logging.h"
#include "starviewerapplication.h"
#include "dicommask.h"
#include "patient.h"
#include "statswatcher.h"
#include "pacsparameters.h"
#include "multiplequerystudy.h"
#include "querypacs.h"
#include "pacsserver.h"
#include "pacslistdb.h"
#include "pacsconnection.h"
#include "study.h"
#include "qoperationstatescreen.h"
#include "processimagesingleton.h"

namespace udg
{

QInputOutputPacsWidget::QInputOutputPacsWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);

    createContextMenuQStudyTreeWidget();

    setQStudyTreeWidgetColumnsWidth();

    m_multipleQueryStudy = new MultipleQueryStudy();

    m_processImageSingleton = ProcessImageSingleton::getProcessImageSingleton();
    m_processImageSingleton->setPath(StarviewerSettings().getCacheImagePath());

    m_statsWatcher = new StatsWatcher("QueryInputOutputPacsWidget",this);
    m_statsWatcher->addClicksCounter(m_viewButton);
    m_statsWatcher->addClicksCounter(m_retrieveButton);
}

QInputOutputPacsWidget::~QInputOutputPacsWidget()
{
    saveQStudyTreeWidgetColumnsWidth();
}

void QInputOutputPacsWidget::createConnections()
{
    connect(m_studyTreeWidget, SIGNAL(studyExpanded(QString)), SLOT(expandSeriesOfStudy(QString)));
    connect(m_studyTreeWidget, SIGNAL(seriesExpanded(QString, QString)), SLOT(expandImagesOfSeries(QString, QString)));

    connect(m_studyTreeWidget, SIGNAL(studyDoubleClicked()), SLOT(view()));
    connect(m_studyTreeWidget, SIGNAL(seriesDoubleClicked()), SLOT(view()));
    connect(m_studyTreeWidget, SIGNAL(imageDoubleClicked()), SLOT(view()));

    connect(m_viewButton, SIGNAL(clicked()), SLOT(view()));
    connect(m_retrieveButton, SIGNAL(clicked()), SLOT(retrieveSelectedStudies()));


    //connecta els signals el qexecute operation thread amb els de qretrievescreen, per coneixer quant s'ha descarregat una imatge, serie, estudi, si hi ha error, etc..
    connect(&m_qexecuteOperationThread, SIGNAL(setErrorOperation(QString)), m_qoperationStateScreen, SLOT(setErrorOperation(QString)));
    connect(&m_qexecuteOperationThread, SIGNAL(errorInOperation(QString, QString, QExecuteOperationThread::OperationError)), m_qoperationStateScreen, SLOT(setErrorOperation(QString)));

    connect(&m_qexecuteOperationThread, SIGNAL(setOperationFinished(QString)), m_qoperationStateScreen, SLOT(setOperationFinished(QString)));

    connect(&m_qexecuteOperationThread, SIGNAL(setOperating(QString)), m_qoperationStateScreen, SLOT(setOperating(QString)));
    connect(&m_qexecuteOperationThread, SIGNAL(imageCommit(QString, int)), m_qoperationStateScreen, SLOT(imageCommit(QString, int)));
    connect(&m_qexecuteOperationThread, SIGNAL(currentProcessingStudyImagesRetrievedChanged(int)), m_qoperationStateScreen, SLOT(setRetrievedImagesToCurrentProcessingStudy(int)));
    connect(&m_qexecuteOperationThread, SIGNAL(seriesCommit(QString)), m_qoperationStateScreen, SLOT(seriesCommit(QString)));
    connect(&m_qexecuteOperationThread, SIGNAL(newOperation(Operation *)), m_qoperationStateScreen, SLOT(insertNewOperation(Operation *)));
    connect(&m_qexecuteOperationThread, SIGNAL(studyWillBeDeleted(QString)), SIGNAL(studyWillBeDeletedFromDatabase(QString)));
    connect(&m_qexecuteOperationThread, SIGNAL(setCancelledOperation(QString)), m_qoperationStateScreen, SLOT(setCancelledOperation(QString)));

    connect(&m_qexecuteOperationThread, SIGNAL(retrieveFinished(QString)), SIGNAL(studyRetrieved(QString)));
    
    // Label d'informació (cutre-xapussa)
    connect(&m_qexecuteOperationThread, SIGNAL(errorInOperation(QString, QString, QExecuteOperationThread::OperationError)), SIGNAL(operationStateChange()));
    connect(&m_qexecuteOperationThread, SIGNAL(setErrorOperation(QString)), SIGNAL(operationStateChange()));
    connect(&m_qexecuteOperationThread, SIGNAL(setOperationFinished(QString)), SIGNAL(operationStateChange()));
    connect(&m_qexecuteOperationThread, SIGNAL(newOperation(Operation *)), SIGNAL(operationStateChange()));
    connect(&m_qexecuteOperationThread, SIGNAL(setCancelledOperation(QString)), SIGNAL(operationStateChange()));

    //connect tracta els errors de connexió al PACS
    connect (m_multipleQueryStudy, SIGNAL(errorConnectingPacs(QString)), SLOT(errorConnectingPacs(QString)));
    connect (m_multipleQueryStudy, SIGNAL(errorQueringStudiesPacs(QString)), SLOT(errorQueringStudiesPacs(QString)));

    //connect tracta els errors de connexió al PACS, al descarregar imatges
    connect (&m_qexecuteOperationThread, SIGNAL(errorInOperation(QString, QString, QExecuteOperationThread::OperationError)), SLOT(showQExecuteOperationThreadError(QString, QString, QExecuteOperationThread::OperationError)));

    //connecta el signal que emiteix qexecuteoperationthread, per visualitzar un estudi amb aquesta classe
    connect(&m_qexecuteOperationThread, SIGNAL(viewStudy(QString, QString, QString)), this, SIGNAL(viewRetrievedStudy(QString)), Qt::QueuedConnection);
}

void  QInputOutputPacsWidget::createContextMenuQStudyTreeWidget()
{
    QAction *action;

    action = m_contextMenuQStudyTreeWidget.addAction(QIcon(":/images/view.png"), tr("&View"), this, SLOT(view()), tr("Ctrl+V"));
    (void) new QShortcut(action->shortcut(), this, SLOT(view()));

    action = m_contextMenuQStudyTreeWidget.addAction(QIcon(":/images/retrieve.png"), tr("&Retrieve"), this, SLOT(retrieveSelectedStudies()), tr("Ctrl+R"));
    (void) new QShortcut(action->shortcut(), this, SLOT(retrieveSelectedStudies()));

    m_studyTreeWidget->setContextMenu(& m_contextMenuQStudyTreeWidget); //Especifiquem que es el menu del dicomdir
}

void QInputOutputPacsWidget::setQStudyTreeWidgetColumnsWidth()
{
    StarviewerSettings settings;

    for (int column = 0; column < m_studyTreeWidget->getNumberOfColumns(); column++)
    {
        m_studyTreeWidget->setColumnWidth(column, settings.getStudyPacsListColumnWidth(column));
    }
}

void QInputOutputPacsWidget::saveQStudyTreeWidgetColumnsWidth()
{
    StarviewerSettings settings;

    for (int column = 0; column < m_studyTreeWidget->getNumberOfColumns(); column++)
    {
        settings.setStudyPacsListColumnWidth(column, m_studyTreeWidget->getColumnWidth(column));
    }
}

void QInputOutputPacsWidget::queryStudy(DicomMask queryMask, QList<PacsParameters> pacsToQuery)
{
    if (AreValidQueryParameters(&queryMask, pacsToQuery))
    {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        StatsWatcher::log("Cerca al PACS amb paràmetres: " + queryMask.getFilledMaskFields());

        Status queryStatus = queryMultiplePacs(queryMask, pacsToQuery, m_multipleQueryStudy);

        if(!queryStatus.good())  //no fem la query
        {
            m_studyTreeWidget->clear();
            QApplication::restoreOverrideCursor();
            QMessageBox::information(this, ApplicationNameString, tr("An error has produced while querying.\n\nRepeat it, if the problem persist contact with an administrator."));
            return;
        }

        if (m_multipleQueryStudy->getPatientStudyList().isEmpty())
        {
            m_studyTreeWidget->clear();
            QApplication::restoreOverrideCursor();
            QMessageBox::information(this, ApplicationNameString, tr("No study match found."));
            return;
        }
        m_hashPacsIDOfStudyInstanceUID = m_multipleQueryStudy->getHashTablePacsIDOfStudyInstanceUID();

        //fem que es visualitzi l'studyView seleccionat
        m_studyTreeWidget->insertPatientList(m_multipleQueryStudy->getPatientStudyList());  
        m_studyTreeWidget->setSortColumn(QStudyTreeWidget::ObjectName);

        QApplication::restoreOverrideCursor();
    }
}

void QInputOutputPacsWidget::storeStudiesToPacs()
{
    /*QList<PacsParameters> selectedPacsList;
    QStringList studiesUIDList = m_studyTreeWidgetCache->getSelectedStudiesUID();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    StarviewerSettings settings;

    selectedPacsList = m_PACSNodes->getSelectedPacs(); //Emplemen el pacsList amb les pacs seleccionats al QPacsList

    if(selectedPacsList.size() == 0)
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, ApplicationNameString, tr("You have to select a PACS to store the study in"));
    }
    else if(selectedPacsList.size() == 1)
    {
        StarviewerSettings settings;
        foreach(QString studyUID, studiesUIDList)
        {
            PacsListDB pacsListDB;
            PacsParameters pacs;
            Operation storeStudyOperation;
            Study *study;
            LocalDatabaseManager localDatabaseManager;
            QList<Patient*> patientList;

            DicomMask dicomMask;
            dicomMask.setStudyUID(studyUID);
            patientList = localDatabaseManager.queryPatientStudy(dicomMask);
//            if(showDatabaseManagerError(localDatabaseManager.getLastError()))    return;

            // \TODO Això s'ha de fer perquè queryPatientStudy retorna llista de Patients
            // Nosaltres, en realitat, volem llista d'study amb les dades de Patient omplertes.
            if(patientList.size() != 1 && patientList.first()->getNumberOfStudies() != 1)
            {
//                showDatabaseManagerError(LocalDatabaseManager::DatabaseCorrupted);
                return;
            }

            study = patientList.first()->getStudies().first();
            Patient *patient = study->getParentPatient();

            storeStudyOperation.setPatientName(patient->getFullName());
            storeStudyOperation.setPatientID(patient->getID());
            storeStudyOperation.setStudyUID(study->getInstanceUID());
            storeStudyOperation.setStudyID(study->getID());
            storeStudyOperation.setPriority(Operation::Low);
            storeStudyOperation.setOperation(Operation::Move);
            storeStudyOperation.setDicomMask(dicomMask);

            delete patient;
            //cerquem els paràmetres del Pacs al qual s'han de cercar les dades
            pacs = pacsListDB.queryPacs(selectedPacsList.value(0).getPacsID());
            pacs.setAELocal(settings.getAETitleMachine());
            pacs.setTimeOut(settings.getTimeout().toInt());
            storeStudyOperation.setPacsParameters(pacs);

            m_qexecuteOperationThread.queueOperation(storeStudyOperation);
        }
    }
    else
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, ApplicationNameString, tr("The studies can only be stored to one PACS"));
    }

    QApplication::restoreOverrideCursor();*/
}

void QInputOutputPacsWidget::clear()
{
    m_studyTreeWidget->clear();
}

void QInputOutputPacsWidget::setQOperationStateScreen(QOperationStateScreen *qoperationStateScreen)
{
    m_qoperationStateScreen = qoperationStateScreen;

    createConnections();
}

void QInputOutputPacsWidget::expandSeriesOfStudy(QString studyInstanceUID)
{
    Status state;
    QString text, pacsDescription, pacsID = getPacsIDFromQueriedStudies(studyInstanceUID);
    QueryPacs querySeriesPacs;

    PacsServer pacsServer = getPacsServerByPacsID(pacsID);
    pacsDescription = pacsServer.getPacs().getAEPacs() + " Institució" + pacsServer.getPacs().getInstitution()  + " IP:" + pacsServer.getPacs().getPacsAddress();

    INFO_LOG("Cercant informacio de les sèries de l'estudi" + studyInstanceUID + " del PACS " + pacsDescription);

    state = pacsServer.connect(PacsServer::query, PacsServer::seriesLevel);
    if (!state.good())
    {
        //Error al connectar
        ERROR_LOG("Error al connectar al PACS " + pacsDescription + ". PACS ERROR : " + state.text());
        errorConnectingPacs (pacsID);
        return;
    }

    querySeriesPacs.setConnection(pacsID, pacsServer.getConnection());
    state = querySeriesPacs.query(buildSeriesDicomMask(studyInstanceUID));
    pacsServer.disconnect();

    if (!state.good())
    {
        //Error a la query
        ERROR_LOG("QueryScreen::QueryPacs : Error cercant les sèries al PACS " + pacsDescription + ". PACS ERROR : " + state.text());

        text = tr("Error! Can't query series to PACS named %1").arg(pacsDescription);
        QMessageBox::warning(this, ApplicationNameString, text);
        return;
    }

    if (querySeriesPacs.getQueryResultsAsSeriesList().isEmpty())
    {
        QMessageBox::information(this, ApplicationNameString, tr("No series match for this study.\n"));
        return;
    }

    m_studyTreeWidget->insertSeriesList(studyInstanceUID, querySeriesPacs.getQueryResultsAsSeriesList());
}

void QInputOutputPacsWidget::expandImagesOfSeries(QString studyInstanceUID, QString seriesInstanceUID)
{
    Status state;
    QString text, pacsDescription, pacsID = getPacsIDFromQueriedStudies(studyInstanceUID);
    QueryPacs queryImages;
    DicomMask dicomMask = buildImageDicomMask(studyInstanceUID, seriesInstanceUID);

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    PacsServer pacsServer = getPacsServerByPacsID(pacsID);
    pacsDescription = pacsServer.getPacs().getAEPacs() + " Institució" + pacsServer.getPacs().getInstitution()  + " IP:" + pacsServer.getPacs().getPacsAddress(); 

    INFO_LOG("Cercant informacio de les imatges de l'estudi" + studyInstanceUID + " serie " + seriesInstanceUID + " del PACS " + pacsDescription);

    state = pacsServer.connect(PacsServer::query,PacsServer::imageLevel);
    if (!state.good())
    {   //Error al connectar
        QApplication::restoreOverrideCursor();
        ERROR_LOG("Error al connectar al PACS " + pacsDescription + ". PACS ERROR : " + state.text());
        errorConnectingPacs (pacsID);
        return;
    }

    queryImages.setConnection(pacsID, pacsServer.getConnection());

    state = queryImages.query(dicomMask);
    if (!state.good())
    {
        //Error a la query
        QApplication::restoreOverrideCursor();
        ERROR_LOG("QueryScreen::QueryPacs : Error cercant les images al PACS " + pacsDescription + ". PACS ERROR : " + state.text());

        text = tr("Error! Can't query images to PACS named %1 ").arg(pacsDescription);
        QMessageBox::warning(this, ApplicationNameString, text);
        return;
    }

    pacsServer.disconnect();

    if (queryImages.getQueryResultsAsImageList().isEmpty())
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::information(this, ApplicationNameString, tr("No images match for this series.\n"));
        return;
    }

    m_studyTreeWidget->insertImageList(studyInstanceUID, seriesInstanceUID, queryImages.getQueryResultsAsImageList());

    QApplication::restoreOverrideCursor();
}

void QInputOutputPacsWidget::retrieveSelectedStudies(bool view)
{
    QList<Study*> selectedStudies = m_studyTreeWidget->getSelectedStudies();

    if(selectedStudies.isEmpty())
    {
        QApplication::restoreOverrideCursor();
        if(view)
            QMessageBox::warning(this, ApplicationNameString, tr("Select a study to view "));
        else
            QMessageBox::warning(this, ApplicationNameString, tr("Select a study to download "));

        return;
    }

    foreach(Study *studyToRetrieve, selectedStudies)
    {
        DicomMask maskStudyToRetrieve;

        maskStudyToRetrieve.setStudyUID(studyToRetrieve->getInstanceUID());

        // TODO aquí només tenim en compte l'última sèrie o imatge seleccionada
        // per tant si seleccionem més d'una sèrie/imatge només s'en baixarà una
        // Caldria fer possible que es baixi tants com en seleccionem
        if (!m_studyTreeWidget->getCurrentSeriesUID().isEmpty())
            maskStudyToRetrieve.setSeriesUID(m_studyTreeWidget->getCurrentSeriesUID());

        if (!m_studyTreeWidget->getCurrentImageUID().isEmpty())
            maskStudyToRetrieve.setSOPInstanceUID(m_studyTreeWidget->getCurrentImageUID());

        retrieve(view, getPacsIDFromQueriedStudies(studyToRetrieve->getInstanceUID()), maskStudyToRetrieve, studyToRetrieve);
    }
}

void QInputOutputPacsWidget::view()
{
    retrieveSelectedStudies(true);
}

Status QInputOutputPacsWidget::queryMultiplePacs(DicomMask searchMask, QList<PacsParameters> listPacsToQuery, MultipleQueryStudy *multipleQueryStudy)
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

    multipleQueryStudy->setMask(searchMask); //assignem la mascara
    multipleQueryStudy->setPacsList(filledPacsParameters);
    return multipleQueryStudy->StartQueries();
}

void QInputOutputPacsWidget::retrieve(bool view, QString pacsIdToRetrieve, DicomMask maskStudyToRetrieve, Study *studyToRetrieve)
{
    StarviewerSettings settings;
    QString defaultSeriesUID;
    Operation operation;
    PacsParameters pacs;

    QApplication::setOverrideCursor(QCursor (Qt::WaitCursor));

    //busquem els paràmetres del pacs del qual volem descarregar l'estudi
    PacsListDB pacsListDB;
    pacs = pacsListDB.queryPacs(pacsIdToRetrieve);

    //emplanem els parametres locals per conenctar amb el pacs amb dades del starviewersettings
    pacs.setAELocal(settings.getAETitleMachine());
    pacs.setTimeOut(settings.getTimeout().toInt(NULL, 10));
    pacs.setLocalPort(settings.getLocalPort());

    //definim l'operació
    operation.setPacsParameters(pacs);
    operation.setDicomMask(maskStudyToRetrieve);
    if (view)
    {
        operation.setOperation(Operation::View);
        operation.setPriority(Operation::Medium);//Té priorita mitjà per passar al davant de les operacions de Retrieve
    }
    else
    {
        operation.setOperation(Operation::Retrieve);
        operation.setPriority(Operation::Low);
    }
    //emplenem les dades de l'operació
    operation.setPatientName(studyToRetrieve->getParentPatient()->getFullName());
    operation.setPatientID(studyToRetrieve->getParentPatient()->getID());
    operation.setStudyID(studyToRetrieve->getID());
    operation.setStudyUID(maskStudyToRetrieve.getStudyUID());

    m_qexecuteOperationThread.queueOperation(operation);

    QApplication::restoreOverrideCursor();
}

bool QInputOutputPacsWidget::AreValidQueryParameters(DicomMask *maskToQuery, QList<PacsParameters> pacsToQuery)
{
    if (pacsToQuery.isEmpty()) //es comprova que hi hagi pacs seleccionats
    {
        QMessageBox::warning(this, ApplicationNameString, tr("Please select a PACS to query"));
        return false;
    }

    if (maskToQuery->isAHeavyQuery()) //Es comprova que la consulta no sigui molt llarga
    {
        QMessageBox::StandardButton response = QMessageBox::question(this, ApplicationNameString,
                                                                     tr("This query can take a long time.\nDo you want continue?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        return (response == QMessageBox::Yes);
    }
    else return true;
}

QString QInputOutputPacsWidget::getPacsIDFromQueriedStudies(QString studyInstanceUID)
{
    /*TODO Tenir en compte que podem tenir un studyUID repetit en dos PACS, ara mateix no ho tenim contemplat a la QHash  */
    if (!m_hashPacsIDOfStudyInstanceUID.contains(studyInstanceUID))
    {
        ERROR_LOG(QString("No s'ha trobat a quin PACS pertany l'estudi %1 a la QHash").arg(studyInstanceUID));
        return "";
    }
    else
        return m_hashPacsIDOfStudyInstanceUID[studyInstanceUID];
}

PacsServer QInputOutputPacsWidget::getPacsServerByPacsID(QString pacsID)
{
    PacsParameters pacsParameters;
    PacsListDB pacsListDB;
    pacsParameters = pacsListDB.queryPacs(pacsID);//cerquem els paràmetres del Pacs 

    StarviewerSettings settings;
    pacsParameters.setAELocal(settings.getAETitleMachine()); //especifiquem el nostres AE
    pacsParameters.setTimeOut(settings.getTimeout().toInt(NULL, 10)); //li especifiquem el TimeOut

    PacsServer pacsServer;
    pacsServer.setPacs(pacsParameters);

    return pacsServer;
}

void QInputOutputPacsWidget::errorConnectingPacs(QString IDPacs)
{
    PacsListDB pacsListDB;
    PacsParameters errorPacs;
    QString errorMessage;

    errorPacs = pacsListDB.queryPacs(IDPacs);

    errorMessage = tr("Can't connect to PACS %1 from %2.\nBe sure that the IP and AETitle of the PACS are correct.")
        .arg(errorPacs.getAEPacs())
        .arg(errorPacs.getInstitution());

    QMessageBox::critical(this, ApplicationNameString, errorMessage);
}

DicomMask QInputOutputPacsWidget::buildSeriesDicomMask(QString studyInstanceUID)
{
    DicomMask mask;

    mask.setStudyUID(studyInstanceUID);
    mask.setSeriesDate("");
    mask.setSeriesTime("");
    mask.setSeriesModality("");
    mask.setSeriesNumber("");
    mask.setSeriesUID("");
    mask.setPPSStartDate("");
    mask.setPPStartTime("");
    mask.setRequestAttributeSequence("", "");

    return mask;
}

DicomMask QInputOutputPacsWidget::buildImageDicomMask(QString studyInstanceUID, QString seriesInstanceUID)
{
    DicomMask mask;

    mask.setStudyUID(studyInstanceUID);
    mask.setSeriesUID(seriesInstanceUID);
    mask.setImageNumber("");
    mask.setSOPInstanceUID("");

    return mask;
}

void QInputOutputPacsWidget::errorQueringStudiesPacs(QString PacsID)
{
    PacsListDB pacsListDB;
    PacsParameters errorPacs;
    QString errorMessage;

    errorPacs = pacsListDB.queryPacs(PacsID);
    errorMessage = tr("Can't query PACS %1 from %2\nBe sure that the IP and AETitle of this PACS are correct")
        .arg(errorPacs.getAEPacs())
        .arg(errorPacs.getInstitution()
  );

    QMessageBox::critical(this, ApplicationNameString, errorMessage);
}

void QInputOutputPacsWidget::showQExecuteOperationThreadError(QString studyInstanceUID, QString pacsID, QExecuteOperationThread::OperationError error)
{
    QString message;
    StarviewerSettings settings;
    PacsParameters pacs = PacsListDB().queryPacs(pacsID);

    switch (error)
    {
        case QExecuteOperationThread::ErrorConnectingPacs :
            message = tr("Please review the operation list screen, ");
            message += tr("%1 can't connect to PACS %2 trying to retrieve or store a study.\n").arg(ApplicationNameString, pacs.getAEPacs());
            message += tr("\nBe sure that your computer is connected on network and the Pacs parameters are correct.");
            message += tr("\nIf the problem persist contact with an administrator.");
            QMessageBox::critical(this, ApplicationNameString, message);
            break;
        case QExecuteOperationThread::ErrorRetrieving :
            message = tr("Please review the operation list screen, ");
            message += tr("an error ocurred retrieving a study.\n");
            message += tr("\nPACS %1 doesn't respond correctly, be sure that your computer is connected on network and the PACS parameters are correct.").arg(pacs.getAEPacs());
            message += tr("\nIf the problem persist contact with an administrator.");
            QMessageBox::critical(this, ApplicationNameString, message);
            break;
        case QExecuteOperationThread::MoveDestinationAETileUnknown:
            message = tr("Please review the operation list screen, ");
            message += tr("PACS %1 doesn't recognize your computer's AETitle %2 and some studies can't be retrieved.").arg(pacs.getAEPacs(), settings.getAETitleMachine());
            message += tr("\n\nContact with an administrador to register your computer to the PACS.");
            QMessageBox::warning(this, ApplicationNameString, message);
            break;
        case QExecuteOperationThread::NoEnoughSpace :
            message = tr("There is not enough space to retrieve studies, please free space.");
            message += tr("\nAll pending retrieve operations will be cancelled.");
            QMessageBox::warning(this, ApplicationNameString, message);
            break;
        case QExecuteOperationThread::ErrorFreeingSpace :
            message = tr("Please review the operation list screen, ");
            message += tr("an error ocurred freeing space and some operations may have failed.");
            message += tr("\n\nClose all %1 windows and try again."
                         "\nIf the problem persist contact with an administrator.").arg(ApplicationNameString);
            QMessageBox::critical(this, ApplicationNameString, message);
            break;
        case QExecuteOperationThread::DatabaseError :
            message = tr("Please review the operation list screen, ");
            message += tr("a database error ocurred and some operations may have failed.");
            message += tr("\n\nClose all %1 windows and try again."
                         "\nIf the problem persist contact with an administrator.").arg(ApplicationNameString);
            QMessageBox::critical(this, ApplicationNameString, message);
            break;
       case QExecuteOperationThread::PatientInconsistent :
            message = tr("Please review the operation list screen, ");
            message += tr("an error ocurred and some operations may have failed.");
            message += tr("\n%1 has not be capable of read correctly dicom information of the study.").arg(ApplicationNameString);
            message += tr("\n\nThe study may be corrupted, if It is not corrupted please contact with %1 team.").arg(ApplicationNameString);
            QMessageBox::critical(this, ApplicationNameString, message);
            break;
	   case QExecuteOperationThread::MoveRefusedOutOfResources :
			message = tr("Please review the operation list screen, ");
            message += tr("PACS %1 is out of resources and can't process the request for retrieving a study.").arg(pacs.getAEPacs());
            message += tr("\n\nTry later to retrieve the study, if the problem persists please contact with PACS administrator to solve the problem.");
            QMessageBox::critical(this, ApplicationNameString, message);
            break;
       case QExecuteOperationThread::IncomingConnectionsPortPacsInUse :
            message = tr("Port %1 for incoming connections from PACS is already in use by another application.").arg(StarviewerSettings().getLocalPort());
            message += tr("\n\n%1 can't retrieve the studies, all pending retrieve operations will be cancelled.").arg(ApplicationNameString);
            message += tr("\n\nIf there is another %1 window retrieving studies from the PACS please wait until those retrieving has finished and try again.").arg(ApplicationNameString);
            QMessageBox::critical(this, ApplicationNameString, message);
            break;
        default:
            message = tr("Please review the operation list screen, an unknown error has ocurred retrieving a study.");
            message += tr("\n\nClose all %1 windows and try again."
                         "\nIf the problem persist contact with an administrator.").arg(ApplicationNameString);
    }
}

};
