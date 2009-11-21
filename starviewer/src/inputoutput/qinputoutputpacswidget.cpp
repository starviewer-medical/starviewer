/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qinputoutputpacswidget.h"

#include <QMessageBox>
#include <QShortcut>
#include <QMovie>

#include "inputoutputsettings.h"
#include "status.h"
#include "logging.h"
#include "starviewerapplication.h"
#include "dicommask.h"
#include "patient.h"
#include "statswatcher.h"
#include "pacsdevice.h"
#include "querypacs.h"
#include "pacsdevicemanager.h"
#include "study.h"
#include "qoperationstatescreen.h"
#include "processimagesingleton.h"
#include "localdatabasemanager.h"
#include "pacsmanager.h"

namespace udg
{

QInputOutputPacsWidget::QInputOutputPacsWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);

    createContextMenuQStudyTreeWidget();

    Settings settings;
    settings.restoreColumnsWidths( InputOutputSettings::PacsStudyListColumnsWidth, m_studyTreeWidget->getQTreeWidget() );

    m_processImageSingleton = ProcessImageSingleton::getProcessImageSingleton();
    m_processImageSingleton->setPath( LocalDatabaseManager::getCachePath());

    m_statsWatcher = new StatsWatcher("QueryInputOutputPacsWidget",this);
    m_statsWatcher->addClicksCounter(m_viewButton);
    m_statsWatcher->addClicksCounter(m_retrieveButton);

    m_pacsManager = new PacsManager();

    //Preparem el QMovie per indicar quan s'estan fent consultes al PACS
    QMovie *operationAnimation = new QMovie(this);
    operationAnimation->setFileName(":/images/loader.gif");
    m_queryAnimationLabel->setMovie(operationAnimation);
    operationAnimation->start();

    setQueryInProgress(false);

    //Indiquem que el QStudyTreeWidget inicialment s'ordenarà pel la columna name
    m_studyTreeWidget->setSortColumn(QStudyTreeWidget::ObjectName);
}

QInputOutputPacsWidget::~QInputOutputPacsWidget()
{
    Settings settings;
    settings.saveColumnsWidths( InputOutputSettings::PacsStudyListColumnsWidth, m_studyTreeWidget->getQTreeWidget() );
}

