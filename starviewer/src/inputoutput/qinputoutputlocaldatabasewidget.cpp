/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qinputoutputlocaldatabasewidget.h"

#include <QMessageBox>
#include <QShortcut>

#include "status.h"
#include "logging.h"
#include "starviewerapplication.h"
#include "dicommask.h"
#include "patient.h"
#include "statswatcher.h"
#include "qcreatedicomdir.h"
#include "inputoutputsettings.h"
#include "qwidgetselectpacstostoredicomimage.h"

namespace udg
{

QInputOutputLocalDatabaseWidget::QInputOutputLocalDatabaseWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);

    //esborrem els estudis vells de la cache
    deleteOldStudies();
    createContextMenuQStudyTreeWidget();

    Settings settings;
    settings.restoreColumnsWidths(InputOutputSettings::LocalDatabaseStudyList, m_studyTreeWidget->getQTreeWidget() );
    settings.restoreGeometry(InputOutputSettings::LocalDatabaseSplitterState, m_StudyTreeSeriesListQSplitter );

    m_statsWatcher = new StatsWatcher("QueryInputOutputLocalDatabaseWidget",this);
    m_statsWatcher->addClicksCounter(m_viewButton);

    //Indiquem que el QStudyTreeWidget inicialment s'ordenarà pel la columna name
    m_studyTreeWidget->setSortColumn(QStudyTreeWidget::ObjectName);
    m_studyTreeWidget->setMaximumExpandTreeItemsLevel(QStudyTreeWidget::SeriesLevel);

    m_qwidgetSelectPacsToStoreDicomImage = new QWidgetSelectPacsToStoreDicomImage();

    createConnections();
}

QInputOutputLocalDatabaseWidget::~QInputOutputLocalDatabaseWidget()
{
    Settings().saveColumnsWidths(InputOutputSettings::LocalDatabaseStudyList, m_studyTreeWidget->getQTreeWidget() );
}

void QInputOutputLocalDatabaseWidget::createConnections()
{
    connect(m_studyTreeWidget, SIGNAL(studyExpanded(QString)), SLOT(expandSeriesOfStudy(QString)));
    connect(m_studyTreeWidget, SIGNAL(seriesExpanded(QString, QString)), SLOT(expandImagesOfSeries(QString, QString)));

    connect(m_studyTreeWidget, SIGNAL(studyDoubleClicked()), SLOT(viewFromQStudyTreeWidget()));
    connect(m_studyTreeWidget, SIGNAL(seriesDoubleClicked()), SLOT(viewFromQStudyTreeWidget()));
    connect(m_studyTreeWidget, SIGNAL(imageDoubleClicked()), SLOT(viewFromQStudyTreeWidget()));

    connect(m_viewButton, SIGNAL(clicked()), SLOT(viewFromQStudyTreeWidget()));

    connect(m_seriesListWidget, SIGNAL(selectedSeriesIcon(QString)), m_studyTreeWidget, SLOT(setCurrentSeries(QString)));
    connect(m_seriesListWidget, SIGNAL(viewSeriesIcon()), SLOT(viewFromQSeriesListWidget()));
    connect(m_studyTreeWidget, SIGNAL(currentStudyChanged()), SLOT(setSeriesToSeriesListWidget()));
    connect(m_studyTreeWidget, SIGNAL(currentSeriesChanged(QString)), m_seriesListWidget, SLOT(setCurrentSeries(QString)));
    //Si passem de tenir un element seleccionat a no tenir-ne li diem al seriesListWidget que no mostri cap previsualització
    connect(m_studyTreeWidget, SIGNAL(notCurrentItemSelected()), m_seriesListWidget, SLOT(clear()));

    //Connecta amb el signal que indica que ha finalitza el thread d'esborrar els estudis vells
    connect(&m_qdeleteOldStudiesThread, SIGNAL(finished()), SLOT(deleteOldStudiesThreadFinished()));

    ///Si movem el QSplitter capturem el signal per guardar la seva posició
    connect(m_StudyTreeSeriesListQSplitter, SIGNAL(splitterMoved (int, int)), SLOT(qSplitterPositionChanged()));
    connect(m_qwidgetSelectPacsToStoreDicomImage, SIGNAL(selectedPacsToStore()), SLOT(storeSelectedStudyiesToSelectedPacs()));
}

