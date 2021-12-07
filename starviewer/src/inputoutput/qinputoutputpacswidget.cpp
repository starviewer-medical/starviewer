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

#include "qinputoutputpacswidget.h"

#include "dicommask.h"
#include "inputoutputsettings.h"
#include "logging.h"
#include "pacsrequeststatus.h"
#include "settings.h"
#include "shortcutmanager.h"
#include "starviewerapplication.h"
#include "statswatcher.h"
#include "study.h"
#include "studyoperationresult.h"
#include "studyoperationsservice.h"

#include <QMessageBox>
#include <QMovie>
#include <QShortcut>

namespace udg {

namespace {

// Returns a DicomMask to search for all series in the study with the given Study Instance UID.
DicomMask buildSeriesDicomMask(QString studyInstanceUID)
{
    DicomMask mask;

    mask.setStudyInstanceUID(studyInstanceUID);
    mask.setSeriesDate(QDate(), QDate());
    mask.setSeriesTime(QTime(), QTime());
    mask.setSeriesModality("");
    mask.setSeriesNumber("");
    mask.setSeriesInstanceUID("");
    mask.setPPSStartDate(QDate(), QDate());
    mask.setPPStartTime(QTime(), QTime());
    mask.setRequestAttributeSequence("", "");
    mask.setSeriesDescription("");
    mask.setSeriesProtocolName("");

    return mask;
}

// Returns a DicomMask to search for all instances in the series with the given Series Instance UID in the study with the given Study Instance UID.
DicomMask buildImageDicomMask(QString studyInstanceUID, QString seriesInstanceUID)
{
    DicomMask mask;

    mask.setStudyInstanceUID(studyInstanceUID);
    mask.setSeriesInstanceUID(seriesInstanceUID);
    mask.setImageNumber("");
    mask.setSOPInstanceUID("");

    return mask;
}

}

QInputOutputPacsWidget::QInputOutputPacsWidget(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);

    createContextMenuQStudyTreeWidget();

    Settings settings;
    settings.restoreColumnsWidths(InputOutputSettings::PACSStudyListColumnsWidth, m_studyTreeWidget->getQTreeWidget());

    QStudyTreeWidget::ColumnIndex sortByColumn = (QStudyTreeWidget::ColumnIndex) settings.getValue(InputOutputSettings::PACSStudyListSortByColumn).toInt();
    Qt::SortOrder sortOrderColumn = (Qt::SortOrder) settings.getValue(InputOutputSettings::PACSStudyListSortOrder).toInt();
    m_studyTreeWidget->setSortByColumn (sortByColumn, sortOrderColumn);

    m_statsWatcher = new StatsWatcher("QueryInputOutputPacsWidget", this);
    m_statsWatcher->addClicksCounter(m_retrievAndViewButton);
    m_statsWatcher->addClicksCounter(m_retrieveButton);

    // Preparem el QMovie per indicar quan s'estan fent consultes al PACS
    QMovie *operationAnimation = new QMovie(this);
    operationAnimation->setFileName(":/images/animations/loader.gif");
    m_queryAnimationLabel->setMovie(operationAnimation);
    operationAnimation->start();

    setQueryInProgress(false);

    createConnections();
}

QInputOutputPacsWidget::~QInputOutputPacsWidget()
{
    Settings settings;
    settings.saveColumnsWidths(InputOutputSettings::PACSStudyListColumnsWidth, m_studyTreeWidget->getQTreeWidget());

    // Guardem per quin columna està ordenada la llista d'estudis i en quin ordre
    settings.setValue(InputOutputSettings::PACSStudyListSortByColumn, m_studyTreeWidget->getSortColumn());
    settings.setValue(InputOutputSettings::PACSStudyListSortOrder, m_studyTreeWidget->getSortOrderColumn());

    // Cancel and schedule deletion of current queries
    for (StudyOperationResult *result : m_pendingQueryResults)
    {
        result->cancel();
        result->deleteLater();
    }
}