void QInputOutputPacsWidget::createConnections()
{
    connect(m_studyTreeWidget, SIGNAL(studyExpanded(QString)), SLOT(expandSeriesOfStudy(QString)));
    connect(m_studyTreeWidget, SIGNAL(seriesExpanded(QString, QString)), SLOT(expandImagesOfSeries(QString, QString)));

    connect(m_studyTreeWidget, SIGNAL(studyDoubleClicked()), SLOT(retrieveSelectedStudies()));
    connect(m_studyTreeWidget, SIGNAL(seriesDoubleClicked()), SLOT(retrieveSelectedStudies()));
    connect(m_studyTreeWidget, SIGNAL(imageDoubleClicked()), SLOT(retrieveSelectedStudies()));

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

    //connect tracta els errors de connexió al PACS, al descarregar imatges
    connect (&m_qexecuteOperationThread, SIGNAL(errorInOperation(QString, QString, QExecuteOperationThread::OperationError)), SLOT(showQExecuteOperationThreadError(QString, QString, QExecuteOperationThread::OperationError)));
    //connect tracta els warning de connexió al PACS, al descarregar imatges
    connect (&m_qexecuteOperationThread, SIGNAL(warningInOperation(QString, QString, QExecuteOperationThread::OperationWarning)), SLOT(showQExecuteOperationThreadWarning(QString, QString, QExecuteOperationThread::OperationWarning)));

    //connecta el signal que emiteix qexecuteoperationthread, per visualitzar un estudi amb aquesta classe
    connect(&m_qexecuteOperationThread, SIGNAL(viewStudy(QString, QString, QString)), this, SIGNAL(viewRetrievedStudy(QString)), Qt::QueuedConnection);

    connect(m_pacsManager, SIGNAL(queryStudyResultsReceived(QList<Patient*>, QHash<QString, QString>)), SLOT(queryStudyResultsReceived(QList<Patient*>, QHash<QString, QString>)));
    connect(m_pacsManager, SIGNAL(querySeriesResultsReceived(QString, QList<Series*>)), SLOT(querySeriesResultsReceived(QString , QList<Series*>)));
    connect(m_pacsManager, SIGNAL(queryImageResultsReceived(QString, QString, QList<Image*>)), SLOT(queryImageResultsReceived(QString , QString ,QList<Image*>)));

    connect(m_pacsManager, SIGNAL(queryFinished()), SLOT(queryFinished()));

    connect(m_pacsManager, SIGNAL(errorQueryingStudy(PacsDevice)), SLOT(errorQueryingStudy(PacsDevice)));
    connect(m_pacsManager, SIGNAL(errorQueryingSeries(QString, PacsDevice)), SLOT(errorQueryingSeries(QString, PacsDevice)));
    connect(m_pacsManager, SIGNAL(errorQueryingImage(QString, QString, PacsDevice)), SLOT(errorQueryingImage(QString, QString, PacsDevice)));

    connect(m_cancelQueryButton, SIGNAL(clicked()), SLOT(cancelCurrentQueries()));
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

void QInputOutputPacsWidget::queryStudy(DicomMask queryMask, QList<PacsDevice> pacsToQuery)
{
    if (pacsToQuery.count() == 0)
    {
        QMessageBox::information(this, ApplicationNameString, tr("You have to select at least one PACS to query."));

        return;
    }

    if (AreValidQueryParameters(&queryMask, pacsToQuery))
    {
        m_studyTreeWidget->clear();
        m_hashPacsIDOfStudyInstanceUID.clear();

        /*En el cas que ens facin una consulta d'estudis, si s'estan fent altres consultes les cancel·lem, ja que aquesta serà la 
          consulta principal, a partir de la qual es podran fet altres consultes de series, imatges  */
        if (m_pacsManager->isExecutingQueries())
        {
            m_pacsManager->cancelCurrentQueries();
        }

        m_pacsManager->queryStudy(queryMask, pacsToQuery);

        setQueryInProgress(true);
    }
}

void QInputOutputPacsWidget::storeDicomObjectsToPacs(PacsDevice pacsToStore, Study* studyToStore, DicomMask dicomMaskObjectsToStore)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    Operation storeStudyOperation;

    storeStudyOperation.setPatientName(studyToStore->getParentPatient()->getFullName());
    storeStudyOperation.setPatientID(studyToStore->getParentPatient()->getID());
    storeStudyOperation.setStudyUID(studyToStore->getInstanceUID());
    storeStudyOperation.setStudyID(studyToStore->getID());
    storeStudyOperation.setPriority(Operation::Low);
    storeStudyOperation.setOperation(Operation::Move);
    storeStudyOperation.setDicomMask(dicomMaskObjectsToStore);
    storeStudyOperation.setPacsDevice(pacsToStore);

    m_qexecuteOperationThread.queueOperation(storeStudyOperation);

    QApplication::restoreOverrideCursor();
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
    PacsDevice pacsDevice = PacsDeviceManager().getPACSDeviceByID(getPacsIDFromQueriedStudies(studyInstanceUID));
    QString pacsDescription;

    pacsDescription = pacsDevice.getAETitle() + " Institució" + pacsDevice.getInstitution()  + " IP:" + pacsDevice.getAddress();

    INFO_LOG("Cercant informacio de les sèries de l'estudi" + studyInstanceUID + " del PACS " + pacsDescription);

    m_pacsManager->querySeries(buildSeriesDicomMask(studyInstanceUID), pacsDevice);

    setQueryInProgress(true);
}

void QInputOutputPacsWidget::expandImagesOfSeries(QString studyInstanceUID, QString seriesInstanceUID)
{
    PacsDevice pacsDevice = PacsDeviceManager().getPACSDeviceByID(getPacsIDFromQueriedStudies(studyInstanceUID));
    QString pacsDescription;

    pacsDescription = pacsDevice.getAETitle() + " Institució" + pacsDevice.getInstitution()  + " IP:" + pacsDevice.getAddress();

    m_pacsManager->queryImage(buildImageDicomMask(studyInstanceUID, seriesInstanceUID), pacsDevice);
    setQueryInProgress(true);
}

