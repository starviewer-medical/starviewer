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

QOperationStateScreen::QOperationStateScreen(QWidget *parent)
 : QDialog(parent)
{
    setupUi(this);
    setWindowFlags((this->windowFlags() | Qt::WindowMaximizeButtonHint) ^ Qt::WindowContextHelpButtonHint);

    createConnections();

    Settings settings;
    settings.restoreColumnsWidths(InputOutputSettings::OperationStateColumnsWidth, m_treeRetrieveStudy);

    QOperationStateScreen::ColumnIndex sortByColumn = (QOperationStateScreen::ColumnIndex)
        settings.getValue(InputOutputSettings::OperationStateListSortByColumn).toInt();

    Qt::SortOrder sortOrderColumn = (Qt::SortOrder) settings.getValue(InputOutputSettings::OperationStateListSortOrder).toInt();
    m_treeRetrieveStudy->sortItems(sortByColumn, sortOrderColumn);

    // Conte el PACSJobID
    m_treeRetrieveStudy->setColumnHidden(QOperationStateScreen::PACSJobID, true);
    // Aquesta columna mostrava en quina hora s'havia sol·licitat l'estudi, ara l'hora es fusiona conjuntament amb la data per això amaguem aquesta columna
    // que no la podrem eliminar fins que s'hagi resolt el ticket #1401
    m_treeRetrieveStudy->setColumnHidden(QOperationStateScreen::Started, true);
}

QOperationStateScreen::~QOperationStateScreen()
{
    Settings settings;

    // Guardem per quin columna està ordenada la llista d'estudis i en quin ordre
    settings.setValue(InputOutputSettings::OperationStateListSortByColumn, m_treeRetrieveStudy->header()->sortIndicatorSection());
    settings.setValue(InputOutputSettings::OperationStateListSortOrder, m_treeRetrieveStudy->header()->sortIndicatorOrder());
}

void QOperationStateScreen::setPacsManager(PacsManager *pacsManager)
{
    m_pacsManager = pacsManager;

    connect(m_pacsManager, SIGNAL(newPACSJobEnqueued(PACSJobPointer)), SLOT(newPACSJobEnqueued(PACSJobPointer)));
    connect(m_pacsManager, SIGNAL(requestedCancelPACSJob(PACSJobPointer)), SLOT(requestedCancelPACSJob(PACSJobPointer)));
}

void QOperationStateScreen::createConnections()
{
    connect(m_buttonClear, SIGNAL(clicked()), this, SLOT(clearList()));
    connect(m_cancellAllRequestsButton, SIGNAL(clicked()), this, SLOT(cancelAllRequests()));
    connect(m_cancelSelectedRequestsButton, SIGNAL(clicked()), this, SLOT(cancelSelectedRequests()));
}

void QOperationStateScreen::newPACSJobEnqueued(PACSJobPointer pacsJob)
{
    // Els altres tipus de PACSJob no es interessen
    if (pacsJob->getPACSJobType() == PACSJob::SendDICOMFilesToPACSJobType || pacsJob->getPACSJobType() == PACSJob::RetrieveDICOMFilesFromPACSJobType)
    {
        m_PACSJobPendingToFinish.insert(pacsJob->getPACSJobID(), pacsJob);

        connect(pacsJob.data(), SIGNAL(PACSJobStarted(PACSJobPointer)), SLOT(PACSJobStarted(PACSJobPointer)));
        connect(pacsJob.data(), SIGNAL(PACSJobFinished(PACSJobPointer)), SLOT(PACSJobFinished(PACSJobPointer)));
        connect(pacsJob.data(), SIGNAL(PACSJobCancelled(PACSJobPointer)), SLOT(PACSJobCancelled(PACSJobPointer)));

        switch (pacsJob->getPACSJobType())
        {
            case PACSJob::SendDICOMFilesToPACSJobType:
                insertNewPACSJob(pacsJob);
                connect(pacsJob.objectCast<SendDICOMFilesToPACSJob>().data(), SIGNAL(DICOMFileSent(PACSJobPointer, int)),
                        SLOT(DICOMFileCommit(PACSJobPointer, int)));
                connect(pacsJob.objectCast<SendDICOMFilesToPACSJob>().data(), SIGNAL(DICOMSeriesSent(PACSJobPointer, int)),
                        SLOT(DICOMSeriesCommit(PACSJobPointer, int)));
                break;
            case PACSJob::RetrieveDICOMFilesFromPACSJobType:
                insertNewPACSJob(pacsJob);
                connect(pacsJob.objectCast<RetrieveDICOMFilesFromPACSJob>().data(), SIGNAL(DICOMFileRetrieved(PACSJobPointer, int)),
                        SLOT(DICOMFileCommit(PACSJobPointer, int)));
                connect(pacsJob.objectCast<RetrieveDICOMFilesFromPACSJob>().data(), SIGNAL(DICOMSeriesRetrieved(PACSJobPointer, int)),
                        SLOT(DICOMSeriesCommit(PACSJobPointer, int)));
                break;
            default:
                break;
        }
    }
}

