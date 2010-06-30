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
#include "localdatabasemanager.h"
#include "pacsmanager.h"
#include "harddiskinformation.h"
#include "retrievedicomfilesfrompacsjob.h"

namespace udg
{

QInputOutputPacsWidget::QInputOutputPacsWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);

    createContextMenuQStudyTreeWidget();

    Settings settings;
    settings.restoreColumnsWidths( InputOutputSettings::PACSStudyListColumnsWidth, m_studyTreeWidget->getQTreeWidget() );

    m_statsWatcher = new StatsWatcher("QueryInputOutputPacsWidget",this);
    m_statsWatcher->addClicksCounter(m_retrievAndViewButton);
    m_statsWatcher->addClicksCounter(m_retrieveButton);

    //Preparem el QMovie per indicar quan s'estan fent consultes al PACS
    QMovie *operationAnimation = new QMovie(this);
    operationAnimation->setFileName(":/images/loader.gif");
    m_queryAnimationLabel->setMovie(operationAnimation);
    operationAnimation->start();

    setQueryInProgress(false);

    //Indiquem que el QStudyTreeWidget inicialment s'ordenarà pel la columna name
    m_studyTreeWidget->setSortColumn(QStudyTreeWidget::ObjectName);

    createConnections();
}

QInputOutputPacsWidget::~QInputOutputPacsWidget()
{
    Settings settings;
    settings.saveColumnsWidths( InputOutputSettings::PACSStudyListColumnsWidth, m_studyTreeWidget->getQTreeWidget() );
}

void QInputOutputPacsWidget::createConnections()
{
    connect(m_studyTreeWidget, SIGNAL(studyExpanded(QString)), SLOT(expandSeriesOfStudy(QString)));
    connect(m_studyTreeWidget, SIGNAL(seriesExpanded(QString, QString)), SLOT(expandImagesOfSeries(QString, QString)));

    connect(m_studyTreeWidget, SIGNAL(studyDoubleClicked()), SLOT(retrieveSelectedStudies()));
    connect(m_studyTreeWidget, SIGNAL(seriesDoubleClicked()), SLOT(retrieveSelectedStudies()));
    connect(m_studyTreeWidget, SIGNAL(imageDoubleClicked()), SLOT(retrieveSelectedStudies()));

    connect(m_retrievAndViewButton, SIGNAL(clicked()), SLOT(retrieveAndViewSelectedStudies()));
    connect(m_retrieveButton, SIGNAL(clicked()), SLOT(retrieveSelectedStudies()));

    connect(m_cancelQueryButton, SIGNAL(clicked()), SLOT(cancelCurrentQueries()));
}

void  QInputOutputPacsWidget::createContextMenuQStudyTreeWidget()
{
    QAction *action;

    action = m_contextMenuQStudyTreeWidget.addAction(QIcon(":/images/retrieveAndView.png"), tr("Retrieve && &View"), this, SLOT(retrieveAndViewSelectedStudies()), tr("Ctrl+V"));
    (void) new QShortcut(action->shortcut(), this, SLOT(retrieveAndViewSelectedStudies()));

    action = m_contextMenuQStudyTreeWidget.addAction(QIcon(":/images/retrieve.png"), tr("&Retrieve"), this, SLOT(retrieveSelectedStudies()), tr("Ctrl+R"));
    (void) new QShortcut(action->shortcut(), this, SLOT(retrieveSelectedStudies()));

    m_studyTreeWidget->setContextMenu(& m_contextMenuQStudyTreeWidget); //Especifiquem que es el menu del dicomdir
}

