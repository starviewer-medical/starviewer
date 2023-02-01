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

#include "qinputoutputdicomdirwidget.h"

#include "dicommask.h"
#include "harddiskinformation.h"
#include "inputoutputsettings.h"
#include "localdatabasemanager.h"
#include "logging.h"
#include "messagebus.h"
#include "patient.h"
#include "shortcutmanager.h"
#include "starviewerapplication.h"
#include "statswatcher.h"
#include "status.h"
#include "study.h"
#include "usermessage.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QShortcut>

namespace udg {

QInputOutputDicomdirWidget::QInputOutputDicomdirWidget(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);

    createConnections();
    createContextMenuQStudyTreeWidget();

    Settings settings;
    settings.restoreColumnsWidths(InputOutputSettings::DICOMDIRStudyListColumnsWidth, m_studyTreeWidget->getQTreeWidget());

    QStudyTreeWidget::ColumnIndex sortByColumn = (QStudyTreeWidget::ColumnIndex) settings.getValue(InputOutputSettings::DICOMDIRStudyListSortByColumn).toInt();
    Qt::SortOrder sortOrderColumn = (Qt::SortOrder) settings.getValue(InputOutputSettings::DICOMDIRStudyListSortOrder).toInt();
    m_studyTreeWidget->setSortByColumn (sortByColumn, sortOrderColumn);

    m_statsWatcher = new StatsWatcher("QueryInputOutputDicomdirWidget", this);
    m_statsWatcher->addClicksCounter(m_viewButton);
    m_statsWatcher->addClicksCounter(m_retrieveButton);
    m_statsWatcher->addClicksCounter(m_openDICOMDIRPushButton);
}

QInputOutputDicomdirWidget::~QInputOutputDicomdirWidget()
{
    Settings settings;
    settings.saveColumnsWidths(InputOutputSettings::DICOMDIRStudyListColumnsWidth, m_studyTreeWidget->getQTreeWidget());

    // Guardem per quin columna està ordenada la llista d'estudis i en quin ordre
    settings.setValue(InputOutputSettings::DICOMDIRStudyListSortByColumn, m_studyTreeWidget->getSortColumn());
    settings.setValue(InputOutputSettings::DICOMDIRStudyListSortOrder, m_studyTreeWidget->getSortOrderColumn());
}

void QInputOutputDicomdirWidget::createConnections()
{
    connect (m_openDICOMDIRPushButton, SIGNAL(clicked()), SLOT(openDicomdir()));

    connect(m_studyTreeWidget, SIGNAL(requestedSeriesOfStudy(Study*)), SLOT(requestedSeriesOfStudy(Study*)));
    connect(m_studyTreeWidget, SIGNAL(requestedImagesOfSeries(Series*)), SLOT(requestedImagesOfSeries(Series*)));

    connect(m_studyTreeWidget, SIGNAL(studyDoubleClicked()), SLOT(view()));
    connect(m_studyTreeWidget, SIGNAL(seriesDoubleClicked()), SLOT(view()));
    connect(m_studyTreeWidget, SIGNAL(imageDoubleClicked()), SLOT(view()));
    connect(m_viewButton, SIGNAL(clicked()), SLOT(view()));
    connect(m_retrieveButton, SIGNAL(clicked()), SLOT(retrieveSelectedStudies()));
}

void QInputOutputDicomdirWidget::createContextMenuQStudyTreeWidget()
{
    QAction *action;

    action = m_contextMenuQStudyTreeWidget.addAction(QIcon(":/images/icons/visibility.svg"), tr("&View"), this, SLOT(view()));
    action->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::ViewSelectedStudies));
    (void) new QShortcut(action->shortcut(), this, SLOT(view()));

    action = m_contextMenuQStudyTreeWidget.addAction(QIcon(":/images/icons/document-import.svg"), tr("&Import"), this, SLOT(retrieveSelectedStudies()));
    action->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::ImportToLocalDatabaseSelectedDICOMDIRStudies));
    (void) new QShortcut(action->shortcut(), this, SLOT(retrieveSelectedStudies()));

    // Especifiquem que es el menu del dicomdir
    m_studyTreeWidget->setContextMenu(& m_contextMenuQStudyTreeWidget);
}

