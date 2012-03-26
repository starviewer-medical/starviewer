#include "qinputoutputlocaldatabasewidget.h"

#include <QMessageBox>
#include <QShortcut>

#include "logging.h"
#include "starviewerapplication.h"
#include "dicommask.h"
#include "patient.h"
#include "statswatcher.h"
#include "qcreatedicomdir.h"
#include "inputoutputsettings.h"
#include "qwidgetselectpacstostoredicomimage.h"
#include "pacsmanager.h"
#include "senddicomfilestopacsjob.h"
#include "retrievedicomfilesfrompacsjob.h"
#include "shortcutmanager.h"

namespace udg {

QInputOutputLocalDatabaseWidget::QInputOutputLocalDatabaseWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);

    createContextMenuQStudyTreeWidget();

    Settings settings;
    settings.restoreColumnsWidths(InputOutputSettings::LocalDatabaseStudyList, m_studyTreeWidget->getQTreeWidget());
    settings.restoreGeometry(InputOutputSettings::LocalDatabaseSplitterState, m_StudyTreeSeriesListQSplitter);

    QStudyTreeWidget::ColumnIndex sortByColumn = (QStudyTreeWidget::ColumnIndex)
        settings.getValue(InputOutputSettings::LocalDatabaseStudyListSortByColumn).toInt();

    Qt::SortOrder sortOrderColumn = (Qt::SortOrder) settings.getValue(InputOutputSettings::LocalDatabaseStudyListSortOrder).toInt();
    m_studyTreeWidget->setSortByColumn (sortByColumn, sortOrderColumn);

    m_statsWatcher = new StatsWatcher("QueryInputOutputLocalDatabaseWidget", this);
    m_statsWatcher->addClicksCounter(m_viewButton);

    m_studyTreeWidget->setMaximumExpandTreeItemsLevel(QStudyTreeWidget::SeriesLevel);
    m_studyTreeWidget->setUseDICOMSourceToDiscriminateStudies(false);

    m_qwidgetSelectPacsToStoreDicomImage = new QWidgetSelectPacsToStoreDicomImage();

    createConnections();

    // Esborrem els estudis vells de la cache.
    //ATENCIÓ!S'ha de fer després del createConnections perquè sinó no haurem connectat amb el signal per control els errors al esborrar estudis
    //TODO: Això s'hauria de moure fora d'aquí no ha de ser responsabilitat d'aquesta classe
    deleteOldStudies();
}

QInputOutputLocalDatabaseWidget::~QInputOutputLocalDatabaseWidget()
{
    Settings settings;
    settings.saveColumnsWidths(InputOutputSettings::LocalDatabaseStudyList, m_studyTreeWidget->getQTreeWidget());

    // Guardem per quin columna està ordenada la llista d'estudis i en quin ordre
    settings.setValue(InputOutputSettings::LocalDatabaseStudyListSortByColumn, m_studyTreeWidget->getSortColumn());
    settings.setValue(InputOutputSettings::LocalDatabaseStudyListSortOrder, m_studyTreeWidget->getSortOrderColumn());

}