void QInputOutputPacsWidget::createConnections()
{
    connect(m_studyTreeWidget, SIGNAL(requestedSeriesOfStudy(Study*)), SLOT(requestedSeriesOfStudy(Study*)));
    connect(m_studyTreeWidget, SIGNAL(requestedImagesOfSeries(Series*)), SLOT(requestedImagesOfSeries(Series*)));

    connect(m_studyTreeWidget, SIGNAL(studyDoubleClicked()), SLOT(retrieveSelectedItemsFromQStudyTreeWidget()));
    connect(m_studyTreeWidget, SIGNAL(seriesDoubleClicked()), SLOT(retrieveSelectedItemsFromQStudyTreeWidget()));
    connect(m_studyTreeWidget, SIGNAL(imageDoubleClicked()), SLOT(retrieveSelectedItemsFromQStudyTreeWidget()));

    connect(m_retrievAndViewButton, SIGNAL(clicked()), SLOT(retrieveAndViewSelectedItemsFromQStudyTreeWidget()));
    connect(m_retrieveButton, SIGNAL(clicked()), SLOT(retrieveSelectedItemsFromQStudyTreeWidget()));

    connect(m_cancelQueryButton, SIGNAL(clicked()), SLOT(cancelCurrentQueriesToPACS()));
}

void QInputOutputPacsWidget::createContextMenuQStudyTreeWidget()
{
    QAction *action;

    action = m_contextMenuQStudyTreeWidget.addAction(QIcon(":/images/icons/download-view.svg"), tr("Retrieve && &View"), this,
                                                     SLOT(retrieveAndViewSelectedItemsFromQStudyTreeWidget()));
    action->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::RetrieveAndViewSelectedStudies));
    (void) new QShortcut(action->shortcut(), this, SLOT(retrieveAndViewSelectedItemsFromQStudyTreeWidget()));

    action = m_contextMenuQStudyTreeWidget.addAction(QIcon(":/images/icons/folder-download.svg"), tr("&Retrieve"), this, SLOT(retrieveSelectedItemsFromQStudyTreeWidget()));
    action->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::RetrieveSelectedStudies));
    (void) new QShortcut(action->shortcut(), this, SLOT(retrieveSelectedItemsFromQStudyTreeWidget()));

    // Especifiquem que es el menu del dicomdir
    m_studyTreeWidget->setContextMenu(& m_contextMenuQStudyTreeWidget);
}

void QInputOutputPacsWidget::queryStudy(DicomMask queryMask, QList<PacsDevice> pacsToQueryList)
{
    if (pacsToQueryList.count() == 0)
    {
        QMessageBox::information(this, ApplicationNameString, tr("You have to select at least one PACS to query."));
        return;
    }

    if (areValidQueryParameters(&queryMask, pacsToQueryList))
    {
        cancelCurrentQueriesToPACS();

        m_studyTreeWidget->clear();

        foreach (const PacsDevice &pacsDeviceToQuery, pacsToQueryList)
        {
            StudyOperationResult *result = StudyOperationsService::instance()->searchPacs(pacsDeviceToQuery, queryMask,
                                                                                          StudyOperations::TargetResource::Studies);
            addPendingQuery(result);
        }
    }
}

void QInputOutputPacsWidget::addPendingQuery(StudyOperationResult *result)
{
    // This connections will be deleted when result is destroyed
    connect(result, &StudyOperationResult::finishedSuccessfully, this, &QInputOutputPacsWidget::showQueryResult);
    connect(result, &StudyOperationResult::finishedWithError, this, &QInputOutputPacsWidget::showQueryError);
    connect(result, &StudyOperationResult::cancelled, this, &QInputOutputPacsWidget::onQueryCancelled);
    connect(result, &StudyOperationResult::ended, result, &StudyOperationResult::deleteLater);

    m_pendingQueryResults.insert(result);
    setQueryInProgress(true);
}

void QInputOutputPacsWidget::showQueryResult(StudyOperationResult *result)
{
    if (result->getResultType() == StudyOperationResult::ResultType::Studies)
    {
        m_studyTreeWidget->insertPatientList(result->getStudies());
    }
    else if (result->getResultType() == StudyOperationResult::ResultType::Series)
    {
        QList<Series*> seriesList = result->getSeries();
        QString studyInstanceUID = result->getRequestStudyInstanceUid();

        if (seriesList.isEmpty())
        {
            QMessageBox::information(this, ApplicationNameString, tr("No series match for this study %1.").arg(studyInstanceUID) + "\n");
        }
        else
        {
            m_studyTreeWidget->insertSeriesList(studyInstanceUID, seriesList);
        }
    }
    else if (result->getResultType() == StudyOperationResult::ResultType::Instances)
    {
        QList<Image*> imageList = result->getInstances();
        QString studyInstanceUID = result->getRequestStudyInstanceUid();
        QString seriesInstanceUID = result->getRequestSeriesInstanceUid();

        if (imageList.isEmpty())
        {
            QMessageBox::information(this, ApplicationNameString, tr("No images match series %1.").arg(seriesInstanceUID) + "\n");
        }
        else
        {
            m_studyTreeWidget->insertImageList(studyInstanceUID, seriesInstanceUID, imageList);
        }
    }

    m_pendingQueryResults.erase(result);
    setQueryInProgress(!m_pendingQueryResults.empty());
}

