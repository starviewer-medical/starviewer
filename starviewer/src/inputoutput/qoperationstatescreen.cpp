/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qoperationstatescreen.h"

#include <QCloseEvent>
#include <QMessageBox>

#include "inputoutputsettings.h"
#include "pacsmanager.h"
#include "pacsjob.h"
#include "senddicomfilestopacsjob.h"
#include "retrievedicomfilesfrompacsjob.h"
#include "study.h"
#include "starviewerapplication.h"

namespace udg {

QOperationStateScreen::QOperationStateScreen( QWidget *parent )
 : QDialog( parent )
{
    setupUi( this );
    setWindowFlags( (this->windowFlags() | Qt::WindowMaximizeButtonHint)  ^ Qt::WindowContextHelpButtonHint  );

    createConnections();

    Settings settings;
    settings.restoreColumnsWidths(InputOutputSettings::OperationStateColumnsWidth, m_treeRetrieveStudy);
    m_treeRetrieveStudy->setColumnHidden( 9 , true );//Conte el PACSJobID 
    m_treeRetrieveStudy->sortItems(5, Qt::AscendingOrder);//Fem que per defecte ordeni per la data d'inici de la descàrrega
}

void QOperationStateScreen::setPacsManager(PacsManager *pacsManager)
{
    m_pacsManager = pacsManager;

    connect(m_pacsManager, SIGNAL(newPACSJobEnqueued(PACSJob*)), SLOT(newPACSJobEnqueued(PACSJob*)));
    connect(m_pacsManager, SIGNAL(requestedCancelPACSJob(PACSJob*)), SLOT(requestedCancelPACSJob(PACSJob*)));
}

void QOperationStateScreen::createConnections()
{
    connect(m_buttonClear, SIGNAL(clicked()), this, SLOT(clearList()));
    connect(m_cancellAllRequestsButton, SIGNAL(clicked()), this, SLOT(cancelAllRequests()));
    connect(m_cancelSelectedRequestsButton, SIGNAL(clicked()), this, SLOT(cancelSelectedRequests()));
}

void QOperationStateScreen::newPACSJobEnqueued(PACSJob *pacsJob)
{
    m_PACSJobPendingToFinish.insert(pacsJob->getPACSJobID(), pacsJob);

    connect(pacsJob, SIGNAL(PACSJobStarted(PACSJob*)), SLOT(PACSJobStarted(PACSJob*))); 
    connect(pacsJob, SIGNAL(PACSJobFinished(PACSJob*)), SLOT(PACSJobFinished(PACSJob*))); 
    connect(pacsJob, SIGNAL(PACSJobCancelled(PACSJob*)), SLOT(PACSJobCancelled(PACSJob*)));

    switch(pacsJob->getPACSJobType())
    {
        case PACSJob::SendDICOMFilesToPACSJobType:
            insertNewPACSJob(pacsJob);
            connect(dynamic_cast<SendDICOMFilesToPACSJob*> ( pacsJob ), SIGNAL(DICOMFileSent(PACSJob*, int)), SLOT(DICOMFileCommit(PACSJob *, int )));
            connect(dynamic_cast<SendDICOMFilesToPACSJob*> ( pacsJob ), SIGNAL(DICOMSeriesSent(PACSJob*, int)), SLOT(DICOMSeriesCommit(PACSJob *, int )));
            break;
        case PACSJob::RetrieveDICOMFilesFromPACSJobType:
            insertNewPACSJob(pacsJob);
            connect(dynamic_cast<RetrieveDICOMFilesFromPACSJob*> ( pacsJob ), SIGNAL(DICOMFileRetrieved(PACSJob*, int)), SLOT(DICOMFileCommit(PACSJob *, int )));
            connect(dynamic_cast<RetrieveDICOMFilesFromPACSJob*> ( pacsJob ), SIGNAL(DICOMSeriesRetrieved(PACSJob*, int)), SLOT(DICOMSeriesCommit(PACSJob *, int )));
            break;
        default:
            break;
    }
}

void QOperationStateScreen::PACSJobStarted(PACSJob *pacsJob)
{
    QTreeWidgetItem* qtreeWidgetItem = getQTreeWidgetItemByPACSJobId(pacsJob->getPACSJobID());

    if  (qtreeWidgetItem != NULL)
    {
        qtreeWidgetItem->setText(0, pacsJob->getPACSJobType() == PACSJob::RetrieveDICOMFilesFromPACSJobType ? tr("RETRIEVING") : tr("SENDING") );
    }
}

void QOperationStateScreen::PACSJobFinished(PACSJob *pacsJob)
{
    QTreeWidgetItem* qtreeWidgetItem = getQTreeWidgetItemByPACSJobId(pacsJob->getPACSJobID());

    if  (qtreeWidgetItem != NULL)
    {
        qtreeWidgetItem->setText(0, getPACSJobStatusResume(pacsJob));
    }

    m_PACSJobPendingToFinish.remove(pacsJob->getPACSJobID());
}

void QOperationStateScreen::PACSJobCancelled(PACSJob *pacsJob)
{
    QTreeWidgetItem* qtreeWidgetItem = getQTreeWidgetItemByPACSJobId(pacsJob->getPACSJobID());

    if  (qtreeWidgetItem != NULL)
    {
        qtreeWidgetItem->setText(0, tr("CANCELLED"));
    }

    m_PACSJobPendingToFinish.remove(pacsJob->getPACSJobID());
}

void QOperationStateScreen::DICOMFileCommit(PACSJob *pacsJob, int numberOfImages)
{
    QTreeWidgetItem* qtreeWidgetItem = getQTreeWidgetItemByPACSJobId(pacsJob->getPACSJobID());

    if  (qtreeWidgetItem != NULL)
    {
        qtreeWidgetItem->setText(8, QString().setNum(numberOfImages));
    }
}

void QOperationStateScreen::DICOMSeriesCommit(PACSJob *pacsJob, int numberOfSeries)
{
    QTreeWidgetItem* qtreeWidgetItem = getQTreeWidgetItemByPACSJobId(pacsJob->getPACSJobID());

    if  (qtreeWidgetItem != NULL)
    {
        qtreeWidgetItem->setText(7, QString().setNum(numberOfSeries));
    }
}

void QOperationStateScreen::clearList()
{
    // seleccionem els elements que volem esborrar
    QList<QTreeWidgetItem *> clearableItems;
    clearableItems = m_treeRetrieveStudy->findItems( tr("RETRIEVED"), Qt::MatchExactly, 0 );
    clearableItems += m_treeRetrieveStudy->findItems( tr("STORED"), Qt::MatchExactly, 0 );
    clearableItems += m_treeRetrieveStudy->findItems( tr("ERROR"), Qt::MatchExactly, 0 );
    clearableItems += m_treeRetrieveStudy->findItems( tr("CANCELLED"), Qt::MatchExactly, 0 );
    // els eliminem de la llista
    foreach( QTreeWidgetItem *itemToClear, clearableItems )
    {
        m_treeRetrieveStudy->invisibleRootItem()->takeChild( m_treeRetrieveStudy->invisibleRootItem()->indexOfChild(itemToClear) );
    }
}

void QOperationStateScreen::cancelAllRequests()
{
    foreach(PACSJob *pacsJob, m_PACSJobPendingToFinish.values())
    {
        m_pacsManager->requestCancelPACSJob(pacsJob);
    }
}

void QOperationStateScreen::cancelSelectedRequests()
{
    if (m_treeRetrieveStudy->selectedItems().count() > 0)
    {
        foreach(QTreeWidgetItem *item, m_treeRetrieveStudy->selectedItems())
        {
            if (m_PACSJobPendingToFinish.contains(item->text(9).toInt()))
            {
                m_pacsManager->requestCancelPACSJob(m_PACSJobPendingToFinish[item->text(9).toInt()]);
            }
        }
    }
    else
    {
        QMessageBox::information(this,ApplicationNameString, tr("You have to select at least one operation to cancel."));
    }
}

void QOperationStateScreen::requestedCancelPACSJob(PACSJob *pacsJob)
{
    QTreeWidgetItem* qtreeWidgetItem = getQTreeWidgetItemByPACSJobId(pacsJob->getPACSJobID());

    if  (qtreeWidgetItem != NULL)
    {
        qtreeWidgetItem->setText(0, tr("CANCELLING"));
    }
}

void QOperationStateScreen::insertNewPACSJob(PACSJob *pacsJob)
{
    Q_ASSERT(pacsJob->getPACSJobType() == PACSJob::SendDICOMFilesToPACSJobType || pacsJob->getPACSJobType() == PACSJob::RetrieveDICOMFilesFromPACSJobType);

    QTreeWidgetItem* item = new QTreeWidgetItem();
    Study *study = getStudyFromPACSJob(pacsJob);

    item->setText(0 , tr("PENDING") );
    item->setText(1, pacsJob->getPACSJobType() == PACSJob::SendDICOMFilesToPACSJobType ? tr("Server") : tr("Local"));
    item->setText(2 , pacsJob->getPacsDevice().getAETitle());
    item->setText(3 , study->getParentPatient()->getID());
    item->setText(4 , study->getParentPatient()->getFullName());
    item->setText(5 , QDate::currentDate().toString("dd/MM/yyyy"));
    item->setText(6 , QTime::currentTime().toString("hh:mm"));
    item->setText(7 , "0"); // series
    item->setText(8 , "0"); //imatges
    item->setText(9 , QString().setNum(pacsJob->getPACSJobID()));

    m_treeRetrieveStudy->addTopLevelItem(item);
}


Study* QOperationStateScreen::getStudyFromPACSJob(PACSJob *pacsJob)
{
    Study *study = NULL;

    if (pacsJob->getPACSJobType() == PACSJob::SendDICOMFilesToPACSJobType)
    {
        study = (dynamic_cast<SendDICOMFilesToPACSJob*> ( pacsJob ))->getStudyOfImagesToSend();
    }
    else if (pacsJob->getPACSJobType() == PACSJob::RetrieveDICOMFilesFromPACSJobType)
    {
        study = (dynamic_cast<RetrieveDICOMFilesFromPACSJob*> ( pacsJob ))->getStudyToRetrieveDICOMFiles();
    }

    return study;
}

QString QOperationStateScreen::getPACSJobStatusResume(PACSJob *pacsJob)
{
    if (pacsJob->getPACSJobType() == PACSJob::RetrieveDICOMFilesFromPACSJobType)
    {
        switch((dynamic_cast<RetrieveDICOMFilesFromPACSJob*> ( pacsJob ))->getStatus())
        {
            case PACSRequestStatus::OkRetrieve:
                return tr("RETRIEVED");
                break;
            case PACSRequestStatus::RetrieveCancelled:
                return tr("CANCELLED");
                break;
            default:
                return tr("ERROR");
                break;
        }
    }
    else
    {
        if ((dynamic_cast<SendDICOMFilesToPACSJob*> ( pacsJob ))->getStatus() == PACSRequestStatus::OkSend)
        {
            return tr("SENT");
        }
        else
        {
            return tr("ERROR");
        }
    }
}

void QOperationStateScreen::closeEvent( QCloseEvent* ce )
{
    Settings settings;
    settings.saveColumnsWidths(InputOutputSettings::OperationStateColumnsWidth, m_treeRetrieveStudy);
    ce->accept();
}

QTreeWidgetItem* QOperationStateScreen::getQTreeWidgetItemByPACSJobId(int pacsJobID)
{
    QTreeWidgetItem *qtreeWidgetItem = NULL;
    QList<QTreeWidgetItem *> qTreeWidgetPacsJobItems = m_treeRetrieveStudy->findItems(QString().setNum(pacsJobID), Qt::MatchExactly, 9);

    if  (!qTreeWidgetPacsJobItems.isEmpty())
    {
        qtreeWidgetItem = qTreeWidgetPacsJobItems.at(0);
    }

    return qtreeWidgetItem;
}

};