bool QInputOutputDicomdirWidget::openDicomdir()
{
    Settings settings;
    QString dicomdirPath;
    Status state;
    bool ok = false;
    // L'asterisc abans de DICOMDIR i dicomdir hi és per compatibilitat amb Mac.
    dicomdirPath = QFileDialog::getOpenFileName(0, tr("Open"), settings.getValue(InputOutputSettings::LastOpenedDICOMDIRPath).toString(),
                                                "DICOMDIR (*DICOMDIR *dicomdir)");

    // Si és buit no ens han seleccionat cap fitxer
    if (!dicomdirPath.isEmpty())
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        // Obrim el dicomdir
        state = m_readDicomdir.open (dicomdirPath);
        QApplication::restoreOverrideCursor();
        if (!state.good())
        {
            QMessageBox::warning(this, ApplicationNameString, tr("Error opening DICOMDIR"));
            ERROR_LOG("Error al obrir el DICOMDIR " + dicomdirPath + state.text());
        }
        else
        {
            INFO_LOG("Obert el DICOMDIR " + dicomdirPath);
            settings.setValue(InputOutputSettings::LastOpenedDICOMDIRPath, QFileInfo(dicomdirPath).dir().path());
            ok = true;
            // Cerquem els estudis al dicomdir per a que es mostrin
            // Netegem el filtre de cerca al obrir el dicomdir
            emit clearSearchTexts();
            queryStudy(DicomMask());
        }
    }

    return ok;
}

void QInputOutputDicomdirWidget::queryStudy(DicomMask queryMask)
{
    QList<Patient*> patientStudyList;
    Status state;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    m_studyTreeWidget->clear();

    state = m_readDicomdir.readStudies(patientStudyList, queryMask);
    if (!state.good())
    {
        QApplication::restoreOverrideCursor();
        if (state.code() == 1302)
        {
            // Aquest és l'error quan no tenim un dicomdir obert l'ig
            QMessageBox::warning(this, ApplicationNameString, tr("You must open a DICOMDIR before searching."));
            ERROR_LOG("No s'ha obert cap directori DICOMDIR " + state.text());
        }
        else
        {
            QMessageBox::warning(this, ApplicationNameString, tr("Error querying in DICOMDIR"));
            ERROR_LOG("Error cercant estudis al DICOMDIR " + state.text());
        }
        return;
    }

    if (patientStudyList.isEmpty())
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::information(this, ApplicationNameString, tr("No study match found."));
    }
    else
    {
        m_studyTreeWidget->insertPatientList(patientStudyList);
        QApplication::restoreOverrideCursor();
    }
}

void QInputOutputDicomdirWidget::clear()
{
    m_studyTreeWidget->clear();
}

void QInputOutputDicomdirWidget::requestedSeriesOfStudy(Study *study)
{
    QList<Series*> seriesList;

    INFO_LOG("Cerca de sèries al DICOMDIR de l'estudi " + study->getInstanceUID());

    // "" pq no busquem cap serie en concret
    m_readDicomdir.readSeries(study->getInstanceUID(), "", seriesList);

    if (seriesList.isEmpty())
    {
        QMessageBox::information(this, ApplicationNameString, tr("No series match for this study.") + "\n");
    }
    else
    {
        // Inserim la informació de la sèrie al llistat
        m_studyTreeWidget->insertSeriesList(study->getInstanceUID(), seriesList);
    }
}