void QInputOutputLocalDatabaseWidget::createContextMenuQStudyTreeWidget()
{
    QAction *action;

    action = m_contextMenuQStudyTreeWidget.addAction(QIcon(":/images/view.png"), tr("&View"), this, SLOT(viewFromQStudyTreeWidget()), tr("Ctrl+V"));
    (void) new QShortcut(action->shortcut(), this, SLOT(viewFromQStudyTreeWidget()));

    action = m_contextMenuQStudyTreeWidget.addAction(QIcon(":/images/databaseRemove.png"), tr("&Delete"), this, SLOT(deleteSelectedItemsFromLocalDatabase()), Qt::Key_Delete);
    (void) new QShortcut(action->shortcut(), this, SLOT(deleteSelectedItemsFromLocalDatabase()));

#ifndef STARVIEWER_LITE
    action = m_contextMenuQStudyTreeWidget.addAction(tr("Send to DICOMDIR List"), this, SLOT(addSelectedStudiesToCreateDicomdirList()), tr("Ctrl+M"));
    (void) new QShortcut(action->shortcut(), this, SLOT(addSelectedStudiesToCreateDicomdirList()));

    action = m_contextMenuQStudyTreeWidget.addAction(QIcon(":/images/store.png"), tr("Send to PACS"), this, SLOT(selectedStudiesStoreToPacs()), tr("Ctrl+S"));
    (void) new QShortcut(action->shortcut(), this, SLOT(selectedStudiesStoreToPacs()));
#endif
    m_studyTreeWidget->setContextMenu(&m_contextMenuQStudyTreeWidget);//Especifiquem que és el menú per la cache
}

//TODO s'hauria buscar una manera més elegant de comunicar les dos classes, fer un singletton de QCreateDicomdir ?
void QInputOutputLocalDatabaseWidget::setQCreateDicomdir(QCreateDicomdir * qcreateDicomdir)
{
    m_qcreateDicomdir = qcreateDicomdir;
}

void QInputOutputLocalDatabaseWidget::clear()
{
    m_studyTreeWidget->clear();
    m_seriesListWidget->clear();
}

void QInputOutputLocalDatabaseWidget::queryStudy(DicomMask queryMask)
{
    LocalDatabaseManager localDatabaseManager;
    QList<Patient *> patientStudyList;

    StatsWatcher::log("Cerca d'estudis a la base de dades local amb paràmetres: " + queryMask.getFilledMaskFields());
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    m_studyTreeWidget->clear();
    m_seriesListWidget->clear();

    patientStudyList = localDatabaseManager.queryPatientStudy(queryMask);

    if (showDatabaseManagerError(localDatabaseManager.getLastError()))    return;

    /* Aquest mètode a part de ser cridada quan l'usuari fa click al botó search, també es cridada al
     * constructor d'aquesta classe, per a que al engegar l'aplicació ja es mostri la llista d'estudis
     * que hi ha a la base de dades local. Si el mètode no troba cap estudi a la base de dades local
     * es llença el missatge que no s'han trobat estudis, però com que no és idonii, en el cas aquest que es
     * crida des del constructor que es mostri el missatge de que no s'han trobat estudis al engegar l'aplicació, el que
     * es fa és que per llançar el missatge es comprovi que la finestra estigui activa. Si la finestra no està activa
     * vol dir que el mètode ha estat invocat des del constructor
     */
    if (patientStudyList.isEmpty() && isActiveWindow())
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::information(this, ApplicationNameString, tr("No study match found."));
    }
    else
    {
        m_studyTreeWidget->insertPatientList(patientStudyList);//es mostra la llista d'estudis
        QApplication::restoreOverrideCursor();
    }
}

void QInputOutputLocalDatabaseWidget::addStudyToQStudyTreeWidget(QString studyUID)
{
    LocalDatabaseManager localDatabaseManager;
    DicomMask studyMask;
    QList<Patient*> patientList;

    studyMask.setStudyUID(studyUID);
    patientList = localDatabaseManager.queryPatientStudy(studyMask);
    if(showDatabaseManagerError(localDatabaseManager.getLastError()))    return;

    if (patientList.count() == 1)
    {
        m_studyTreeWidget->insertPatient(patientList.at(0));
        m_studyTreeWidget->sort();
    }
}