void QInputOutputPacsWidget::setPacsManager(PacsManager *pacsManager)
{
    m_pacsManager = pacsManager;

    connect(m_pacsManager, SIGNAL(queryStudyResultsReceived(QList<Patient*>, QHash<QString, QString>)), SLOT(queryStudyResultsReceived(QList<Patient*>, QHash<QString, QString>)));
    connect(m_pacsManager, SIGNAL(querySeriesResultsReceived(QString, QList<Series*>)), SLOT(querySeriesResultsReceived(QString , QList<Series*>)));
    connect(m_pacsManager, SIGNAL(queryImageResultsReceived(QString, QString, QList<Image*>)), SLOT(queryImageResultsReceived(QString , QString ,QList<Image*>)));

    connect(m_pacsManager, SIGNAL(queryFinished()), SLOT(queryFinished()));

    connect(m_pacsManager, SIGNAL(errorQueryingStudy(PacsDevice)), SLOT(errorQueryingStudy(PacsDevice)));
    connect(m_pacsManager, SIGNAL(errorQueryingSeries(QString, PacsDevice)), SLOT(errorQueryingSeries(QString, PacsDevice)));
    connect(m_pacsManager, SIGNAL(errorQueryingImage(QString, QString, PacsDevice)), SLOT(errorQueryingImage(QString, QString, PacsDevice)));
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

void QInputOutputPacsWidget::clear()
{
    m_studyTreeWidget->clear();
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

void QInputOutputPacsWidget::retrieveSelectedStudies()
{
    if(m_studyTreeWidget->getSelectedStudiesUID().isEmpty())
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, ApplicationNameString, tr("Select a study to retrieve."));
        return;
    }

    foreach(DicomMask dicomMask, m_studyTreeWidget->getDicomMaskOfSelectedItems())
    {
        retrieve(getPacsIDFromQueriedStudies(dicomMask.getStudyInstanceUID()), m_studyTreeWidget->getStudy(dicomMask.getStudyInstanceUID()), dicomMask, None);
    }
}

void QInputOutputPacsWidget::retrieveAndViewSelectedStudies()
{
    if(m_studyTreeWidget->getSelectedStudiesUID().isEmpty())
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, ApplicationNameString, tr("Select a study to retrieve and view."));
        return;
    }

    foreach(DicomMask dicomMask, m_studyTreeWidget->getDicomMaskOfSelectedItems())
    {
        retrieve(getPacsIDFromQueriedStudies(dicomMask.getStudyInstanceUID()), m_studyTreeWidget->getStudy(dicomMask.getStudyInstanceUID()), dicomMask, View);
    }
}

void QInputOutputPacsWidget::cancelCurrentQueries()
{
    m_pacsManager->cancelCurrentQueries();
    setQueryInProgress(false);
}

void QInputOutputPacsWidget::retrieveDICOMFilesFromPACSJobStarted(PACSJob *pacsJob)
{
    emit studyRetrieveStarted((dynamic_cast<RetrieveDICOMFilesFromPACSJob*> (pacsJob))->getStudyToRetrieveDICOMFiles()->getInstanceUID());
}

void QInputOutputPacsWidget::retrieveDICOMFilesFromPACSJobFinished(PACSJob *pacsJob)
{
    RetrieveDICOMFilesFromPACSJob *retrieveDICOMFilesFromPACSJob = dynamic_cast<RetrieveDICOMFilesFromPACSJob*> (pacsJob);

    if (retrieveDICOMFilesFromPACSJob->getStatus() != PACSRequestStatus::OkRetrieve)
    {
        if (retrieveDICOMFilesFromPACSJob->getStatus() == PACSRequestStatus::RetrieveWarning)
        {
            QMessageBox::warning(this, ApplicationNameString, retrieveDICOMFilesFromPACSJob->getStatusDescription());
        }
        else
        {
            QMessageBox::critical(this, ApplicationNameString, retrieveDICOMFilesFromPACSJob->getStatusDescription());
            emit studyRetrieveFailed(retrieveDICOMFilesFromPACSJob->getStudyToRetrieveDICOMFiles()->getInstanceUID());
            return;
        }
    }

    emit studyRetrieveFinished(retrieveDICOMFilesFromPACSJob->getStudyToRetrieveDICOMFiles()->getInstanceUID());

    switch(m_actionsWhenRetrieveJobFinished.take(retrieveDICOMFilesFromPACSJob->getPACSJobID()))
    {
        case Load:
            emit loadRetrievedStudy(retrieveDICOMFilesFromPACSJob->getStudyToRetrieveDICOMFiles()->getInstanceUID());;
            break;
        case View:
            emit viewRetrievedStudy(retrieveDICOMFilesFromPACSJob->getStudyToRetrieveDICOMFiles()->getInstanceUID());
            break;
        default:
            break;
    }

    /*Com que l'objecte és un punter altres classes poden haver capturat el Signal per això li fem un deleteLater() en comptes d'un delete, per evitar
      que quan responguin al signal es trobin que l'objecte ja no existeix. L'objecte serà destruït per Qt quan es retorni el eventLoop*/
    pacsJob->deleteLater();
}