void QInputOutputLocalDatabaseWidget::createConnections()
{
    connect(m_studyTreeWidget, SIGNAL(requestedSeriesOfStudy(Study*)), SLOT(requestedSeriesOfStudy(Study*)));

    connect(m_studyTreeWidget, SIGNAL(studyDoubleClicked()), SLOT(viewFromQStudyTreeWidget()));
    connect(m_studyTreeWidget, SIGNAL(seriesDoubleClicked()), SLOT(viewFromQStudyTreeWidget()));
    connect(m_studyTreeWidget, SIGNAL(imageDoubleClicked()), SLOT(viewFromQStudyTreeWidget()));

    connect(m_viewButton, SIGNAL(clicked()), SLOT(viewFromQStudyTreeWidget()));

    connect(m_seriesThumbnailPreviewWidget, SIGNAL(seriesThumbnailClicked(QString,QString)), this, SLOT(currentSeriesChangedOfQSeriesListWidget(QString, QString)));
    connect(m_seriesThumbnailPreviewWidget, SIGNAL(seriesThumbnailDoubleClicked(QString,QString)), SLOT(viewFromQSeriesListWidget(QString, QString)));
    connect(m_studyTreeWidget, SIGNAL(currentStudyChanged(Study*)), SLOT(setSeriesToSeriesListWidget(Study*)));
    connect(m_studyTreeWidget, SIGNAL(currentSeriesChanged(Series*)), SLOT(currentSeriesOfQStudyTreeWidgetChanged(Series*)));
    // Si passem de tenir un element seleccionat a no tenir-ne li diem al seriesListWidget que no mostri cap previsualització
    connect(m_studyTreeWidget, SIGNAL(notCurrentItemSelected()), m_seriesThumbnailPreviewWidget, SLOT(clear()));

    // Connecta amb el signal que indica que ha finalitza el thread d'esborrar els estudis vells
    connect(&m_qdeleteOldStudiesThread, SIGNAL(finished()), SLOT(deleteOldStudiesThreadFinished()));

    /// Si movem el QSplitter capturem el signal per guardar la seva posició
    connect(m_StudyTreeSeriesListQSplitter, SIGNAL(splitterMoved (int, int)), SLOT(qSplitterPositionChanged()));
    connect(m_qwidgetSelectPacsToStoreDicomImage, SIGNAL(selectedPacsToStore()), SLOT(sendSelectedStudiesToSelectedPacs()));
}

void QInputOutputLocalDatabaseWidget::createContextMenuQStudyTreeWidget()
{
    QAction *action;

    action = m_contextMenuQStudyTreeWidget.addAction(QIcon(":/images/view.png"), tr("&View"), this, SLOT(viewFromQStudyTreeWidget()));
    action->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::ViewSelectedStudies));
    (void) new QShortcut(action->shortcut(), this, SLOT(viewFromQStudyTreeWidget()));

    action = m_contextMenuQStudyTreeWidget.addAction(QIcon(":/images/databaseRemove.png"), tr("&Delete"), this, SLOT(deleteSelectedItemsFromLocalDatabase()));
    action->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::DeleteSelectedLocalDatabaseStudies));
    (void) new QShortcut(action->shortcut(), this, SLOT(deleteSelectedItemsFromLocalDatabase()));

#ifndef STARVIEWER_LITE
    action = m_contextMenuQStudyTreeWidget.addAction(tr("Send to DICOMDIR List"), this, SLOT(addSelectedStudiesToCreateDicomdirList()));
    action->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::SendSelectedStudiesToDICOMDIRList));
    (void) new QShortcut(action->shortcut(), this, SLOT(addSelectedStudiesToCreateDicomdirList()));

    action = m_contextMenuQStudyTreeWidget.addAction(QIcon(":/images/store.png"), tr("Send to PACS"), this, SLOT(selectedStudiesStoreToPacs()));
    action->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::StoreSelectedStudiesToPACS));
    (void) new QShortcut(action->shortcut(), this, SLOT(selectedStudiesStoreToPacs()));
#endif
    // Especifiquem que és el menú per la cache
    m_studyTreeWidget->setContextMenu(&m_contextMenuQStudyTreeWidget);
}

// TODO s'hauria buscar una manera més elegant de comunicar les dos classes, fer un singletton de QCreateDicomdir ?
void QInputOutputLocalDatabaseWidget::setQCreateDicomdir(QCreateDicomdir *qcreateDicomdir)
{
    m_qcreateDicomdir = qcreateDicomdir;
}

void QInputOutputLocalDatabaseWidget::clear()
{
    m_studyTreeWidget->clear();
    m_seriesThumbnailPreviewWidget->clear();
}

void QInputOutputLocalDatabaseWidget::setPacsManager(PacsManager *pacsManager)
{
    m_pacsManager = pacsManager;
    connect(pacsManager, SIGNAL(newPACSJobEnqueued(PACSJob*)), SLOT(newPACSJobEnqueued(PACSJob*)));
}