void QInputOutputLocalDatabaseWidget::removeStudyFromQStudyTreeWidget(QString studyInstanceUID)
{
    m_studyTreeWidget->removeStudy(studyInstanceUID);
}

void QInputOutputLocalDatabaseWidget::expandSeriesOfStudy(QString studyInstanceUID)
{
    QList<Series*> seriesList;
    LocalDatabaseManager localDatabaseManager;
    DicomMask mask;

    INFO_LOG("Cerca de sèries a la font cache de l'estudi " + studyInstanceUID);

    //preparem la mascara i cerquem les series a la cache
    mask.setStudyUID(studyInstanceUID);
    seriesList = localDatabaseManager.querySeries(mask);

    if (showDatabaseManagerError(localDatabaseManager.getLastError())) return;

    if (seriesList.isEmpty())
    {
        QMessageBox::information(this, ApplicationNameString, tr("No series match for this study.\n"));
        return;
    }
    else m_studyTreeWidget->insertSeriesList(studyInstanceUID, seriesList); //inserim la informació de les sèries al estudi
}

void QInputOutputLocalDatabaseWidget::expandImagesOfSeries(QString studyInstanceUID, QString seriesInstanceUID)
{
    LocalDatabaseManager localDatabaseManager;
    DicomMask mask;
    QList<Image*> imageList;

    INFO_LOG("Cerca d'imatges a la font cache de l'estudi " + studyInstanceUID + " i serie " + seriesInstanceUID);

    mask.setStudyUID(studyInstanceUID);
    mask.setSeriesUID(seriesInstanceUID);
    imageList = localDatabaseManager.queryImage(mask);

    if(showDatabaseManagerError(localDatabaseManager.getLastError()))   return;

    if (imageList.isEmpty())
    {
        QMessageBox::information(this, ApplicationNameString, tr("No images match for this study.\n"));
        return;
    }
    else m_studyTreeWidget->insertImageList(studyInstanceUID, seriesInstanceUID, imageList);
}

void QInputOutputLocalDatabaseWidget::setSeriesToSeriesListWidget()
{
    QList<Series*> seriesList;
    LocalDatabaseManager localDatabaseManager;
    DicomMask mask;
    QString studyInstanceUID = m_studyTreeWidget->getCurrentStudyUID();

    INFO_LOG("Cerca de sèries a la cache de l'estudi " + studyInstanceUID);

    //preparem la mascara i cerquem les series a la cache
    mask.setStudyUID(studyInstanceUID);

    seriesList = localDatabaseManager.querySeries(mask);
    if (showDatabaseManagerError(localDatabaseManager.getLastError()))    return;

    m_seriesListWidget->clear();

    foreach(Series* series, seriesList)
    {
        m_seriesListWidget->insertSeries(studyInstanceUID, series);
    }
}

void QInputOutputLocalDatabaseWidget::deleteSelectedItemsFromLocalDatabase()
{
    QList<DicomMask> selectedDicomMaskToDelete = m_studyTreeWidget->getDicomMaskOfSelectedItems();

    if(!selectedDicomMaskToDelete.isEmpty())
    {
        QMessageBox::StandardButton response = QMessageBox::question(this, ApplicationNameString,
                                                                           tr("Are you sure you want to delete the selected Items?"),
                                                                           QMessageBox::Yes | QMessageBox::No,
                                                                           QMessageBox::No);
        if (response  == QMessageBox::Yes)
        {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            LocalDatabaseManager localDatabaseManager;

            foreach(DicomMask dicomMaskToDelete, selectedDicomMaskToDelete)
            {
                if(m_qcreateDicomdir->studyExists(dicomMaskToDelete.getStudyUID()))
                {
                    Study *studyToDelete = m_studyTreeWidget->getStudy(dicomMaskToDelete.getStudyUID());
                    QString warningMessage;
                    
                    if (dicomMaskToDelete.getSeriesUID().isEmpty())
                    {
                        warningMessage = tr("The study %1 of patient %2 is in use by the DICOMDIR List. If you want to delete "
                                            "this study you should remove it from the DICOMDIR List first.")
                                         .arg(studyToDelete->getID() , studyToDelete->getParentPatient()->getFullName());
                    }
                    else
                    {   //TODO:Hauriem de mostar el Series ID en lloc del Series UID
                        warningMessage = tr("The series with UID %1 of study %2 patient %3 is in use by the DICOMDIR List. If you want to delete "
                                            "this series you should remove the study from the DICOMDIR List first.")
                                         .arg(dicomMaskToDelete.getSeriesUID(), studyToDelete->getID(), studyToDelete->getParentPatient()->getFullName());
                    }

                    QMessageBox::warning(this, ApplicationNameString, warningMessage);
                }
                else
                {
                    if (!dicomMaskToDelete.getSeriesUID().isEmpty())
                    {
                        localDatabaseManager.deleteSeries(dicomMaskToDelete.getStudyUID(), dicomMaskToDelete.getSeriesUID());

                        m_seriesListWidget->removeSeries(dicomMaskToDelete.getSeriesUID());
                        m_studyTreeWidget->removeSeries(dicomMaskToDelete.getStudyUID(), dicomMaskToDelete.getSeriesUID());
                    }
                    else
                    {
                        localDatabaseManager.deleteStudy(dicomMaskToDelete.getStudyUID());

                        m_seriesListWidget->clear();
                        m_studyTreeWidget->removeStudy(dicomMaskToDelete.getStudyUID());
                    }
                    
                    if (showDatabaseManagerError(localDatabaseManager.getLastError()))
                    break;
                }
            }
            QApplication::restoreOverrideCursor();
        }
    }
    else QMessageBox::information(this, ApplicationNameString, tr("Please select at least one study to delete"));
}