void QInputOutputDicomdirWidget::requestedImagesOfSeries(Series *series)
{
    QList<Image*> imageList;

    INFO_LOG("Cerca d'imatges al DICOMDIR de l'estudi " + series->getParentStudy()->getInstanceUID() + " i serie " + series->getInstanceUID());

    m_readDicomdir.readImages(series->getInstanceUID(), "", imageList);

    if (imageList.isEmpty())
    {
        QMessageBox::information(this, ApplicationNameString, tr("No images match for this series.") + "\n");
        return;
    }
    else
    {
        m_studyTreeWidget->insertImageList(series->getParentStudy()->getInstanceUID(), series->getInstanceUID(), imageList);
    }
}

void QInputOutputDicomdirWidget::retrieveSelectedStudies()
{
    DICOMDIRImporter importDicom;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // TODO ara només permetrem importar estudis sencers

    QList<QPair<DicomMask, DICOMSource> > dicomMaskDICOMSourceList = m_studyTreeWidget->getDicomMaskOfSelectedItems();

    for (int index = 0; index < dicomMaskDICOMSourceList.count(); index++)
    {
        DicomMask dicomMaskToRetrieve = dicomMaskDICOMSourceList.at(index).first;

        importDicom.import(m_readDicomdir.getDicomdirFilePath(), dicomMaskToRetrieve.getStudyInstanceUID(), dicomMaskToRetrieve.getSeriesInstanceUID(),
                           dicomMaskToRetrieve.getSOPInstanceUID());
        if (importDicom.getLastError() != DICOMDIRImporter::Ok)
        {
            // S'ha produït un error
            QApplication::restoreOverrideCursor();
            showDICOMDIRImporterError(dicomMaskToRetrieve.getStudyInstanceUID(), importDicom.getLastError());
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

            if (importDicom.getLastError() != DICOMDIRImporter::PatientInconsistent &&
                importDicom.getLastError() != DICOMDIRImporter::DicomdirInconsistent)
            {
                // Si es produeix qualsevol dels altres errors parem d'importar estudis, perquè segurament les següents importacions també fallaran
                break;
            }
        }
        else
        {
            MessageBus::instance()->send("Database/StudyInserted", dicomMaskToRetrieve.getStudyInstanceUID());
        }
    }

    QApplication::restoreOverrideCursor();
}