void QInputOutputLocalDatabaseWidget::queryStudy(DicomMask queryMask)
{
    LocalDatabaseManager localDatabaseManager;
    QList<Patient*> patientStudyList;

    StatsWatcher::log("Cerca d'estudis a la base de dades local amb paràmetres: " + queryMask.getFilledMaskFields());
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    clear();

    patientStudyList = localDatabaseManager.queryPatientStudy(queryMask);

    if (showDatabaseManagerError(localDatabaseManager.getLastError()))
    {
        return;
    }

    // Aquest mètode a part de ser cridada quan l'usuari fa click al botó search, també es cridada al
    // constructor d'aquesta classe, per a que al engegar l'aplicació ja es mostri la llista d'estudis
    // que hi ha a la base de dades local. Si el mètode no troba cap estudi a la base de dades local
    // es llença el missatge que no s'han trobat estudis, però com que no és idonii, en el cas aquest que es
    // crida des del constructor que es mostri el missatge de que no s'han trobat estudis al engegar l'aplicació, el que
    // es fa és que per llançar el missatge es comprovi que la finestra estigui activa. Si la finestra no està activa
    // vol dir que el mètode ha estat invocat des del constructor
    if (patientStudyList.isEmpty() && isActiveWindow())
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::information(this, ApplicationNameString, tr("No study match found."));
    }
    else
    {
        // Es mostra la llista d'estudis
        m_studyTreeWidget->insertPatientList(patientStudyList, true);
        QApplication::restoreOverrideCursor();
    }
}

void QInputOutputLocalDatabaseWidget::addStudyToQStudyTreeWidget(QString studyUID)
{
    LocalDatabaseManager localDatabaseManager;
    DicomMask studyMask;
    QList<Patient*> patientList;

    studyMask.setStudyInstanceUID(studyUID);
    patientList = localDatabaseManager.queryPatientStudy(studyMask);
    if (showDatabaseManagerError(localDatabaseManager.getLastError()))
    {
        return;
    }

    if (patientList.count() == 1)
    {
        m_studyTreeWidget->insertPatient(patientList.at(0), true);
        m_studyTreeWidget->sort();
    }
}

void QInputOutputLocalDatabaseWidget::removeStudyFromQStudyTreeWidget(QString studyInstanceUID)
{
    m_studyTreeWidget->removeStudy(studyInstanceUID);
}

void QInputOutputLocalDatabaseWidget::requestedSeriesOfStudy(Study *study)
{ 
    INFO_LOG("Cerca de sèries a la font cache de l'estudi " + study->getInstanceUID());

    // Preparem la mascara i cerquem les series a la cache
    DicomMask mask;
    mask.setStudyInstanceUID(study->getInstanceUID());

    LocalDatabaseManager localDatabaseManager;
    QList<Series*> seriesList = localDatabaseManager.querySeries(mask);

    if (showDatabaseManagerError(localDatabaseManager.getLastError()))
    {
        return;
    }

    if (seriesList.isEmpty())
    {
        QMessageBox::information(this, ApplicationNameString, tr("No series match for this study.\n"));
        return;
    }
    else
    {
        // Inserim la informació de les sèries al estudi
        m_studyTreeWidget->insertSeriesList(study->getInstanceUID(), seriesList);
    }
}

void QInputOutputLocalDatabaseWidget::setSeriesToSeriesListWidget(Study *currentStudy)
{
    m_seriesThumbnailPreviewWidget->clear();

    if (!currentStudy)
    {
        return;
    }

    INFO_LOG("Cerca de sèries a la cache de l'estudi " + currentStudy->getInstanceUID());

    DicomMask mask;
    mask.setStudyInstanceUID(currentStudy->getInstanceUID());

    LocalDatabaseManager localDatabaseManager;
    QList<Series*> seriesList = localDatabaseManager.querySeries(mask);
    if (showDatabaseManagerError(localDatabaseManager.getLastError()))
    {
        return;
    }

    foreach (Series *series, seriesList)
    {
        m_seriesThumbnailPreviewWidget->insertSeries(currentStudy->getInstanceUID(), series);
    }

    qDeleteAll(seriesList);
}

void QInputOutputLocalDatabaseWidget::currentSeriesOfQStudyTreeWidgetChanged(Series *series)
{
    if (series)
    {
        m_seriesThumbnailPreviewWidget->setCurrentSeries(series->getInstanceUID());
    }
}

void QInputOutputLocalDatabaseWidget::currentSeriesChangedOfQSeriesListWidget(const QString &studyInstanceUID, const QString &seriesInstanceUID)
{
    m_studyTreeWidget->setCurrentSeries(studyInstanceUID, seriesInstanceUID);
}