void QInputOutputPacsWidget::retrieveSelectedStudies(bool view)
{
    if(m_studyTreeWidget->getSelectedStudiesUID().isEmpty())
    {
        QApplication::restoreOverrideCursor();
        if(view)
            QMessageBox::warning(this, ApplicationNameString, tr("Select a study to view "));
        else
            QMessageBox::warning(this, ApplicationNameString, tr("Select a study to download "));

        return;
    }

    foreach(DicomMask dicomMask, m_studyTreeWidget->getDicomMaskOfSelectedItems())
    {
        retrieve(view, getPacsIDFromQueriedStudies(dicomMask.getStudyUID()), dicomMask, m_studyTreeWidget->getStudy(dicomMask.getStudyUID()));
    }
}

void QInputOutputPacsWidget::cancelCurrentQueries()
{
    m_pacsManager->cancelCurrentQueries();
    setQueryInProgress(false);
}

void QInputOutputPacsWidget::view()
{
    retrieveSelectedStudies(true);
}

void QInputOutputPacsWidget::retrieve(bool view, QString pacsIdToRetrieve, DicomMask maskStudyToRetrieve, Study *studyToRetrieve)
{
    QString defaultSeriesUID;
    Operation operation;
    PacsDevice pacs;

    QApplication::setOverrideCursor(QCursor (Qt::WaitCursor));

    //busquem els paràmetres del pacs del qual volem descarregar l'estudi
    pacs = PacsDeviceManager().getPACSDeviceByID(pacsIdToRetrieve);

    //definim l'operació
    operation.setPacsDevice(pacs);
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

bool QInputOutputPacsWidget::AreValidQueryParameters(DicomMask *maskToQuery, QList<PacsDevice> pacsToQuery)
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

void QInputOutputPacsWidget::queryStudyResultsReceived(QList<Patient*> patients, QHash<QString, QString> hashTablePacsIDOfStudyInstanceUID)
{
    m_studyTreeWidget->insertPatientList(patients);
    m_hashPacsIDOfStudyInstanceUID = m_hashPacsIDOfStudyInstanceUID.unite(hashTablePacsIDOfStudyInstanceUID); 
}

void QInputOutputPacsWidget::querySeriesResultsReceived(QString studyInstanceUID, QList<Series*> series)
{
    if (series.isEmpty())
    {
        QMessageBox::information(this, ApplicationNameString, tr("No series match for this study %1.\n").arg(studyInstanceUID));
    }
    else m_studyTreeWidget->insertSeriesList(studyInstanceUID, series);

}

void QInputOutputPacsWidget::queryImageResultsReceived(QString studyInstanceUID, QString seriesInstanceUID, QList<Image*> images)
{
    if (images.isEmpty())
    {
        QMessageBox::information(this, ApplicationNameString, tr("No images match series %1.\n").arg(seriesInstanceUID));
    }
    else m_studyTreeWidget->insertImageList(studyInstanceUID, seriesInstanceUID, images);
}

void QInputOutputPacsWidget::queryFinished()
{
    setQueryInProgress(false);
}

void QInputOutputPacsWidget::errorQueryingStudy(PacsDevice pacsDeviceError)
{
    QString errorMessage;

    errorMessage = tr("%1 can't query to PACS %2 from %3.\nBe sure that your computer is connected on network and the PACS parameters are correct.")
        .arg(ApplicationNameString)
        .arg(pacsDeviceError.getAETitle())
        .arg(pacsDeviceError.getInstitution());

    QMessageBox::critical(this, ApplicationNameString, errorMessage);
}

void QInputOutputPacsWidget::errorQueryingSeries(QString studyInstanceUID, PacsDevice pacsDeviceError)
{
    QString errorMessage = tr("%1 can't query series from study %2 to PACS %3 from %4.\n").arg(ApplicationNameString, studyInstanceUID, pacsDeviceError.getAETitle(), pacsDeviceError.getInstitution());
    errorMessage += tr("Be sure that your computer is connected on network and the PACS parameters are correct.");

    QMessageBox::warning(this, ApplicationNameString, errorMessage);
}

void QInputOutputPacsWidget::errorQueryingImage(QString studyInstanceUID, QString seriesInstanceUID, PacsDevice pacsDeviceError)
{
    QString errorMessage = tr("%1 can't query images from series %2 to PACS %3 from %4.\n").arg(ApplicationNameString, seriesInstanceUID, pacsDeviceError.getAETitle(), pacsDeviceError.getInstitution());
    errorMessage += tr("Be sure that your computer is connected on network and the PACS parameters are correct.");

    QMessageBox::warning(this, ApplicationNameString, errorMessage);
}

void QInputOutputPacsWidget::showQExecuteOperationThreadError(QString studyInstanceUID, QString pacsID, QExecuteOperationThread::OperationError error)
{
    QString message;
    PacsDevice pacs = PacsDeviceManager().getPACSDeviceByID(pacsID);

    /*TODO:S'ha de millorar els missatges d'error indicant quin estudi ha fallat amb nom de pacient i study ID, s'ha de fer que l'error emeti
     * en comptes del studyInstanceUID l'objecte Operation que conté la informació el patientName i el studyID */

    switch (error)
    {
        case QExecuteOperationThread::ErrorConnectingPacs :
            message = tr("Please review the operation list screen, ");
            message += tr("%1 can't connect to PACS %2 trying to retrieve or store a study.\n").arg(ApplicationNameString, pacs.getAETitle());
            message += tr("\nBe sure that your computer is connected on network and the PACS parameters are correct.");
            message += tr("\nIf the problem persist contact with an administrator.");
            QMessageBox::critical(this, ApplicationNameString, message);
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
        case QExecuteOperationThread::MoveDestinationAETileUnknownStatus:
            message = tr("Please review the operation list screen, ");
            message += tr("PACS %1 doesn't recognize your computer's AETitle %2 and some studies can't be retrieved.").arg(pacs.getAETitle(), PacsDevice::getLocalAETitle() );
            message += tr("\n\nContact with an administrador to register your computer to the PACS.");
            QMessageBox::warning(this, ApplicationNameString, message);
            break;
        case QExecuteOperationThread::MoveUnknowStatus :
        case QExecuteOperationThread::MoveFailureOrRefusedStatus :
            message = tr("Please review the operation list screen, ");
            message += tr("PACS %1 doesn't respond as expected and %2 can't process the request for retrieving images.\n\n").arg(pacs.getAETitle(), ApplicationNameString);
            message += tr("The cause of the error can be that the requested images are corrupted or the incoming connections port in PACS configuration is not correct.");
            QMessageBox::critical(this, ApplicationNameString, message);
            break;
        case QExecuteOperationThread::IncomingConnectionsPortPacsInUse :
            message = tr("Port %1 for incoming connections from PACS is already in use by another application.").arg( PacsDevice::getQueryRetrievePort() );
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

void QInputOutputPacsWidget::showQExecuteOperationThreadWarning(QString studyInstanceUID, QString pacsID, QExecuteOperationThread::OperationWarning warning)
{
    QString message;
    PacsDevice pacs = PacsDeviceManager().getPACSDeviceByID(pacsID);

    /*TODO:S'ha de millorar els missatges d'error indicant quin estudi ha fallat amb nom de pacient i study ID, s'ha de fer que l'error emeti
     * en comptes del studyInstanceUID l'objecte Operation que conté la informació el patientName i el studyID */

    switch (warning)
    {
        case QExecuteOperationThread::MoveWarningStatus :
            message = tr("Some images of study %1 from PACS %2 can't be retrieved because may be corrupted.\n").arg(studyInstanceUID, pacs.getAETitle());
            message += tr("The study is incomplet.");
            QMessageBox::warning(this, ApplicationNameString, message);
            break;
        default:
            message = tr("Some images of study %1 from PACS %2.\n").arg(studyInstanceUID, pacs.getAETitle());
            message += tr("The study is incomplet.");
            QMessageBox::warning(this, ApplicationNameString, message);
    }
}

void QInputOutputPacsWidget::setQueryInProgress(bool queryInProgress)
{
    m_queryAnimationLabel->setVisible(queryInProgress);
    m_queryInProgressLabel->setVisible(queryInProgress);
    m_cancelQueryButton->setEnabled(queryInProgress);
}
};