void QInputOutputDicomdirWidget::view()
{
    QList<QPair<DicomMask, DICOMSource> > selectedDICOMITems = m_studyTreeWidget->getDicomMaskOfSelectedItems();
    DicomMask patientToProcessMask;
    Patient *patient;
    QList<Patient*> selectedPatientsList;

    if (selectedDICOMITems.isEmpty())
    {
        QMessageBox::warning(this, ApplicationNameString, tr("Select at least one study to view."));
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    for (int index = 0; index < selectedDICOMITems.count(); index++)
    {
        patientToProcessMask.setStudyInstanceUID(selectedDICOMITems.at(index).first.getStudyInstanceUID());
        patient = m_readDicomdir.retrieve(patientToProcessMask);

        if (patient)
        {
            patient->setSelectedSeries(selectedDICOMITems.at(index).first.getSeriesInstanceUID());
            selectedPatientsList << patient;
        }
        else
        {
            DEBUG_LOG("No s'ha pogut obtenir l'estudi amb UID " + selectedDICOMITems.at(index).first.getStudyInstanceUID());
        }
    }

    QApplication::restoreOverrideCursor();

    if (!selectedPatientsList.isEmpty())
    {
        DEBUG_LOG("Llançat signal per visualitzar estudi del pacient " + patient->getFullName());
        emit viewPatients(selectedPatientsList);
    }
    else
    {
        QMessageBox::warning(this, ApplicationNameString, tr("No valid data found. Maybe some files are missing or corrupted."));
    }
}

void QInputOutputDicomdirWidget::showDICOMDIRImporterError(QString studyInstanceUID, DICOMDIRImporter::DICOMDIRImporterError error)
{
    QString message;

    switch (error)
    {
        case DICOMDIRImporter::ErrorOpeningDicomdir:
            message = tr("Tried to import study with UID %1 ").arg(studyInstanceUID);
            message += tr("but the DICOMDIR file could not be opened, make sure that its path is correct.");
            message += "\n\n\n";
            message += UserMessage::getProblemPersistsAdvice();
            QMessageBox::critical(this, ApplicationNameString, message);
            break;
        case DICOMDIRImporter::ErrorCopyingFiles:
            message = tr("Some files of the study with UID %2 could not be imported. Make sure you have write permission on the %1 cache directory.")
                    .arg(ApplicationNameString, studyInstanceUID);
            message += "\n\n";
            message += UserMessage::getProblemPersistsAdvice();
            QMessageBox::critical(this, ApplicationNameString, message);
            break;
        case DICOMDIRImporter::NoEnoughSpace:
            {
                HardDiskInformation hardDiskInformation;
                Settings settings;
                quint64 freeSpaceInHardDisk = hardDiskInformation.getNumberOfFreeMBytes(LocalDatabaseManager::getCachePath());
                quint64 minimumFreeSpaceRequired = quint64(settings.getValue(InputOutputSettings::MinimumFreeGigaBytesForCache).toULongLong() * 1024);
                message = tr("The study cannot be imported because there is not enough space.");
                message += "\n";
                message += tr("Try to free hard disk space, delete local studies or change in %1 configuration the size of reserved disk space for "
                              "system to be able to import the study.").arg(ApplicationNameString);
                message += "\n\n";
                message += tr("Current status:");
                message += "\n";
                message += "    " + tr("* Disk space reserved for the system: %1 GB").arg(minimumFreeSpaceRequired / 1024.0);
                message += "\n";
                message += "    " + tr("* Free disk space: %1 GB").arg(freeSpaceInHardDisk / 1024.0);

                QMessageBox::warning(this, ApplicationNameString, message);
                break;
            }
        case DICOMDIRImporter::ErrorFreeingSpace:
            message = tr("An error has occurred while freeing space, some studies cannot be imported.");
            message += "\n\n";
            message += UserMessage::getCloseWindowsAndTryAgainAdvice();
            message += "\n";
            message += UserMessage::getProblemPersistsAdvice();
            QMessageBox::critical(this, ApplicationNameString, message);
            break;
        case DICOMDIRImporter::DatabaseError:
            message = tr("A database error has occurred, some studies cannot be imported.");
            message += "\n\n";
            message += UserMessage::getCloseWindowsAndTryAgainAdvice();
            message += "\n";
            message += UserMessage::getProblemPersistsAdvice();
            QMessageBox::critical(this, ApplicationNameString, message);
            break;
        case DICOMDIRImporter::PatientInconsistent:
            message = tr("The study with UID %2 cannot be imported because %1 has not been able to correctly read DICOM information of the study.")
                    .arg(ApplicationNameString, studyInstanceUID);
            message += "\n\n";
            message += tr("The study may be corrupted, if it isn't please contact with the %1 team.").arg(ApplicationNameString);
            QMessageBox::critical(this, ApplicationNameString, message);
            break;
        case DICOMDIRImporter::DicomdirInconsistent:
            message = tr("An error has occurred while trying to import study with UID %1. ").arg(studyInstanceUID);
            message += tr("This DICOMDIR is inconsistent.");
            message += "\n\n";
            message += tr("Please contact with the %1 team.").arg(ApplicationNameString);
            QMessageBox::critical(this, ApplicationNameString, message);
            break;
        case DICOMDIRImporter::Ok:
            break;
        default:
            message = tr("An unknown error has occurred while importing DICOMDIR.");
            message += "\n\n";
            message += UserMessage::getCloseWindowsAndTryAgainAdvice();
            message += "\n";
            message += UserMessage::getProblemPersistsAdvice();
            QMessageBox::critical(this, ApplicationNameString, message);
    }
}

};