void QInputOutputLocalDatabaseWidget::deleteSelectedItemsFromLocalDatabase()
{
    QList<QPair<DicomMask, DICOMSource> > selectedDicomMaskDICOMSoruceToDelete = m_studyTreeWidget->getDicomMaskOfSelectedItems();

    if (!selectedDicomMaskDICOMSoruceToDelete.isEmpty())
    {
        QMessageBox::StandardButton response = QMessageBox::question(this, ApplicationNameString,
                                                                           tr("Are you sure you want to delete the selected Items?"),
                                                                           QMessageBox::Yes | QMessageBox::No,
                                                                           QMessageBox::No);
        if (response == QMessageBox::Yes)
        {
            QApplication::setOverrideCursor(Qt::BusyCursor);
            LocalDatabaseManager localDatabaseManager;

            for (int index = 0; index < selectedDicomMaskDICOMSoruceToDelete.count(); index++)
            {
                DicomMask dicomMaskToDelete = selectedDicomMaskDICOMSoruceToDelete.at(index).first;
                if (m_qcreateDicomdir->studyExistsInDICOMDIRList(dicomMaskToDelete.getStudyInstanceUID()))
                {
                    Study *studyToDelete = m_studyTreeWidget->getStudy(dicomMaskToDelete.getStudyInstanceUID());
                    QString warningMessage;

                    if (dicomMaskToDelete.getSeriesInstanceUID().isEmpty())
                    {
                        warningMessage = tr("The study %1 of patient %2 is in use by the DICOMDIR List. If you want to delete "
                                            "this study you should remove it from the DICOMDIR List first.")
                                         .arg(studyToDelete->getID(), studyToDelete->getParentPatient()->getFullName());
                    }
                    else
                    {
                        // TODO:Hauriem de mostar el Series ID en lloc del Series UID
                        warningMessage = tr("The series with UID %1 of study %2 patient %3 is in use by the DICOMDIR List. If you want to delete "
                                            "this series you should remove the study from the DICOMDIR List first.")
                                         .arg(dicomMaskToDelete.getSeriesInstanceUID(), studyToDelete->getID(),
                                              studyToDelete->getParentPatient()->getFullName());
                    }

                    QMessageBox::warning(this, ApplicationNameString, warningMessage);
                }
                else
                {
                    if (!dicomMaskToDelete.getSeriesInstanceUID().isEmpty())
                    {
                        INFO_LOG(QString("L'usuari ha indicat que vol esborrar de la cache la serie %1 de l'estudi %2")
                                    .arg(dicomMaskToDelete.getSeriesInstanceUID(), dicomMaskToDelete.getStudyInstanceUID()));
                        localDatabaseManager.deleteSeries(dicomMaskToDelete.getStudyInstanceUID(), dicomMaskToDelete.getSeriesInstanceUID());

                        m_seriesThumbnailPreviewWidget->removeSeries(dicomMaskToDelete.getSeriesInstanceUID());
                        m_studyTreeWidget->removeSeries(dicomMaskToDelete.getStudyInstanceUID(), dicomMaskToDelete.getSeriesInstanceUID());
                    }
                    else
                    {
                        INFO_LOG(QString("L'usuari ha indicat que vol esborrar de la cache l'estudi %1").arg(dicomMaskToDelete.getStudyInstanceUID()));
                        localDatabaseManager.deleteStudy(dicomMaskToDelete.getStudyInstanceUID());

                        m_seriesThumbnailPreviewWidget->clear();
                        removeStudyFromQStudyTreeWidget(dicomMaskToDelete.getStudyInstanceUID());
                    }

                    if (showDatabaseManagerError(localDatabaseManager.getLastError()))
                    {
                        break;
                    }
                }
            }
            QApplication::restoreOverrideCursor();
        }
    }
    else
    {
        QMessageBox::information(this, ApplicationNameString, tr("Please select at least one item to delete."));
    }
}

void QInputOutputLocalDatabaseWidget::view(QString studyInstanceUID, bool loadOnly)
{
    DicomMask studyToView;
    studyToView.setStudyInstanceUID(studyInstanceUID);

    QList<DicomMask> studiesToView;
    studiesToView.append(studyToView);

    view(studiesToView, loadOnly);
}

