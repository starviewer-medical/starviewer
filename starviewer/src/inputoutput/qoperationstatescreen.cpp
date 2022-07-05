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

#include "inputoutputsettings.h"
#include "patient.h"
#include "starviewerapplication.h"
#include "study.h"
#include "studyoperationresult.h"
#include "studyoperationsservice.h"

#include <QCloseEvent>
#include <QMessageBox>

namespace udg {

QOperationStateScreen::QOperationStateScreen(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);

    createConnections();

    Settings settings;
    settings.restoreColumnsWidths(InputOutputSettings::OperationStateColumnsWidth, m_treeRetrieveStudy);

    QOperationStateScreen::ColumnIndex sortByColumn = (QOperationStateScreen::ColumnIndex)
        settings.getValue(InputOutputSettings::OperationStateListSortByColumn).toInt();

    Qt::SortOrder sortOrderColumn = (Qt::SortOrder) settings.getValue(InputOutputSettings::OperationStateListSortOrder).toInt();
    m_treeRetrieveStudy->sortItems(sortByColumn, sortOrderColumn);

    // Stores the pointer to the result in the user role
    m_treeRetrieveStudy->setColumnHidden(QOperationStateScreen::Result, true);
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

void QOperationStateScreen::createConnections()
{
    connect(m_buttonClear, SIGNAL(clicked()), this, SLOT(clearList()));
    connect(m_cancellAllRequestsButton, SIGNAL(clicked()), this, SLOT(cancelAllRequests()));
    connect(m_cancelSelectedRequestsButton, SIGNAL(clicked()), this, SLOT(cancelSelectedRequests()));

    connect(StudyOperationsService::instance(), &StudyOperationsService::operationRequested, this, &QOperationStateScreen::registerOperation);
}

void QOperationStateScreen::registerOperation(StudyOperationResult *result)
{
    if (result->getOperationType() == StudyOperationResult::OperationType::Retrieve || result->getOperationType() == StudyOperationResult::OperationType::Store)
    {
        insertIntoTree(result);

        connect(result, &StudyOperationResult::started, this, &QOperationStateScreen::onOperationStarted);
        connect(result, &StudyOperationResult::finished, this, &QOperationStateScreen::onOperationFinished);
        connect(result, &StudyOperationResult::cancelled, this, &QOperationStateScreen::onOperationCancelled);
        connect(result, &StudyOperationResult::instanceTransferred, this, &QOperationStateScreen::onInstanceTransferred);
        connect(result, &StudyOperationResult::seriesTransferred, this, &QOperationStateScreen::onSeriesTransferred);
    }
}

void QOperationStateScreen::onOperationStarted(StudyOperationResult *result)
{
    QTreeWidgetItem *item = m_resultToItemMap.value(result);
    item->setText(QOperationStateScreen::Status,
                  result->getOperationType() == StudyOperationResult::OperationType::Retrieve ? tr("RETRIEVING") : tr("SENDING"));
}

void QOperationStateScreen::onOperationFinished(StudyOperationResult *result)
{
    QTreeWidgetItem *item = m_resultToItemMap.value(result);
    QString status;

    if (!result->getErrorText().isEmpty())
    {
        status = tr("ERROR");
    }
    else if (result->getOperationType() == StudyOperationResult::OperationType::Retrieve)
    {
        status = tr("RETRIEVED");
    }
    else if (result->getOperationType() == StudyOperationResult::OperationType::Store)
    {
        status = tr("SENT");
    }

    item->setText(QOperationStateScreen::Status, status);
    m_resultToItemMap.remove(result);
}

void QOperationStateScreen::onOperationCancelled(StudyOperationResult *result)
{
    QTreeWidgetItem *item = m_resultToItemMap.value(result);
    item->setText(QOperationStateScreen::Status, tr("CANCELLED"));
    m_resultToItemMap.remove(result);
}

void QOperationStateScreen::onInstanceTransferred(StudyOperationResult *result, int totalInstancesTransferred)
{
    QTreeWidgetItem *item = m_resultToItemMap.value(result);
    item->setText(QOperationStateScreen::Files, QString::number(totalInstancesTransferred));
}

void QOperationStateScreen::onSeriesTransferred(StudyOperationResult *result, int totalSeriesTransferred)
{
    QTreeWidgetItem *item = m_resultToItemMap.value(result);
    item->setText(QOperationStateScreen::Series, QString::number(totalSeriesTransferred));
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
        delete m_treeRetrieveStudy->invisibleRootItem()->takeChild(m_treeRetrieveStudy->invisibleRootItem()->indexOfChild(itemToClear));
    }
}

void QOperationStateScreen::cancelAllRequests()
{
    for (StudyOperationResult *result : m_resultToItemMap.keys())
    {
        QTreeWidgetItem *item = m_resultToItemMap.value(result);
        item->setText(QOperationStateScreen::Status, tr("CANCELLING"));
        result->cancel();
    }
}

void QOperationStateScreen::cancelSelectedRequests()
{
    if (m_treeRetrieveStudy->selectedItems().count() > 0)
    {
        foreach (QTreeWidgetItem *item, m_treeRetrieveStudy->selectedItems())
        {
            StudyOperationResult *result = item->data(QOperationStateScreen::Result, Qt::UserRole).value<StudyOperationResult*>();

            if (m_resultToItemMap.contains(result))
            {
                item->setText(QOperationStateScreen::Status, tr("CANCELLING"));
                result->cancel();
            }
        }
    }
    else
    {
        QMessageBox::information(this, ApplicationNameString, tr("You have to select at least one operation to cancel."));
    }
}

void QOperationStateScreen::insertIntoTree(StudyOperationResult *result)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();

    item->setText(QOperationStateScreen::Status, tr("PENDING"));
    item->setText(QOperationStateScreen::Direction, result->getOperationType() == StudyOperationResult::OperationType::Store ? tr("Server") : tr("Local"));
    const PacsDevice &pacs = result->getRequestPacsDevice();
    QString remoteEnd;
    if (result->getOperationType() == StudyOperationResult::OperationType::Retrieve)
    {
        remoteEnd = pacs.getType() == PacsDevice::Type::Dimse ? pacs.getAETitle() : pacs.getBaseUri().toString();
    }
    else if (result->getOperationType() == StudyOperationResult::OperationType::Store)
    {
        remoteEnd = pacs.getType() == PacsDevice::Type::Wado ? pacs.getBaseUri().toString() : pacs.getAETitle();
    }
    item->setText(QOperationStateScreen::FromTo, remoteEnd);
    item->setText(QOperationStateScreen::PatientID, result->getRequestStudy()->getParentPatient()->getID());
    item->setText(QOperationStateScreen::PatientName, result->getRequestStudy()->getParentPatient()->getFullName());
    item->setText(QOperationStateScreen::Date, QDate::currentDate().toString(Qt::ISODate) + "   " + QTime::currentTime().toString("hh:mm"));
    // Series
    item->setText(QOperationStateScreen::Series, "0");
    // Imatges
    item->setText(QOperationStateScreen::Files, "0");
    item->setData(QOperationStateScreen::Result, Qt::UserRole, QVariant::fromValue(result));

    m_treeRetrieveStudy->addTopLevelItem(item);
    m_resultToItemMap[result] = item;
}

void QOperationStateScreen::closeEvent(QCloseEvent *ce)
{
    Settings settings;
    settings.saveColumnsWidths(InputOutputSettings::OperationStateColumnsWidth, m_treeRetrieveStudy);
    ce->accept();
}

}