void QOperationStateScreen::PACSJobStarted(PACSJobPointer pacsJob)
{
    QTreeWidgetItem *qtreeWidgetItem = getQTreeWidgetItemByPACSJobId(pacsJob->getPACSJobID());

    if (qtreeWidgetItem != NULL)
    {
        qtreeWidgetItem->setText(QOperationStateScreen::Status, pacsJob->getPACSJobType() == PACSJob::RetrieveDICOMFilesFromPACSJobType ? tr("RETRIEVING")
            : tr("SENDING"));
    }
}

void QOperationStateScreen::PACSJobFinished(PACSJobPointer pacsJob)
{
    QTreeWidgetItem *qtreeWidgetItem = getQTreeWidgetItemByPACSJobId(pacsJob->getPACSJobID());

    if (qtreeWidgetItem != NULL)
    {
        qtreeWidgetItem->setText(QOperationStateScreen::Status, getPACSJobStatusResume(pacsJob));
    }

    m_PACSJobPendingToFinish.remove(pacsJob->getPACSJobID());
}

void QOperationStateScreen::PACSJobCancelled(PACSJobPointer pacsJob)
{
    QTreeWidgetItem *qtreeWidgetItem = getQTreeWidgetItemByPACSJobId(pacsJob->getPACSJobID());

    if (qtreeWidgetItem != NULL)
    {
        qtreeWidgetItem->setText(QOperationStateScreen::Status, tr("CANCELLED"));
    }

    m_PACSJobPendingToFinish.remove(pacsJob->getPACSJobID());
}

void QOperationStateScreen::DICOMFileCommit(PACSJobPointer pacsJob, int numberOfImages)
{
    QTreeWidgetItem *qtreeWidgetItem = getQTreeWidgetItemByPACSJobId(pacsJob->getPACSJobID());

    if (qtreeWidgetItem != NULL)
    {
        qtreeWidgetItem->setText(QOperationStateScreen::Files, QString().setNum(numberOfImages));
    }
}

void QOperationStateScreen::DICOMSeriesCommit(PACSJobPointer pacsJob, int numberOfSeries)
{
    QTreeWidgetItem *qtreeWidgetItem = getQTreeWidgetItemByPACSJobId(pacsJob->getPACSJobID());

    if (qtreeWidgetItem != NULL)
    {
        qtreeWidgetItem->setText(QOperationStateScreen::Series, QString().setNum(numberOfSeries));
    }
}

void QOperationStateScreen::clearList()
{
    // Seleccionem els elements que volem esborrar
    QList<QTreeWidgetItem*> clearableItems;
    clearableItems = m_treeRetrieveStudy->findItems(tr("RETRIEVED"), Qt::MatchExactly, QOperationStateScreen::Status);
    clearableItems += m_treeRetrieveStudy->findItems(tr("SENT"), Qt::MatchExactly, QOperationStateScreen::Status);
    clearableItems += m_treeRetrieveStudy->findItems(tr("ERROR"), Qt::MatchExactly, QOperationStateScreen::Status);
    clearableItems += m_treeRetrieveStudy->findItems(tr("CANCELLED"), Qt::MatchExactly, QOperationStateScreen::Status);
    // Els eliminem de la llista
    foreach (QTreeWidgetItem *itemToClear, clearableItems)
    {
        m_treeRetrieveStudy->invisibleRootItem()->takeChild(m_treeRetrieveStudy->invisibleRootItem()->indexOfChild(itemToClear));
    }
}

void QOperationStateScreen::cancelAllRequests()
{
    foreach (PACSJobPointer pacsJob, m_PACSJobPendingToFinish.values())
    {
        m_pacsManager->requestCancelPACSJob(pacsJob);
    }
}