void QInputOutputLocalDatabaseWidget::view(QList<DicomMask> dicomMaskStudiesToView, bool loadOnly)
{
    DicomMask patientToProcessMask;
    Patient *patient;
    QList<Patient*> selectedPatientsList;

    if (dicomMaskStudiesToView.isEmpty())
    {
        QMessageBox::warning(this, ApplicationNameString, tr("Select at least one study to view."));
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    foreach (DicomMask dicomMaskStudyToView, dicomMaskStudiesToView)
    {
        LocalDatabaseManager localDatabaseManager;

        patientToProcessMask.setStudyInstanceUID(dicomMaskStudyToView.getStudyInstanceUID());

        patient = localDatabaseManager.retrieve(patientToProcessMask);

        if (showDatabaseManagerError(localDatabaseManager.getLastError()))
        {
            QApplication::restoreOverrideCursor();
            return;
        }

        if (patient)
        {
            patient->setSelectedSeries(dicomMaskStudyToView.getSeriesInstanceUID());
            selectedPatientsList << patient;
        }
        else
        {
            DEBUG_LOG("No s'ha pogut obtenir l'estudi amb UID " + dicomMaskStudyToView.getStudyInstanceUID());
        }
    }

    if (selectedPatientsList.count() > 0)
    {
        DEBUG_LOG("Llançat signal per visualitzar estudi del pacient " + patient->getFullName());
        emit viewPatients(selectedPatientsList, loadOnly);
    }

    QApplication::restoreOverrideCursor();
}

void QInputOutputLocalDatabaseWidget::viewFromQStudyTreeWidget()
{
    QList<DicomMask> dicomMaskStudiesToView;
    QList<QPair<DicomMask, DICOMSource> > selectedDICOMItemsInQStudyTreeWidget = m_studyTreeWidget->getDicomMaskOfSelectedItems();

    for (int index = 0; index < selectedDICOMItemsInQStudyTreeWidget.count(); index++)
    {
        dicomMaskStudiesToView.append(selectedDICOMItemsInQStudyTreeWidget.at(index).first);
    }

    view(dicomMaskStudiesToView);
}

void QInputOutputLocalDatabaseWidget::viewFromQSeriesListWidget(QString studyInstanceUID, QString seriesInstanceUID)
{
    DicomMask studyToView;
    studyToView.setStudyInstanceUID(studyInstanceUID);
    studyToView.setSeriesInstanceUID(seriesInstanceUID);

    view(QList<DicomMask>() << studyToView);

    StatsWatcher::log("Obrim estudi seleccionant sèrie desde thumbnail");
}

// TODO en comptes de fer un signal cap a la queryscreen, perquè aquesta indiqui a la QInputOutPacsWidget que guardi un estudi al PACS,
// no hauria de ser aquesta funció l'encarregada de guardar l'estudi directament al PACS, entenc que no és responsabilitat de
// QInputOutputPacsWidget
void QInputOutputLocalDatabaseWidget::selectedStudiesStoreToPacs()
{
    if (m_studyTreeWidget->getDicomMaskOfSelectedItems().count() == 0)
    {
        QMessageBox::warning(this, ApplicationNameString, tr("Select at least one item to send to PACS."));
    }
    else
    {
        m_qwidgetSelectPacsToStoreDicomImage->show();
    }
}

void QInputOutputLocalDatabaseWidget::addSelectedStudiesToCreateDicomdirList()
{
    DicomMask studyMask;
    LocalDatabaseManager localDatabaseManager;
    QList<Patient*> patientList;
    QList<Study*> studies;
    QList<QPair<DicomMask, DICOMSource> > selectedDICOMItemsFromQStudyTreeWidget = m_studyTreeWidget->getDicomMaskOfSelectedItems();

    for (int index = 0; index < selectedDICOMItemsFromQStudyTreeWidget.count(); index++)
    {
        studyMask.setStudyInstanceUID(selectedDICOMItemsFromQStudyTreeWidget.at(index).first.getStudyInstanceUID());
        patientList = localDatabaseManager.queryPatientStudy(studyMask);
        if (showDatabaseManagerError(localDatabaseManager.getLastError()))
        {
            return;
        }

        // \TODO Això s'ha de fer perquè queryPatientStudy retorna llista de Patients
        // Nosaltres, en realitat, volem llista d'study amb les dades de Patient omplertes.
        if (patientList.size() != 1 && patientList.first()->getNumberOfStudies() != 1)
        {
            showDatabaseManagerError(LocalDatabaseManager::DatabaseCorrupted);
            return;
        }
        studies << patientList.first()->getStudies().first();
    }
    m_qcreateDicomdir->addStudies(studies);
}

// TODO: Aquesta responsabilitat d'esborrar els estudis vells al iniciar-se l'aplicació s'hauria de traslladar a un altre lloc, no és responsabilitat
//       d'aquesta inferfície
void QInputOutputLocalDatabaseWidget::deleteOldStudies()
{
    Settings settings;
    // Mirem si està activada la opció de la configuració d'esborrar els estudis vells no visualitzats en un número de dies determinat
    // fem la comprovació, per evitar engegar el thread si no s'han d'esborrar els estudis vells
    if (settings.getValue(InputOutputSettings::DeleteLeastRecentlyUsedStudiesInDaysCriteria).toBool())
    {
        m_qdeleteOldStudiesThread.deleteOldStudies();
    }
}

QList<Image*> QInputOutputLocalDatabaseWidget::getAllImagesFromPatient(Patient *patient)
{
    QList<Image*> images;

    foreach (Study *study, patient->getStudies())
    {
        foreach (Series *series, study->getSeries())
        {
            images.append(series->getImages());
        }
    }

    return images;
}

void QInputOutputLocalDatabaseWidget::deleteOldStudiesThreadFinished()
{
    showDatabaseManagerError(m_qdeleteOldStudiesThread.getLastError(), tr("deleting old studies"));
}

void QInputOutputLocalDatabaseWidget::qSplitterPositionChanged()
{
    Settings().saveGeometry(InputOutputSettings::LocalDatabaseSplitterState, m_StudyTreeSeriesListQSplitter);
}

void QInputOutputLocalDatabaseWidget::sendSelectedStudiesToSelectedPacs()
{
    foreach (PacsDevice pacsDevice, m_qwidgetSelectPacsToStoreDicomImage->getSelectedPacsToStoreDicomImages())
    {
        QList<QPair<DicomMask, DICOMSource> > dicomObjectsToSendToPACS = m_studyTreeWidget->getDicomMaskOfSelectedItems();

        for (int index = 0; index < dicomObjectsToSendToPACS.count(); index++)
        {
            DicomMask dicomMaskToSend = dicomObjectsToSendToPACS.at(index).first;
            LocalDatabaseManager localDatabaseManager;
            Patient *patient = localDatabaseManager.retrieve(dicomMaskToSend);

            if (localDatabaseManager.getLastError() != LocalDatabaseManager::Ok)
            {
                ERROR_LOG(QString("Error a la base de dades intentar obtenir els estudis que s'han d'enviar al PACS, Error: %1; StudyUID: %2")
                                  .arg(localDatabaseManager.getLastError())
                                  .arg(dicomMaskToSend.getStudyInstanceUID()));

                QString message = tr("An error occurred with database, preparing the DICOM files to send to PACS %1. The DICOM files won't be sent.")
                    .arg(pacsDevice.getAETitle());
                message += tr("\nClose all %1 windows and try again."
                         "\n\nIf the problem persists contact with an administrator.").arg(ApplicationNameString);
                QMessageBox::critical(this, ApplicationNameString, message);
            }
            else
            {
                sendDICOMFilesToPACS(pacsDevice, getAllImagesFromPatient(patient));
            }
        }
    }
}

void QInputOutputLocalDatabaseWidget::sendDICOMFilesToPACS(PacsDevice pacsDevice, QList<Image*> images)
{
    SendDICOMFilesToPACSJob *sendDICOMFilesToPACSJob = new SendDICOMFilesToPACSJob(pacsDevice, images);
    connect(sendDICOMFilesToPACSJob, SIGNAL(PACSJobFinished(PACSJob*)), SLOT(sendDICOMFilesToPACSJobFinished(PACSJob*)));
    m_pacsManager->enqueuePACSJob(sendDICOMFilesToPACSJob);
}

void QInputOutputLocalDatabaseWidget::sendDICOMFilesToPACSJobFinished(PACSJob *pacsJob)
{
    SendDICOMFilesToPACSJob *sendDICOMFilesToPACSJob = dynamic_cast<SendDICOMFilesToPACSJob*> (pacsJob);

    if (sendDICOMFilesToPACSJob->getStatus() != PACSRequestStatus::SendOk)
    {
        if (sendDICOMFilesToPACSJob->getStatus() == PACSRequestStatus::SendWarningForSomeImages ||
            sendDICOMFilesToPACSJob->getStatus() == PACSRequestStatus::SendSomeDICOMFilesFailed)
        {
            QMessageBox::warning(this, ApplicationNameString, sendDICOMFilesToPACSJob->getStatusDescription());
        }
        else
        {
            QMessageBox::critical(this, ApplicationNameString, sendDICOMFilesToPACSJob->getStatusDescription());
        }
    }

    // Com que l'objecte és un punter altres classes poden haver capturat el Signal per això li fem un deleteLater() en comptes d'un delete, per evitar
    // que quan responguin al signal es trobin que l'objecte ja no existeix. L'objecte serà destruït per Qt quan es retorni el eventLoop
    pacsJob->deleteLater();
}

void QInputOutputLocalDatabaseWidget::newPACSJobEnqueued(PACSJob *pacsJob)
{
    // Connectem amb el signal RetrieveDICOMFilesFromPACSJob de que s'esborrarà un estudi de la caché per treure'ls de la QStudyTreeWidget quan se
    // n'esborrin
    // TODO: RetrieveDICOMFilesFromPACS no hauria d'emetre aquest signal, hauria de ser una CacheManager d'aquesta manera treuriem la responsabilitat
    //       de RetrieveDICOMFilesFromPACS de fer-ho, i a més no caldria connectar el signal cada vegada que fan un nou Job. Una vegada s'hagi implementar la
    //       CacheManager aquest mètode HA DE DESAPAREIXER, quan es tregui aquest mètode recordar a treure l'include a "retrievedicomfilesfrompacsjob.h"
    if (pacsJob->getPACSJobType() == PACSJob::RetrieveDICOMFilesFromPACSJobType)
    {
        connect(dynamic_cast<RetrieveDICOMFilesFromPACSJob*> (pacsJob), SIGNAL(studyFromCacheWillBeDeleted(QString)),
                SLOT(removeStudyFromQStudyTreeWidget(QString)));
    }
}

bool QInputOutputLocalDatabaseWidget::showDatabaseManagerError(LocalDatabaseManager::LastError error, const QString &doingWhat)
{
    QString message;

    if (!doingWhat.isEmpty())
    {
        message = tr("An error has occurred while ") + doingWhat + ":\n\n";
    }

    switch (error)
    {
        case LocalDatabaseManager::Ok:
            return false;
        case LocalDatabaseManager::DatabaseLocked:
            message += tr("The database is blocked by another process.");
            message += tr("\nClose all %1 windows and try again."
                         "\n\nIf the problem persists contact with an administrator.").arg(ApplicationNameString);
            break;
        case LocalDatabaseManager::DatabaseCorrupted:
            message += tr("Database is corrupted.");
            message += tr("\nClose all %1 windows and try again."
                         "\n\nIf the problem persists contact with an administrator.").arg(ApplicationNameString);
            break;
        case LocalDatabaseManager::SyntaxErrorSQL:
            message += tr("Database syntax error.");
            message += tr("\nClose all %1 windows and try again."
                         "\n\nIf the problem persists contact with an administrator.").arg(ApplicationNameString);
            break;
        case LocalDatabaseManager::DatabaseError:
            message += tr("An internal database error occurred.");
            message += tr("\nClose all %1 windows and try again."
                         "\n\nIf the problem persists contact with an administrator.").arg(ApplicationNameString);
            break;
        case LocalDatabaseManager::DeletingFilesError:
            message += tr("Some files cannot be deleted."
                         "\nThese files have to be deleted manually.");
            break;
        default:
            message = tr("Unknown error.");
            break;
    }

    QApplication::restoreOverrideCursor();

    QMessageBox::critical(this, ApplicationNameString, message);

    return true;
}

};