void QInputOutputPacsWidget::showQueryError(StudyOperationResult *result)
{
    // Critical if requested studies, warning if requested series or instances from a study
    if (result->getRequestLevel() == StudyOperationResult::RequestLevel::Studies)
    {
        QMessageBox::critical(this, ApplicationNameString, result->getErrorText());
    }
    else
    {
        QMessageBox::warning(this, ApplicationNameString, result->getErrorText());
    }

    m_pendingQueryResults.erase(result);
    setQueryInProgress(!m_pendingQueryResults.empty());
}

void QInputOutputPacsWidget::onQueryCancelled(StudyOperationResult *result)
{
    m_pendingQueryResults.erase(result);
    setQueryInProgress(!m_pendingQueryResults.empty());
}

void QInputOutputPacsWidget::cancelCurrentQueriesToPACS()
{
    for (auto it = m_pendingQueryResults.begin(); it != m_pendingQueryResults.end(); )
    {
        (*it)->cancel();
        it = m_pendingQueryResults.erase(it);
    }

    // Les consultes al PACS poden tarda variis segons a cancel·lar-se, ja que com està documentat hi ha PACS que una vegada un PACS rep l'orde de cancel·lació
    // envien els resultats que havien trobat fins aquell moment i després tanquen la connexió, per fer transparent això a l'usuari, ja que ell no ho notarà en
    // quin moment es cancel·len, ja amaguem el gif indicant que s'ha cancel·lat la consulta, perquè tingui la sensació que s'han cancel·lat immediatament
    setQueryInProgress(false);
}

void QInputOutputPacsWidget::clear()
{
    m_studyTreeWidget->clear();
}

void QInputOutputPacsWidget::requestedSeriesOfStudy(Study *study)
{
    if (study->getDICOMSource().getRetrievePACS().count() == 0)
    {
        ERROR_LOG(QString("No s'ha trobat de quin PACS es l'estudi %1 per obtenir-ne les series").arg(study->getInstanceUID()));
        return;
    }

    PacsDevice pacsDevice = study->getDICOMSource().getRetrievePACS().at(0);
    QString pacsDescription = pacsDevice.getAETitle() + " Institució" + pacsDevice.getInstitution() + " IP:" + pacsDevice.getAddress();

    INFO_LOG("Cercant informacio de les series de l'estudi" + study->getInstanceUID() + " del PACS " + pacsDescription);

    DicomMask mask = buildSeriesDicomMask(study->getInstanceUID());
    StudyOperationResult *result = StudyOperationsService::instance()->searchPacs(pacsDevice, mask, StudyOperations::TargetResource::Series);
    addPendingQuery(result);
}

void QInputOutputPacsWidget::requestedImagesOfSeries(Series *series)
{
    if (series->getDICOMSource().getRetrievePACS().count() == 0)
    {
        ERROR_LOG(QString("No s'ha trobat de quin PACS es la serie per obtenir-ne les imatges").arg(series->getInstanceUID()));
        return;
    }

    PacsDevice pacsDevice = series->getDICOMSource().getRetrievePACS().at(0);
    QString pacsDescription = pacsDevice.getAETitle() + " Institució" + pacsDevice.getInstitution() + " IP:" + pacsDevice.getAddress();

    INFO_LOG("Cercant informacio de les imatges de la serie" + series->getInstanceUID() + " de l'estudi" + series->getParentStudy()->getInstanceUID() +
             " del PACS " + pacsDescription);

    DicomMask mask = buildImageDicomMask(series->getParentStudy()->getInstanceUID(), series->getInstanceUID());
    StudyOperationResult *result = StudyOperationsService::instance()->searchPacs(pacsDevice, mask, StudyOperations::TargetResource::Instances);
    addPendingQuery(result);
}

void QInputOutputPacsWidget::retrieveSelectedItemsFromQStudyTreeWidget()
{
    if (m_studyTreeWidget->getDicomMaskOfSelectedItems().isEmpty())
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, ApplicationNameString, tr("Select a study to retrieve."));
        return;
    }

    retrieveSelectedItemsFromQStudyTreeWidget(None);
}

void QInputOutputPacsWidget::retrieveAndViewSelectedItemsFromQStudyTreeWidget()
{
    if (m_studyTreeWidget->getDicomMaskOfSelectedItems().isEmpty())
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, ApplicationNameString, tr("Select a study to retrieve and view."));
        return;
    }

    retrieveSelectedItemsFromQStudyTreeWidget(View);
}

