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

#include "qcreatedicomdir.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>
#include <QProcess>
#include <QCloseEvent>
#include <QTimer>

#include "converttodicomdir.h"
#include "status.h"
#include "logging.h"
#include "status.h"
#include "harddiskinformation.h"
#include "directoryutilities.h"
#include "starviewerapplication.h"
#include "study.h"
#include "patient.h"
#include "inputoutputsettings.h"
#include "localdatabasemanager.h"
#include "isoimagefilecreator.h"
#include "dicommask.h"
#include "image.h"
#include "dicomdirburningapplication.h"

namespace udg {

const int QCreateDicomdir::CDRomSizeMb = 700;
const int QCreateDicomdir::DVDRomSizeMb = 4800;
const quint64 QCreateDicomdir::CDRomSizeBytes = (quint64) CDRomSizeMb * (quint64) (1024 * 1024);
const quint64 QCreateDicomdir::DVDRomSizeBytes = (quint64) DVDRomSizeMb * (quint64) (1024 * 1024);
const int QCreateDicomdir::DicomHeaderSizeBytes = 23000;

QCreateDicomdir::QCreateDicomdir(QWidget *parent)
 : QDialog(parent)
{
    setupUi(this);
    setWindowFlags(this->windowFlags() ^ Qt::WindowContextHelpButtonHint);
    QString sizeOfDicomdirText;

    resetDICOMDIRList();

    // Crear les accions
    createActions();
    createConnections();

    initializeControls();
}

QCreateDicomdir::~QCreateDicomdir()
{
    clearTemporaryDICOMDIRPath();
}

void QCreateDicomdir::initializeControls()
{
    Settings settings;
    settings.restoreColumnsWidths(InputOutputSettings::CreateDICOMDIRStudyListColumnsWidth, m_dicomdirStudiesList);

    settings.restoreGeometry(InputOutputSettings::CreateDICOMDIRGeometry, this);
    
    // Conte l'UID de l'estudi
    m_dicomdirStudiesList->setColumnHidden(7, true);

    // Per defecte creem els dicomdir al discdur
    m_hardDiskAction->trigger();

    m_anonymizeDICOMDIRCheckBox->setChecked(false);
    m_patientNameAnonymizedFrame->setVisible(false);
}

void QCreateDicomdir::createActions()
{
    m_cdromAction = new QAction(m_cdromDeviceToolButton);
    m_cdromAction->setText(tr("CD-ROM"));
    m_cdromAction->setStatusTip(tr("Record DICOMDIR on a CD-ROM"));
    m_cdromAction->setIcon(QIcon(":/images/icons/media-optical.svg"));
    m_cdromAction->setCheckable(true);
    connect(m_cdromAction, &QAction::triggered, [this] { deviceChanged(CreateDicomdir::CdRom); });
    m_cdromDeviceToolButton->setDefaultAction(m_cdromAction);

    m_dvdromAction = new QAction(m_dvdromDeviceToolButton);
    m_dvdromAction->setText(tr("DVD-ROM"));
    m_dvdromAction->setStatusTip(tr("Record DICOMDIR on a DVD-ROM"));
    m_dvdromAction->setIcon(QIcon(":/images/icons/media-optical-dvd.svg"));
    m_dvdromAction->setCheckable(true);
    connect(m_dvdromAction, &QAction::triggered, [this] { deviceChanged(CreateDicomdir::DvdRom); });
    m_dvdromDeviceToolButton->setDefaultAction(m_dvdromAction);

    m_hardDiskAction = new QAction(m_hardDiskDeviceToolButton);
    m_hardDiskAction->setText(tr("Hard Disk"));
    m_hardDiskAction->setStatusTip(tr("Record DICOMDIR on the Hard Disk"));
    m_hardDiskAction->setIcon(QIcon(":/images/icons/drive-harddisk.svg"));
    m_hardDiskAction->setCheckable(true);
    connect(m_hardDiskAction, &QAction::triggered, [this] { deviceChanged(CreateDicomdir::HardDisk); });
    m_hardDiskDeviceToolButton->setDefaultAction(m_hardDiskAction);

    m_pendriveAction = new QAction(m_pendriveDeviceToolButton);
    m_pendriveAction->setText(tr("USB Flash Drive"));
    m_pendriveAction->setStatusTip(tr("Record DICOMDIR on a USB Flash Drive"));
    m_pendriveAction->setIcon(QIcon(":/images/icons/drive-removable-media.svg"));
    m_pendriveAction->setCheckable(true);
    connect(m_pendriveAction, &QAction::triggered, [this] { deviceChanged(CreateDicomdir::UsbPen); });
    m_pendriveDeviceToolButton->setDefaultAction(m_pendriveAction);

    m_devicesActionGroup = new QActionGroup(this);
    m_devicesActionGroup->setExclusive(true);
    m_devicesActionGroup->addAction(m_cdromAction);
    m_devicesActionGroup->addAction(m_dvdromAction);
    m_devicesActionGroup->addAction(m_hardDiskAction);
    m_devicesActionGroup->addAction(m_pendriveAction);
}

void QCreateDicomdir::createConnections()
{
    connect(m_buttonRemove, SIGNAL(clicked()), this, SLOT(removeSelectedStudy()));
    connect(m_buttonRemoveAll, SIGNAL(clicked()), this, SLOT(resetDICOMDIRList()));
    connect(m_buttonExamineDisk, SIGNAL(clicked()), this, SLOT(examineDicomdirPath()));
    connect(m_buttonCreateDicomdir, SIGNAL(clicked()), this, SLOT(createDicomdir()));
    connect(m_copyFolderContentToDICOMDIRCdDvdCheckBox, SIGNAL(stateChanged(int)), this, SLOT(copyContentFolderToDICOMDIRCheckBoxsStateChanged()));
    connect(m_copyFolderContentToDICOMDIRUsbHardDiskCheckBox, SIGNAL(stateChanged(int)), this, SLOT(copyContentFolderToDICOMDIRCheckBoxsStateChanged()));
    connect(m_anonymizeDICOMDIRCheckBox, SIGNAL(toggled(bool)), SLOT(m_anonymizeDICOMDIRCheckBoxToggled(bool)));
}

void QCreateDicomdir::showDICOMDIRSize()
{
    QString sizeOfDicomdirText, sizeText;
    double sizeInMB;
    Settings settings;

    // Passem a MB
    sizeInMB = m_dicomdirSizeBytes / (1024.0 * 1024);
    sizeText.setNum(sizeInMB, 'f', 0);

    // Si les imatges s'han de convertir a LittleEndian obtenim el tamany que ocuparà l'estudi de manera aproximada
    if (settings.getValue(InputOutputSettings::ConvertDICOMDIRImagesToLittleEndianKey).toBool())
    {
        sizeOfDicomdirText = tr("DICOMDIR size: %1 MB approximately - Available Space: %2 MB").arg(sizeText)
                           .arg(m_availableSpaceToRecordInBytes / (1024 * 1024));
    }
    else
    {
        sizeOfDicomdirText = tr("DICOMDIR size: %1 MB - Available Space: %2 MB").arg(sizeText).arg(m_availableSpaceToRecordInBytes / (1024 * 1024));
    }

    m_dicomdirSizeOnDiskLabel->setText(sizeOfDicomdirText);

    if (sizeInMB < m_progressBarOcupat->maximum())
    {
        m_progressBarOcupat->setValue((int)sizeInMB);
    }
    else
    {
        m_progressBarOcupat->setValue(m_progressBarOcupat->maximum());
    }

    m_progressBarOcupat->repaint();

    sizeOfDicomdirText = tr("%1 MB").arg(sizeText);
    m_labelMbCdDvdOcupat->setText(sizeOfDicomdirText);
}

void QCreateDicomdir::addStudies(const QList<Study*> &studies)
{
    QStringList existingStudies;
    QStringList notAddedStudies;
    qint64 studySizeBytes;
    Status state;
    Settings settings;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    foreach (Study *study, studies)
    {
        if (!studyExistsInDICOMDIRList(study->getInstanceUID()))
        {
            // \TODO Xapussa perquè ara, a primera instància, continui funcionant amb les classes Study i demés. Caldria unificar el tema
            // "a quin directori està aquest study"?
            studySizeBytes = getStudySizeInBytes(settings.getValue(InputOutputSettings::ConvertDICOMDIRImagesToLittleEndianKey).toBool(),
                                                 study->getInstanceUID());
            // Comprovem si tenim prou espai per l'estudi
            if (studySizeBytes + m_dicomdirSizeBytes > m_availableSpaceToRecordInBytes)
            {
                notAddedStudies << study->getInstanceUID();
            }
            else
            {
                // Afegim la informació de l'estudi a la llista
                QTreeWidgetItem *item = new QTreeWidgetItem(m_dicomdirStudiesList);
                m_dicomdirSizeBytes = m_dicomdirSizeBytes + studySizeBytes;

                Patient *patient = study->getParentPatient();
                item->setText(0, patient->getFullName());
                item->setText(1, patient->getID());
                item->setText(2, study->getPatientAge());
                item->setText(3, study->getDescription());
                item->setText(4, study->getModalitiesAsSingleString());
                item->setText(5, study->getDate().toString(Qt::ISODate));
                item->setText(6, study->getTime().toString(Qt::ISODate));
                item->setText(7, study->getInstanceUID());
            }
        }
        else
        {
            existingStudies << study->getInstanceUID();
        }
    }
    QApplication::restoreOverrideCursor();

    updateDICOMDIRSizeWithFolderToCopyToDICOMDIRSize();
    showDICOMDIRSize();

    if (notAddedStudies.size() > 0 || existingStudies.size() > 0)
    {
        QString message;
        message = tr("The following studies were not added to the DICOMDIR list for the following reasons:");
        if (existingStudies.size() > 0)
        {
            message += "\n\n\t";
            message += tr("- Already exist in the list.") + "\n";
            message += existingStudies.join("\n");
        }
        if (notAddedStudies.size() > 0)
        {
            message += "\n\n\t";
            message += tr("- Not enough space on the device.") + "\n";
            message += notAddedStudies.join("\n");
        }
        QMessageBox::warning(0, ApplicationNameString, message);
    }
}

void QCreateDicomdir::createDicomdir()
{
    // TODO:S'hauria de crear mètodes amb aquestes precondicions a comprovar abans de crear un DICOMDIR
    if (m_dicomdirSizeBytes > m_availableSpaceToRecordInBytes)
    {
        QMessageBox::warning(this, ApplicationNameString,
                             tr("DICOMDIR creation aborted.\nThe selected studies exceed the available space for the current device."));
        return;
    }

    /// Comprovem en funció del dispositiu a crear el DICOMDIR, si s'ha de copiar el contingut del directori escollit al DICOMDIR, si és així comprovem si
    /// l'usuari té permisos de lectura sobre el directori i si el directori existeix, en cas que alguna d'aquestes dos condicions no es compleixi es dona
    /// la possibilitat de continuar a l'usuari
    // TODO: Passar això a un mètode
    if (haveToCopyFolderContentToDICOMDIR())
    {
        Settings settings;
        QString message;
        QDir qdir(settings.getValue(InputOutputSettings::DICOMDIRFolderPathToCopy).toString());

        if (!qdir.exists() || !qdir.isReadable())
        {
            if (!qdir.exists())
            {
                message = tr("The directory '%1' from where to copy the content to DICOMDIR does not exist.")
                    .arg(settings.getValue(InputOutputSettings::DICOMDIRFolderPathToCopy).toString());
                message += "\n\n";
            }
            else
            {
                message = tr("You don't have read permission on directory '%1' to copy its content to DICOMDIR.")
                    .arg(settings.getValue(InputOutputSettings::DICOMDIRFolderPathToCopy).toString());
                message += "\n\n";
            }
            message += tr("Do you want to create the DICOMDIR without copying its content?");

            if (QMessageBox::question(this, ApplicationNameString, message, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No)
            {
                return;
            }
            else
            {
                // Si continuem sense copiar el directori desactivem l'opció.
                if (m_currentDevice == CreateDicomdir::UsbPen || m_currentDevice == CreateDicomdir::HardDisk)
                {
                    m_copyFolderContentToDICOMDIRUsbHardDiskCheckBox->setChecked(false);
                }
                else
                {
                    m_copyFolderContentToDICOMDIRCdDvdCheckBox->setChecked(false);
                }
            }
        }
    }

    switch (m_currentDevice)
    {
        case CreateDicomdir::UsbPen:
        case CreateDicomdir::HardDisk:
                createDicomdirOnHardDiskOrFlashMemories();
                break;
        case CreateDicomdir::DvdRom:
        case CreateDicomdir::CdRom:
                // Cd, si s'ha creat bé, executem el programa per gravar el dicomdir a cd's
                Status state = createDicomdirOnCdOrDvd();
                // Error 4001 és el cas en que alguna imatge de l'estudi no compleix amb l'estàndard dicom tot i així el deixem gravar
                if (state.good() || (!state.good() && state.code() == 4001))
                {
                    burnDicomdir();
                }
                break;
    }
}

Status QCreateDicomdir::createDicomdirOnCdOrDvd()
{
    QDir temporaryDirPath;
    QString dicomdirPath = getTemporaryDICOMDIRPath();
    Status state;

    // Si el directori dicomdir ja existeix al temporal l'esborrem
    clearTemporaryDICOMDIRPath();

    INFO_LOG("Iniciant la creació del DICOMDIR en cd-dvd al directori temporal " + dicomdirPath);

    // Creem el directori temporal
    if (!temporaryDirPath.mkpath(dicomdirPath))
    {
        QMessageBox::critical(this, ApplicationNameString, tr("Unable to create the temporary directory to create the DICOMDIR. Please check user permissions."));
        ERROR_LOG("Error al crear directori " + dicomdirPath);
        return state.setStatus("Unable to create temporary DICOMDIR", false, 3002);
    }
    else
    {
        return startCreateDicomdir(dicomdirPath);
    }
}

void QCreateDicomdir::createDicomdirOnHardDiskOrFlashMemories()
{
    QString dicomdirPath = m_lineEditDicomdirPath->text();
    DirectoryUtilities delDirectory;
    QDir directoryDicomdirPath(dicomdirPath);

    // Comprovem si el directori ja es un dicomdir, si és el cas demanem a l'usuari si el desitja sobreecriue o, els estudis seleccionats s'afegiran ja al
    // dicomdir existent

    if (m_lineEditDicomdirPath->text().isEmpty())
    {
        QMessageBox::information(this, ApplicationNameString, tr("No directory specified to create the DICOMDIR."));
        return;
    }

    INFO_LOG("Iniciant la creació del DICOMDIR en discdur o usb al directori " + dicomdirPath);

    // TODO:El codi de comprova si el directori és un DICOMDIR hauria d'estar a una DICOMDIRManager, la UI no ha de saber
    // quins elements componen un DICOMDIR
    if (dicomdirPathIsADicomdir(dicomdirPath))
    {
        switch (QMessageBox::question(this,
                tr("Create DICOMDIR"),
                tr("The directory contains a DICOMDIR. Do you want to overwrite and delete all the files in the directory?"),
                tr("&Yes"), tr("&No"), 0, 1))
        {
            case 0:
                // Si vol sobreescriure, esborrem el contingut del directori
                delDirectory.deleteDirectory(dicomdirPath, false);
                break;
            case 1:
                INFO_LOG("El directori no està buit, i l'usuari no dona permís per esborrar el seu contingut");
                // No fem res, l'usuari no vol sobreescriure el directori, cancel·lem l'operacio i tornem el control a l'usuari
                return;
                break;
        }
    }
    else if (!dicomdirPathIsEmpty(dicomdirPath))
    {
        WARN_LOG(QString("No es pot crear el DICOMDIR perque no esta buit el directori de desti %1").arg(dicomdirPath));
        QMessageBox::information(this, ApplicationNameString, tr("The destination directory is not empty, please choose an empty directory."));
        return;
    }
    else
    {
        // El directori no és un dicomdir
        // Si el directori no existiex, preguntem si el vol crear
        if (!directoryDicomdirPath.exists())
        {
                switch (QMessageBox::question(this,
                        tr("Create Directory?"),
                        tr("The DICOMDIR directory does not exist. Do you want to create it?"),
                        tr("&Yes"), tr("&No"), 0, 1))
                {
                    case 0:
                        if (!directoryDicomdirPath.mkpath(dicomdirPath))
                        {
                            QMessageBox::critical(this, ApplicationNameString, tr("Unable to create directory. Please check user permissions."));
                            ERROR_LOG("Error al crear directori " + dicomdirPath);
                        }
                        break;
                    case 1:
                        INFO_LOG("El directori especificat per l'usuari no existeix, i no el vol crear per tant cancel·lem la creació del DICOMDIR");
                        // Cancel·lem;
                        return;
                        break;
                }
        }
    }

    startCreateDicomdir(dicomdirPath);

    // Guardem la ruta de l'ultim directori on l'usuari ha creat el dicomdir
    m_lastDicomdirDirectory = dicomdirPath;
}

Status QCreateDicomdir::startCreateDicomdir(QString dicomdirPath)
{
    ConvertToDicomdir convertToDicomdir;
    Status state;
    Settings settings;

    convertToDicomdir.setConvertDicomdirImagesToLittleEndian(settings.getValue(InputOutputSettings::ConvertDICOMDIRImagesToLittleEndianKey).toBool());

    // Comprovem si hi ha suficient espai lliure al disc dur
    if (!enoughFreeSpace(dicomdirPath))
    {
        QMessageBox::information(this, ApplicationNameString, tr("Not enough free space to create DICOMDIR. Please free space."));
        ERROR_LOG("Error al crear el DICOMDIR, no hi ha suficient espai al disc ERROR : " + state.text());
        return state.setStatus("Not enough space to create DICOMDIR", false, 3000);
    }

    QList<QTreeWidgetItem*> dicomdirStudiesList(m_dicomdirStudiesList ->findItems("*", Qt::MatchWildcard, 0));
    QTreeWidgetItem *item;

    // Comprovem que hi hagi estudis seleccionats per crear dicomdir
    if (dicomdirStudiesList.count() == 0)
    {
        QMessageBox::information(this, ApplicationNameString, tr("You haven't selected any study to create the DICOMDIR. Please select at least one study."));
        return state.setStatus("No study selected to create the DICOMDIR", false, 3001);
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    for (int i = 0; i < dicomdirStudiesList.count(); i++)
    {
        item = dicomdirStudiesList.at(i);
        // Indiquem a la classe convertToDicomdir, quins estudis s'ha de convertir a dicomdir, passant el UID de l'estudi
        convertToDicomdir.addStudy(item->text(7));
        INFO_LOG("L'estudi " + item->text(7) + " s'afegirà al DICOMDIR ");
    }

    if (m_anonymizeDICOMDIRCheckBox->isChecked())
    {
        convertToDicomdir.setAnonymizeDICOMDIR(true, m_patientNameAnonymizedLineEdit->text());
    }

    state = convertToDicomdir.convert(dicomdirPath, m_currentDevice, haveToCopyFolderContentToDICOMDIR());

    bool clearScreen = false;
    if (!state.good())
    {
        QApplication::restoreOverrideCursor();
        // TODO Estaria bé que el diàleg pogués mostrar adicionalment els detalls de l'error produit que es trobarien a state.text()
        switch (state.code())
        {
            case 4001:
                // Alguna de les imatges no compleix l'estandard dicom però es pot continuar endavant
                QMessageBox::information(this, ApplicationNameString, tr("Some images are not 100% DICOM compliant. Some viewers might have problems" 
                                                                            " to open them."));
                clearScreen = true;
                break;
            case 4002:
                QMessageBox::warning(this, ApplicationNameString, tr("Unable to create DICOMDIR. The content of '%1' cannot be copied. Make sure you have "
                    "read permission in the directory or uncheck copy folder content option.")
                    .arg(settings.getValue(InputOutputSettings::DICOMDIRFolderPathToCopy).toString()));
                break;
            case 4003:
                QMessageBox::warning(this, ApplicationNameString, tr("Unable to create DICOMDIR. The folder '%1' that should be copied contains an item called "
                    "DICOMDIR or DICOM.\n\nRemove the item from the directory or uncheck copy folder content option.")
                    .arg(settings.getValue(InputOutputSettings::DICOMDIRFolderPathToCopy).toString()));
                break;
            case 3003:
                QMessageBox::warning(this, ApplicationNameString, tr("Error anonymizing DICOMDIR, make sure you have write permission on %1 "
                    "or try to create the DICOMDIR without the anonymize option.")
                    .arg(dicomdirPath));
                break;

            case 3001:
                QMessageBox::warning(this, ApplicationNameString, tr("Error copying files, make sure you have appropriate permissions on both source and "
                    "destination folders to copy the files."));
                break;
            
            case 1301:
                QMessageBox::warning(this, ApplicationNameString, tr("Error creating DICOMDIR file, make sure you have read and write permissions on the "
                    "destination folder."));
                break;
            
            default:
                QMessageBox::critical(this, ApplicationNameString, tr("Error creating DICOMDIR. Make sure you have appropriate permissions on both source and "
                    "destination folders and %1 folder is empty.").arg(m_lineEditDicomdirPath->text()));
                ERROR_LOG(QString("Error (%1) al crear el DICOMDIR: %2").arg(state.code()).arg(state.text()));
                DEBUG_LOG(QString("Error (%1) al crear el DICOMDIR: %2").arg(state.code()).arg(state.text()));
                return state;
        }
    }
    else
    {
        clearScreen = true;
    }

    INFO_LOG("Finalitzada la creació del DICOMDIR");
    if (clearScreen)
    {
        clearQCreateDicomdirScreen();
    }

    QApplication::restoreOverrideCursor();

    return state;
}

void QCreateDicomdir::clearQCreateDicomdirScreen()
{
    m_dicomdirStudiesList->clear();
    m_lineEditDicomdirPath->clear();
    // TODO: Al mètode m_anonymizeDICOMDIRCheckBoxToggled també se li assignar valor Anonymous
    m_patientNameAnonymizedLineEdit->setText("Anonymous");

    resetDICOMDIRList();
}

void QCreateDicomdir::examineDicomdirPath()
{
    QString initialDirectory;
    QDir dicomdirPath;

    // Si hi ha entrat un directori
    if (!m_lineEditDicomdirPath->text().isEmpty())
    {
        if (dicomdirPath.exists(m_lineEditDicomdirPath->text()))
        {
            // Si el directori existeix, serà el directori inicial al obrir
            initialDirectory = m_lineEditDicomdirPath->text();
        }
        else
        {
            // Si no existeix directori entrat el directori inicial serà el home
            initialDirectory = QDir::homePath();
        }
    }
    else
    {
        if (m_lastDicomdirDirectory.isEmpty())
        {
            // Si no tenim last directory anem al directori home
            initialDirectory = QDir::homePath();
        }
        else
        {
            dicomdirPath.setPath(m_lastDicomdirDirectory);
            dicomdirPath.cdUp();
            initialDirectory = dicomdirPath.path();
        }
    }

    QString path = QFileDialog::getExistingDirectory(this, tr("Choose an empty directory..."), initialDirectory);
    if (!path.isEmpty())
    {
        m_lineEditDicomdirPath->setText(QDir::toNativeSeparators(path));
        // Actualitzem les etiquetes que indiquen la capacitat del disc
        updateAvailableSpaceToRecord();
        showDICOMDIRSize();
    }
}

void QCreateDicomdir::resetDICOMDIRList()
{
    m_dicomdirSizeBytes = 0;
    m_dicomdirStudiesList->clear();
    m_folderToCopyToDICOMDIRSizeAddedToDICOMDIRSize = false;

    showDICOMDIRSize();
}

void QCreateDicomdir::removeSelectedStudy()
{
    Settings settings;

    if (m_dicomdirStudiesList->selectedItems().count() != 0)
    {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        foreach (QTreeWidgetItem *selectedStudy, m_dicomdirStudiesList->selectedItems())
        {
            // La columna 7 de m_dicomdirStudiesList conté Study Instance UID
            m_dicomdirSizeBytes -= getStudySizeInBytes(settings.getValue(InputOutputSettings::ConvertDICOMDIRImagesToLittleEndianKey).toBool(),
                                                       selectedStudy->text(7));

            delete selectedStudy;
        }
        QApplication::restoreOverrideCursor();

        if (m_dicomdirStudiesList->findItems("*", Qt::MatchWildcard, 0).count() == 0)
        {
            // Si no tenim cap estudi reiniciem les variables que controlen la Llista de DICOMDIR
            resetDICOMDIRList();
        }
        else
        {
            updateDICOMDIRSizeWithFolderToCopyToDICOMDIRSize();
        }
        showDICOMDIRSize();
    }
    else
    {
        QMessageBox::information(this, ApplicationNameString, tr("Please select a study to remove it from the list."));
    }
}

bool QCreateDicomdir::studyExistsInDICOMDIRList(QString studyUID)
{
    QList<QTreeWidgetItem*> dicomdirStudiesList(m_dicomdirStudiesList ->findItems(studyUID, Qt::MatchExactly, 7));

    if (dicomdirStudiesList.count() > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void QCreateDicomdir::burnDicomdir()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QString dicomdirPath = getTemporaryDICOMDIRPath(), isoPath;

    // Indiquem al directori i nom de la imatge a crear
    isoPath = dicomdirPath + "/dicomdir.iso";

    // Es crea un ProgressDialog
    m_progressBar = new QProgressDialog(QObject::tr("Creating DICOMDIR image..."), "", 0, 0);
    m_progressBar->setMinimumDuration(0);
    m_progressBar->setCancelButton(0);
    m_progressBar->setModal(true);
    m_progressBar->setValue(1);

    // Es crea un Timer per tal de poder moure la barra de progres cada segon
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), m_progressBar, SLOT(setValue(m_progressBar->value() + 1)));
    m_timer->start(1000);

    m_isoImageFileCreator = new IsoImageFileCreator();

    // Es conecta el signal que indica la finalització de la creació del fitxer d'imatge ISO amb l'Slot que obre el programa de gravació
    connect(m_isoImageFileCreator, SIGNAL(finishedCreateIsoImageFile(bool)), this, SLOT(openBurningApplication(bool)));

    m_isoImageFileCreator->setInputPath(dicomdirPath);
    m_isoImageFileCreator->setOutputIsoImageFilePath(isoPath);
    m_isoImageFileCreator->setIsoImageLabel(QString("%1 DICOMDIR").arg(ApplicationNameString));

    m_isoImageFileCreator->startCreateIsoImageFile();
}

void QCreateDicomdir::openBurningApplication(bool createIsoResult)
{
    m_timer->stop();
    delete m_timer;
    m_progressBar->close();
    delete m_progressBar;

    if (createIsoResult)
    {
        DICOMDIRBurningApplication dicomdirBurningApplication;
        dicomdirBurningApplication.setCurrentDevice(m_currentDevice);
        dicomdirBurningApplication.setIsoPath(m_isoImageFileCreator->getOutputIsoImageFilePath());

        if (!dicomdirBurningApplication.burnIsoImageFile())
        {
            QMessageBox::critical(this, tr("DICOMDIR Burning Failure"), tr("There was an error during the burning of the DICOMDIR ISO image file.") + "\n" +
                dicomdirBurningApplication.getLastErrorDescription() + "\n\n" + tr("Please, contact your system administrator to solve this problem."));
            ERROR_LOG("Error al gravar la imatge ISO amb descripció: " + dicomdirBurningApplication.getLastErrorDescription());
        }
    }
    else
    {
        QMessageBox::critical(this, tr("DICOMDIR creation failure"), tr("There was an error during the creation of the DICOMDIR ISO image file.") + "\n" +
            m_isoImageFileCreator->getLastErrorDescription() + "\n\n" + tr("Please, contact your system administrator to solve this problem."));
        ERROR_LOG("Error al crear ISO amb descripció: " + m_isoImageFileCreator->getLastErrorDescription());
    }
    delete m_isoImageFileCreator;
    // Un cop acabada l'acció de gravar, esborrem el directori temporal
    clearTemporaryDICOMDIRPath();
    QApplication::restoreOverrideCursor();
}

void QCreateDicomdir::showProcessErrorMessage(const QProcess &process, QString name)
{
    QString errorMessage;
    switch (process.error())
    {
        case QProcess::FailedToStart:
            errorMessage = tr("The process [ %1 ] failed to start. Either the invoked program is missing, or you may have insufficient permissions "
                              "to invoke the program.").arg(name);
            break;

        case QProcess::Crashed:
            errorMessage = tr("The process [ %1 ] crashed some time after starting successfully.").arg(name);
            break;

//             case QProcess::Timedout:
//                 errorMessage = tr("The last waitFor...() function timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again.").arg(name);
//                 break;

            case QProcess::WriteError:
                errorMessage = tr("An error occurred when attempting to write to the process [ %1 ]. For example, the process may not be running, or it may "
                                  "have closed its input channel.").arg(name);
                break;

            case QProcess::ReadError:
                errorMessage = tr("An error occurred when attempting to read from the process [ %1 ]. For example, the process may not be running.").arg(name);
                break;

//             case QProcess::UnknownError:
//                 errorMessage = tr("An unknown error occurred with the process [ %1 ]").arg("mkisofs");
//                 break;

            default:
                break;
    }
    QMessageBox::critical(this, tr("DICOMDIR creation failure"), tr("There was an error during the creation of the DICOMDIR.") + "\n\n" + errorMessage +
                                   "\n\n" + tr("Please, contact your system administrator to solve this problem."));

}

bool QCreateDicomdir::enoughFreeSpace(QString path)
{
    HardDiskInformation hardDisk;

    if (hardDisk.getNumberOfFreeMBytes(path) < static_cast<quint64> (m_dicomdirSizeBytes / (1024 * 1204)))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void QCreateDicomdir::clearTemporaryDICOMDIRPath()
{
    QString dicomdirPath = getTemporaryDICOMDIRPath();

    if (QFile::exists(dicomdirPath))
    {
        DirectoryUtilities delDirectory;
        delDirectory.deleteDirectory(dicomdirPath, true);
    }
}

void QCreateDicomdir::showDatabaseErrorMessage(const Status &state)
{
    if (!state.good())
    {
        QMessageBox::critical(this, ApplicationNameString, state.text() + "\n" + tr("Error Number: %1").arg(state.code()));
    }
}

bool QCreateDicomdir::dicomdirPathIsEmpty(QString dicomdirPath)
{
    QDir dir(dicomdirPath);

    // Llista de fitxers del directori
    return dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot).count() == 0;
}

bool QCreateDicomdir::dicomdirPathIsADicomdir(QString dicomdirPath)
{
    QDir dir(dicomdirPath);
    QStringList fileList = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    // És un directori dicomdir creat per l'starviewer quan només conté un fitxer anomenat DICOMDIR i un directori anomenat DICOM
    if (fileList.count() == 1)
    {
        if (fileList.contains("DICOM", Qt::CaseInsensitive))
        {
            // Si conté directori anomenat dicom
            fileList = dir.entryList(QDir::Files);
            if (fileList.count() == 1)
            {
                // Si conté un fitxer anomenat dicomdir
                return fileList.contains("DICOMDIR", Qt::CaseInsensitive);
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

void QCreateDicomdir::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::ActivationChange)
    {
        Settings settings;

        if (!settings.getValue(InputOutputSettings::DICOMDIRFolderPathToCopy).toString().isEmpty())
        {
            m_copyFolderContentToDICOMDIRCdDvdCheckBox->setEnabled(true);
            m_copyFolderContentToDICOMDIRUsbHardDiskCheckBox->setEnabled(true);
            m_copyFolderContentToDICOMDIRCdDvdCheckBox->setText(tr("Copy the content of \"%1\" to DICOMDIR.")
                .arg(QDir::toNativeSeparators(settings.getValue(InputOutputSettings::DICOMDIRFolderPathToCopy).toString())));
            m_copyFolderContentToDICOMDIRUsbHardDiskCheckBox->setText(tr("Copy the content of \"%1\" to DICOMDIR.")
                .arg(QDir::toNativeSeparators(settings.getValue(InputOutputSettings::DICOMDIRFolderPathToCopy).toString())));
            m_copyFolderContentToDICOMDIRCdDvdCheckBox->setChecked(settings.getValue(InputOutputSettings::CopyFolderContentToDICOMDIROnCDOrDVD).toBool());
            m_copyFolderContentToDICOMDIRUsbHardDiskCheckBox->setChecked(settings.getValue(InputOutputSettings::CopyFolderContentToDICOMDIROnUSBOrHardDisk)
                                                                         .toBool());
        }
        else
        {
            // Si no ens han especificat Path a copiar descativem els checkbox
            m_copyFolderContentToDICOMDIRCdDvdCheckBox->setEnabled(false);
            m_copyFolderContentToDICOMDIRUsbHardDiskCheckBox->setEnabled(false);
        }
    }

    QDialog::changeEvent(event);
}

void QCreateDicomdir::closeEvent(QCloseEvent *ce)
{
    Settings settings;
    settings.saveColumnsWidths(InputOutputSettings::CreateDICOMDIRStudyListColumnsWidth, m_dicomdirStudiesList);
    settings.saveGeometry(InputOutputSettings::CreateDICOMDIRGeometry, this);
    
    ce->accept();
}

void QCreateDicomdir::deviceChanged(int index)
{
    m_currentDevice = (CreateDicomdir::recordDeviceDicomDir) index;

    updateAvailableSpaceToRecord();
    // Starviewer donoa la possibilitat de copiar al contingut d'una carpeta als DICOMDIR que es generen^,
    // Cridem aquest mètode perquè en funció del dispositu potser que es copïi o no la carpeta al DICOMDIR per tant s'ha d'actualitzar
    // la mida del DICOMDIR
    updateDICOMDIRSizeWithFolderToCopyToDICOMDIRSize();

    int maximumDeviceCapacityInMB = m_availableSpaceToRecordInBytes / (1024 * 1024);
    if (m_dicomdirSizeBytes > m_availableSpaceToRecordInBytes)
    {
        QMessageBox::warning(this, ApplicationNameString, tr("The selected device does not have enough space to create a DICOMDIR with the selected studies, "
            "please remove some studies. The capacity of the device is %1 MB.").arg(maximumDeviceCapacityInMB));
    }
    
    switch (m_currentDevice)
    {
        case CreateDicomdir::UsbPen:
        case CreateDicomdir::HardDisk:
            m_stackedWidget->setCurrentIndex(1);
            break;

        case CreateDicomdir::CdRom:
        case CreateDicomdir::DvdRom:
            if (checkDICOMDIRBurningApplicationConfiguration())
            {
                // Indiquem que es mostri la barra de progrés
                m_stackedWidget->setCurrentIndex(0);

                m_progressBarOcupat->setMaximum(maximumDeviceCapacityInMB);
            }
            else
            {
                // La configuració de l'aplicació per gravar cd/dvd no ès vàlida
                QMessageBox::warning(this, ApplicationNameString,
                                     tr("Invalid configuration of the DICOMDIR burning application.\n"
                                        "Please, provide a valid configuration in Tools -> Configuration -> DICOMDIR."));

                // Marquem la opció de crear el dicomdir al disc dur
                m_hardDiskAction->trigger();
            }

            break;
    }

    // El cridem per refrescar la barra de progrés
    showDICOMDIRSize();
}

bool QCreateDicomdir::checkDICOMDIRBurningApplicationConfiguration()
{
    Settings settings;
    return QFile::exists((settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationPathKey)).toString());
}

quint64 QCreateDicomdir::getStudySizeInBytes(bool transferSyntaxInLittleEndian, QString studyInstanceUID)
{
    LocalDatabaseManager localDatabaseManager;

    if (transferSyntaxInLittleEndian)
    {
        // Si les imatges es transformaran a transfer syntax LittleEndian fem un càlcul aproximat del que ocuparà el dicomdir
        DicomMask imageMask;
        quint64 studySizeInLittleEndianTransferSyntax = 0;

        imageMask.setStudyInstanceUID(studyInstanceUID);

        // Agafem les imatges de l'estudi per fer l'estimació del que ocuparà el dicomdir
        QList<Image*> imagesOfStudy = localDatabaseManager.queryImages(imageMask);

        foreach (Image *image, imagesOfStudy)
        {
            studySizeInLittleEndianTransferSyntax += getImageSizeInBytesInLittleEndianTransferSyntax(image);
        }

        return studySizeInLittleEndianTransferSyntax;
    }
    else
    {
        return HardDiskInformation::getDirectorySizeInBytes(localDatabaseManager.getStudyPath(studyInstanceUID));
    }
}

quint64 QCreateDicomdir::getImageSizeInBytesInLittleEndianTransferSyntax(Image *image)
{
    // Per calcular la mida que ocupa el pixel Data un estudi en LittleEndian és
    //  size = (bits-allocated / 8) * rows * columns * samples-per-pixel;
    //  size += size % 2;
    // Llavors hem d'afegir la capçalera => dicom header size

    quint64 imageSizeInBytesInLittleEndianTransferSyntax = 0;

    imageSizeInBytesInLittleEndianTransferSyntax = (image->getBitsAllocated() / 8) * image->getRows() * image->getColumns() * image->getSamplesPerPixel();
    imageSizeInBytesInLittleEndianTransferSyntax += imageSizeInBytesInLittleEndianTransferSyntax % 2;

    // Afegim el tamany de la capçalera
    imageSizeInBytesInLittleEndianTransferSyntax += DicomHeaderSizeBytes;

    return imageSizeInBytesInLittleEndianTransferSyntax;
}

void QCreateDicomdir::updateAvailableSpaceToRecord()
{
    QString path = m_lineEditDicomdirPath->text();
    HardDiskInformation diskInfo;
    switch (m_currentDevice)
    {
        case CreateDicomdir::UsbPen:
        case CreateDicomdir::HardDisk:
            if (path.isEmpty() || !QDir(path).exists())
            {
                m_lineEditDicomdirPath->setText(QDir::toNativeSeparators(QDir::rootPath()));
                path = QDir::rootPath();
            }
            m_availableSpaceToRecordInBytes = diskInfo.getNumberOfFreeBytes(path);
            break;

        case CreateDicomdir::CdRom:
            m_availableSpaceToRecordInBytes = CDRomSizeBytes;
            break;

        case CreateDicomdir::DvdRom:
            m_availableSpaceToRecordInBytes = DVDRomSizeBytes;
            break;
    }
}

QString QCreateDicomdir::getTemporaryDICOMDIRPath()
{
    return QDir::tempPath() + "/DICOMDIR";
}

quint64 QCreateDicomdir::getFolderToCopyToDICOMDIRSizeInBytes()
{
    Settings settings;
    int size = 0;

    if (settings.getValue(InputOutputSettings::DICOMDIRFolderPathToCopy).toBool())
    {
        QString folderPathToCopy = settings.getValue(InputOutputSettings::DICOMDIRFolderPathToCopy).toString();

        if (QFile::exists(folderPathToCopy))
        {
            size = HardDiskInformation().getDirectorySizeInBytes(folderPathToCopy);
        }
    }

    return size;
}

bool QCreateDicomdir::haveToCopyFolderContentToDICOMDIR()
{
    // S'ha de copiar el visor DICOM si està configurat així als settings i el dispositiu actual és cd/dvd
    return (m_copyFolderContentToDICOMDIRCdDvdCheckBox->isChecked() && (m_currentDevice == CreateDicomdir::CdRom || m_currentDevice == CreateDicomdir::DvdRom))
        || (m_copyFolderContentToDICOMDIRUsbHardDiskCheckBox->isChecked() && (m_currentDevice == CreateDicomdir::UsbPen
                                                                          ||  m_currentDevice == CreateDicomdir::HardDisk));
}

void QCreateDicomdir::updateDICOMDIRSizeWithFolderToCopyToDICOMDIRSize()
{
    QList<QTreeWidgetItem*> dicomdirStudies = m_dicomdirStudiesList->findItems("*", Qt::MatchWildcard, 0);

    if (haveToCopyFolderContentToDICOMDIR() && dicomdirStudies.count() > 0 &&
        !m_folderToCopyToDICOMDIRSizeAddedToDICOMDIRSize)
    {
        // Si hem de copiar el contingut de la carpeta al DICOMDIR, tenim un estudi o més i no l'havíem tingut en compte
        // la mida de la carpeta a copiar en el tamany del DICOMDIR, li afegim
        m_dicomdirSizeBytes += getFolderToCopyToDICOMDIRSizeInBytes();
        m_folderToCopyToDICOMDIRSizeAddedToDICOMDIRSize = true;
    }

    if (!haveToCopyFolderContentToDICOMDIR() && dicomdirStudies.count() > 0 &&
        m_folderToCopyToDICOMDIRSizeAddedToDICOMDIRSize)
    {
        // Si no hem de copiar el contingut de la carpeta al DICOMDIR, tenim un estudi o més i l'havíem tingut en compte
        // la mida de la carpeta a copiar en el tamany del DICOMDIR, li restem

        m_dicomdirSizeBytes -= getFolderToCopyToDICOMDIRSizeInBytes();
        m_folderToCopyToDICOMDIRSizeAddedToDICOMDIRSize = false;
    }
}

void QCreateDicomdir::copyContentFolderToDICOMDIRCheckBoxsStateChanged()
{
    updateDICOMDIRSizeWithFolderToCopyToDICOMDIRSize();
    showDICOMDIRSize();
}

void QCreateDicomdir::m_anonymizeDICOMDIRCheckBoxToggled(bool checked)
{
    m_patientNameAnonymizedFrame->setVisible(checked);

    if (checked)
    {
        m_patientNameAnonymizedLineEdit->setText("Anonymous");
    }
    else
    {
        m_patientNameAnonymizedLineEdit->setText("");
    }
}
}