void QInputOutputPacsWidget::retrieve(QString pacsIDToRetrieve, Study *studyToRetrieve, DicomMask maskStudyToRetrieve, ActionsAfterRetrieve actionsAfterRetrieve)
{
    PacsDevice pacsDevice = PacsDeviceManager().getPACSDeviceByID(pacsIDToRetrieve);
    RetrieveDICOMFilesFromPACSJob::RetrievePriorityJob retrievePriorityJob = actionsAfterRetrieve == View ? RetrieveDICOMFilesFromPACSJob::High : RetrieveDICOMFilesFromPACSJob::Medium;

    RetrieveDICOMFilesFromPACSJob *retrieveDICOMFilesFromPACSJob = new RetrieveDICOMFilesFromPACSJob(pacsDevice, studyToRetrieve , maskStudyToRetrieve, 
        retrievePriorityJob);

    m_pacsManager->enqueuePACSJob(retrieveDICOMFilesFromPACSJob);
    connect(retrieveDICOMFilesFromPACSJob, SIGNAL(PACSJobStarted(PACSJob*)), SLOT(retrieveDICOMFilesFromPACSJobStarted(PACSJob*)));
    connect(retrieveDICOMFilesFromPACSJob, SIGNAL(PACSJobFinished(PACSJob*)), SLOT(retrieveDICOMFilesFromPACSJobFinished(PACSJob*)));

    m_actionsWhenRetrieveJobFinished.insert(retrieveDICOMFilesFromPACSJob->getPACSJobID(), actionsAfterRetrieve);
}

bool QInputOutputPacsWidget::AreValidQueryParameters(DicomMask *maskToQuery, QList<PacsDevice> pacsToQuery)
{
    if (pacsToQuery.isEmpty()) //es comprova que hi hagi pacs seleccionats
    {
        QMessageBox::warning(this, ApplicationNameString, tr("Please select a PACS to query."));
        return false;
    }

    if( maskToQuery->isEmpty() )
    {
        QMessageBox::StandardButton response;
        response = QMessageBox::question(this, ApplicationNameString, tr("No search fields were filled.") + "\n" + tr("The query can take a long time.\nDo you want continue?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        return (response == QMessageBox::Yes);
    }
    else
        return true;
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

    mask.setStudyInstanceUID(studyInstanceUID);
    mask.setSeriesDate("");
    mask.setSeriesTime("");
    mask.setSeriesModality("");
    mask.setSeriesNumber("");
    mask.setSeriesInstanceUID("");
    mask.setPPSStartDate("");
    mask.setPPStartTime("");
    mask.setRequestAttributeSequence("", "");

    return mask;
}

DicomMask QInputOutputPacsWidget::buildImageDicomMask(QString studyInstanceUID, QString seriesInstanceUID)
{
    DicomMask mask;

    mask.setStudyInstanceUID(studyInstanceUID);
    mask.setSeriesInstanceUID(seriesInstanceUID);
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

void QInputOutputPacsWidget::setQueryInProgress(bool queryInProgress)
{
    m_queryAnimationLabel->setVisible(queryInProgress);
    m_queryInProgressLabel->setVisible(queryInProgress);
    m_cancelQueryButton->setEnabled(queryInProgress);
}
};