void QInputOutputLocalDatabaseWidget::view(QStringList selectedStudiesInstanceUID, QString selectedSeriesInstanceUID, bool loadOnly )
{
    DicomMask patientToProcessMask;
    Patient *patient;
    QList<Patient *> selectedPatientsList;

    if(selectedStudiesInstanceUID.isEmpty())
    {
        QMessageBox::warning(this, ApplicationNameString, tr("Select at least one study to view"));
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    //TODO: S'hauria de millorar el mètode ja que per la seva estructura lo d'obrir l'estudi per la sèrie que ens tinguin seleccionada només ho farà per un estudi ja que aquest mètode només se li passa per paràmetre una sèrie per defecte
    foreach(QString studyInstanceUIDSelected, selectedStudiesInstanceUID)
    {
        LocalDatabaseManager localDatabaseManager;

        patientToProcessMask.setStudyUID(studyInstanceUIDSelected);

        patient = localDatabaseManager.retrieve(patientToProcessMask);

        if(showDatabaseManagerError(localDatabaseManager.getLastError()))
        {
            QApplication::restoreOverrideCursor();
            return;
        }

        if(patient)
        {
            // Marquem la sèrie per defecte
            // TODO ara sempre posem el mateix UID, per tant de moment només funciona bé del tot quan seleccionem un únic estudi
            patient->setSelectedSeries(selectedSeriesInstanceUID);
            selectedPatientsList << patient;
        }
        else DEBUG_LOG("No s'ha pogut obtenir l'estudi amb UID " + studyInstanceUIDSelected);
    }

    if (selectedPatientsList.count() > 0)
    {
        DEBUG_LOG("Llançat signal per visualitzar estudi del pacient " + patient->getFullName());
        emit viewPatients(Patient::mergePatients(selectedPatientsList), loadOnly );
    }

    QApplication::restoreOverrideCursor();
}

void QInputOutputLocalDatabaseWidget::viewFromQStudyTreeWidget()
{
    view(m_studyTreeWidget->getSelectedStudiesUID(), m_studyTreeWidget->getCurrentSeriesUID());
}

void QInputOutputLocalDatabaseWidget::viewFromQSeriesListWidget()
{
    QStringList selectedStudiesInstanceUID;

    selectedStudiesInstanceUID << m_seriesListWidget->getCurrentStudyUID();//Agafem l'estudi uid de la sèrie seleccionada
    view(selectedStudiesInstanceUID, m_seriesListWidget->getCurrentSeriesUID());

    StatsWatcher::log("Obrim estudi seleccionant sèrie desde thumbnail");
}

/*TODO en comptes de fer un signal cap a la queryscreen, perquè aquesta indiqui a la QInputOutPacsWidget que guardi un estudi al PACS
 *, no hauria de ser aquesta funció l'encarregada de guardar l'estudi directament al PACS, entenc que no és responsabilitat de 
 * QInputOutputPacsWidget
 */
void QInputOutputLocalDatabaseWidget::selectedStudiesStoreToPacs()
{
    if (m_studyTreeWidget->getSelectedStudies().count() == 0)
    {
        QMessageBox::warning(this, ApplicationNameString, tr("Select at least one study to send to PACS."));
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
    QList<Study *> studies;
    
    foreach(QString studyUID, m_studyTreeWidget->getSelectedStudiesUID())
    {
        studyMask.setStudyUID(studyUID);
        patientList = localDatabaseManager.queryPatientStudy(studyMask);
        if(showDatabaseManagerError(localDatabaseManager.getLastError())) 
            return;

        // \TODO Això s'ha de fer perquè queryPatientStudy retorna llista de Patients
        // Nosaltres, en realitat, volem llista d'study amb les dades de Patient omplertes.
        if(patientList.size() != 1 && patientList.first()->getNumberOfStudies() != 1)
        {
            showDatabaseManagerError(LocalDatabaseManager::DatabaseCorrupted);
            return;
        }
        studies << patientList.first()->getStudies().first();
   }
    m_qcreateDicomdir->addStudies(studies);
}

/*TODO: Aquesta responsabilitat d'esborrar els estudis vells al iniciar-se l'aplicació s'hauria de traslladar a un altre lloc, no és responsabilitat
        d'aquesta inferfície */
void QInputOutputLocalDatabaseWidget::deleteOldStudies()
{
    Settings settings;
    /*Mirem si està activada la opció de la configuració d'esborrar els estudis vells no visualitzats en un número de dies determinat
      fem la comprovació, per evitar engegar el thread si no s'han d'esborrar els estudis vells*/
    if (settings.getValue(InputOutputSettings::DeleteLeastRecentlyUsedStudiesInDaysCriteria).toBool())
    {
        m_qdeleteOldStudiesThread.deleteOldStudies();
    }
}

void QInputOutputLocalDatabaseWidget::deleteOldStudiesThreadFinished()
{
    showDatabaseManagerError(m_qdeleteOldStudiesThread.getLastError(), tr("deleting old studies"));
}

void QInputOutputLocalDatabaseWidget::qSplitterPositionChanged()
{
    Settings().saveGeometry(InputOutputSettings::LocalDatabaseSplitterState, m_StudyTreeSeriesListQSplitter );
}

void QInputOutputLocalDatabaseWidget::storeSelectedStudyiesToSelectedPacs()
{
    foreach(PacsDevice pacsDevice, m_qwidgetSelectPacsToStoreDicomImage->getSelectedPacsToStoreDicomImages())
    {
        foreach(DicomMask dicomMask, m_studyTreeWidget->getDicomMaskOfSelectedItems())
        {
            emit storeDicomObjectsToPacs(pacsDevice, m_studyTreeWidget->getStudy(dicomMask.getStudyUID()), dicomMask);
        }
    }
}

bool QInputOutputLocalDatabaseWidget::showDatabaseManagerError(LocalDatabaseManager::LastError error, const QString &doingWhat)
{
    QString message;

    if (!doingWhat.isEmpty())
        message = tr("An error has ocurred while ") + doingWhat + ":\n\n";

    switch(error)
    {
        case LocalDatabaseManager::Ok:
            return false;

        case LocalDatabaseManager::DatabaseLocked:
            message += tr("The database is blocked by another %1 window."
                         "\nClose all the others %1 windows and try again.").arg(ApplicationNameString);
            break;
        case LocalDatabaseManager::DatabaseCorrupted:
			message += tr("%1 database is corrupted.").arg(ApplicationNameString);
            message += tr("\nClose all %1 windows and try again."
                         "\n\nIf the problem persist contact with an administrator.").arg(ApplicationNameString);
            break;
        case LocalDatabaseManager::SyntaxErrorSQL:
            message += tr("%1 database syntax error.").arg(ApplicationNameString);
            message += tr("\nClose all %1 windows and try again."
                         "\n\nIf the problem persist contact with an administrator.").arg(ApplicationNameString);
            break;
        case LocalDatabaseManager::DatabaseError:
            message += tr("An internal error occurs with %1 database.").arg(ApplicationNameString);
            message += tr("\nClose all %1 windows and try again."
                         "\n\nIf the problem persist contact with an administrator.").arg(ApplicationNameString);
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