void QOperationStateScreen::cancelSelectedRequests()
{
    if (m_treeRetrieveStudy->selectedItems().count() > 0)
    {
        foreach (QTreeWidgetItem *item, m_treeRetrieveStudy->selectedItems())
        {
            if (m_PACSJobPendingToFinish.contains(item->text(QOperationStateScreen::PACSJobID).toInt()))
            {
                m_pacsManager->requestCancelPACSJob(m_PACSJobPendingToFinish[item->text(QOperationStateScreen::PACSJobID).toInt()]);
            }
        }
    }
    else
    {
        QMessageBox::information(this, ApplicationNameString, tr("You have to select at least one operation to cancel."));
    }
}

void QOperationStateScreen::requestedCancelPACSJob(PACSJobPointer pacsJob)
{
    QTreeWidgetItem *qtreeWidgetItem = getQTreeWidgetItemByPACSJobId(pacsJob->getPACSJobID());

    if (qtreeWidgetItem != NULL)
    {
        qtreeWidgetItem->setText(QOperationStateScreen::Status, tr("CANCELLING"));
    }
}

void QOperationStateScreen::insertNewPACSJob(PACSJobPointer pacsJob)
{
    Q_ASSERT(pacsJob->getPACSJobType() == PACSJob::SendDICOMFilesToPACSJobType || pacsJob->getPACSJobType() == PACSJob::RetrieveDICOMFilesFromPACSJobType);

    QTreeWidgetItem *item = new QTreeWidgetItem();
    Study *study = getStudyFromPACSJob(pacsJob);

    item->setText(QOperationStateScreen::Status, tr("PENDING"));
    item->setText(QOperationStateScreen::Direction, pacsJob->getPACSJobType() == PACSJob::SendDICOMFilesToPACSJobType ? tr("Server") : tr("Local"));
    item->setText(QOperationStateScreen::FromTo, pacsJob->getPacsDevice().getAETitle());
    item->setText(QOperationStateScreen::PatientID, study->getParentPatient()->getID());
    item->setText(QOperationStateScreen::PatientName, study->getParentPatient()->getFullName());
    item->setText(QOperationStateScreen::Date, QDate::currentDate().toString(Qt::ISODate) + "   " + QTime::currentTime().toString("hh:mm"));
    // Series
    item->setText(QOperationStateScreen::Series, "0");
    // Imatges
    item->setText(QOperationStateScreen::Files, "0");
    item->setText(QOperationStateScreen::PACSJobID, QString().setNum(pacsJob->getPACSJobID()));

    m_treeRetrieveStudy->addTopLevelItem(item);
}

Study* QOperationStateScreen::getStudyFromPACSJob(PACSJobPointer pacsJob)
{
    Study *study = NULL;

    if (pacsJob->getPACSJobType() == PACSJob::SendDICOMFilesToPACSJobType)
    {
        study = pacsJob.objectCast<SendDICOMFilesToPACSJob>()->getStudyOfDICOMFilesToSend();
    }
    else if (pacsJob->getPACSJobType() == PACSJob::RetrieveDICOMFilesFromPACSJobType)
    {
        study = pacsJob.objectCast<RetrieveDICOMFilesFromPACSJob>()->getStudyToRetrieveDICOMFiles();
    }

    return study;
}

QString QOperationStateScreen::getPACSJobStatusResume(PACSJobPointer pacsJob)
{
    if (pacsJob->getPACSJobType() == PACSJob::RetrieveDICOMFilesFromPACSJobType)
    {
        switch (pacsJob.objectCast<RetrieveDICOMFilesFromPACSJob>()->getStatus())
        {
            case PACSRequestStatus::RetrieveOk:
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
        if (pacsJob.objectCast<SendDICOMFilesToPACSJob>()->getStatus() == PACSRequestStatus::SendOk)
        {
            return tr("SENT");
        }
        else
        {
            return tr("ERROR");
        }
    }
}

void QOperationStateScreen::closeEvent(QCloseEvent *ce)
{
    Settings settings;
    settings.saveColumnsWidths(InputOutputSettings::OperationStateColumnsWidth, m_treeRetrieveStudy);
    ce->accept();
}

QTreeWidgetItem* QOperationStateScreen::getQTreeWidgetItemByPACSJobId(int pacsJobID)
{
    QTreeWidgetItem *qtreeWidgetItem = NULL;
    QList<QTreeWidgetItem*> qTreeWidgetPacsJobItems = m_treeRetrieveStudy->findItems(QString().setNum(pacsJobID), Qt::MatchExactly,
                                                                                     QOperationStateScreen::PACSJobID);

    if (!qTreeWidgetPacsJobItems.isEmpty())
    {
        qtreeWidgetItem = qTreeWidgetPacsJobItems.at(0);
    }

    return qtreeWidgetItem;
}

};