void QInputOutputPacsWidget::retrieveSelectedItemsFromQStudyTreeWidget(ActionsAfterRetrieve _actionsAfterRetrieve)
{
    QList<QPair<DicomMask, DICOMSource> > dicomMaskDICOMSourceList = m_studyTreeWidget->getDicomMaskOfSelectedItems();

    for (int index = 0; index < dicomMaskDICOMSourceList.count(); index++)
    {
        if (dicomMaskDICOMSourceList.at(index).second.getRetrievePACS().count() > 0)
        {
            DicomMask dicomMaskToRetrieve = dicomMaskDICOMSourceList.at(index).first;
            DICOMSource dicomSourceStudyToRetrieve = dicomMaskDICOMSourceList.at(index).second;
            PacsDevice pacsDeviceFromRetrieve = dicomSourceStudyToRetrieve.getRetrievePACS().at(0); //Agafem el primer PACS

            retrieve(pacsDeviceFromRetrieve, _actionsAfterRetrieve, m_studyTreeWidget->getStudy(dicomMaskToRetrieve.getStudyInstanceUID(), dicomSourceStudyToRetrieve),
                dicomMaskToRetrieve.getSeriesInstanceUID(), dicomMaskToRetrieve.getSOPInstanceUID());
        }
    }
}

void QInputOutputPacsWidget::onRetrieveSuccess(StudyOperationResult *result, ActionsAfterRetrieve action)
{
    emit studyRetrieveFinished(result->getStudyInstanceUid());

    switch (action)
    {
        case Load:
            emit loadRetrievedStudy(result->getStudyInstanceUid());
            break;
        case View:
            emit viewRetrievedStudy(result->getStudyInstanceUid());
            break;
        default:
            break;
    }
}

void QInputOutputPacsWidget::onRetrievePartialSuccess(StudyOperationResult *result, ActionsAfterRetrieve action)
{
    QMessageBox::warning(this, ApplicationNameString, result->getErrorText());
    onRetrieveSuccess(result, action);
}

void QInputOutputPacsWidget::onRetrieveError(StudyOperationResult *result)
{
    QMessageBox::critical(this, ApplicationNameString, result->getErrorText());
}

void QInputOutputPacsWidget::retrieve(const PacsDevice &pacsDevice, ActionsAfterRetrieve actionAfterRetrieve, Study *studyToRetrieve,
    const QString &seriesInstanceUIDToRetrieve, const QString &sopInstanceUIDToRetrieve)
{
    auto priority = actionAfterRetrieve == View ? StudyOperations::RetrievePriority::High : StudyOperations::RetrievePriority::Medium;
    StudyOperationResult *result = StudyOperationsService::instance()->retrieveFromPacs(pacsDevice, studyToRetrieve, seriesInstanceUIDToRetrieve,
                                                                                        sopInstanceUIDToRetrieve, priority);

    // This connections will be deleted when result is destroyed
    connect(result, &StudyOperationResult::finishedSuccessfully, this, [=](StudyOperationResult *result) {
        onRetrieveSuccess(result, actionAfterRetrieve);
    });
    connect(result, &StudyOperationResult::finishedWithPartialSuccess, this, [=](StudyOperationResult *result) {
        onRetrievePartialSuccess(result, actionAfterRetrieve);
    });
    connect(result, &StudyOperationResult::finishedWithError, this, &QInputOutputPacsWidget::onRetrieveError);
    connect(result, &StudyOperationResult::ended, result, &StudyOperationResult::deleteLater);
}

bool QInputOutputPacsWidget::areValidQueryParameters(DicomMask *maskToQuery, QList<PacsDevice> pacsToQuery)
{
    // Es comprova que hi hagi pacs seleccionats
    if (pacsToQuery.isEmpty())
    {
        QMessageBox::warning(this, ApplicationNameString, tr("Please select a PACS to query."));
        return false;
    }

    if (maskToQuery->isEmpty())
    {
        QMessageBox::StandardButton response;
        response = QMessageBox::question(this, ApplicationNameString, tr("No search fields were filled.") + "\n" +
            tr("The query can take a long time.\nDo you want continue?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        return (response == QMessageBox::Yes);
    }
    else
    {
        return true;
    }
}

void QInputOutputPacsWidget::setQueryInProgress(bool queryInProgress)
{
    m_queryAnimationLabel->setVisible(queryInProgress);
    m_queryInProgressLabel->setVisible(queryInProgress);
    m_cancelQueryButton->setEnabled(queryInProgress);
}

}
