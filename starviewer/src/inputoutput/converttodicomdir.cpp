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

#include "converttodicomdir.h"

#include <QProgressDialog>
#include <QTextStream>
#include <QFile>

#include "logging.h"
#include "status.h"
#include "dicommask.h"
#include "convertdicomtolittleendian.h"
#include "directoryutilities.h"
#include "starviewerapplication.h"
#include "localdatabasemanager.h"
#include "patient.h"
#include "study.h"
#include "image.h"
#include "inputoutputsettings.h"
#include "dicomanonymizer.h"

namespace udg {

ConvertToDicomdir::ConvertToDicomdir()
{
    m_study = 0;
    m_series = 0;
    m_currentItemNumber = 0;
    m_patient = 0;

    m_convertDicomdirImagesToLittleEndian = false;
    m_anonymizeDICOMDIR = false;
    m_progress = NULL;
    m_DICOMAnonymizer = NULL;
}

void ConvertToDicomdir::setAnonymizeDICOMDIR(bool anonymizeDICOMDIR, QString patientNameAnonymized)
{
    m_anonymizeDICOMDIR = anonymizeDICOMDIR;
    m_patientNameAnonymized = patientNameAnonymized;
}

void ConvertToDicomdir::addStudy(const QString &studyUID)
{
    // Els estudis s'han d'agrupar per pacient, el que fem és afegir-los a llista d'estudis per convertir a
    // dicomdir ja ordenats per pacient
    StudyToConvert studyToConvert;
    int index = 0;
    bool stop = false;

    LocalDatabaseManager localDatabaseManager;
    DicomMask studyMask;
    studyMask.setStudyInstanceUID(studyUID);
    QList<Patient*> patientList = localDatabaseManager.queryPatientsAndStudies(studyMask);
    if (localDatabaseManager.getLastError() != LocalDatabaseManager::Ok)
    {
        ERROR_LOG(QString("Error al afegir un study per generar un DICOMDIR; Error: %1; StudyUID: %2")
                  .arg(localDatabaseManager.getLastError())
                  .arg(studyUID));
        return;
    }

    // \TODO Això s'ha de fer perquè queryPatientStudy retorna llista de Patients
    // Nosaltres, en realitat només en volem un.
    Patient *patient = patientList.first();

    studyToConvert.studyUID = studyUID;
    studyToConvert.patientId = patient->getID();

    delete patient;

    // Busquem la posició on s'ha d'inserir l'estudi a llista d'estudis per convertir a dicomdir, ordenant per id de pacient
    while (index < m_studiesToConvert.count() && !stop)
    {
        // Comparem amb els altres estudis de la llista, fins trobar el seu llloc corresponentm
        if (studyToConvert.patientId < m_studiesToConvert.at(index).patientId)
        {
            stop = true;
        }
        else
        {
            index++;
        }
    }

    // Una vegada hem trobat la posició on ha d'anar l'inserim
    if (stop)
    {
        m_studiesToConvert.insert(index, studyToConvert);
    }
    else
    {
        // En aquest cas val al final
        m_studiesToConvert.push_back(studyToConvert);
    }
}

// TODO:Si la creació del DICOMDIR Falla aquest mètode esborra el contingut del directori on s'havia de crear el DICOMDIR, al fer això
//      la classe abans de crear el DICOMDIR hauria de comprovar que el directori està buit, perquè sinó podríem eliminar contingut del usuari.
//      Ara aquesta comprovació es fa a la QCreateDicomdir i s'hauria de fer aquí
Status ConvertToDicomdir::convert(const QString &dicomdirPath, CreateDicomdir::recordDeviceDicomDir selectedDevice, bool copyFolderContent)
{
    // Primer copiem els estudis al directori desti, i posteriorment convertim el directori en un dicomdir
    Status state;
    int totalNumberOfItems = 0;

    m_dicomDirPath = dicomdirPath;

    QString pathFolderContentToCopyToDICOMDIR = Settings().getValue(InputOutputSettings::DICOMDIRFolderPathToCopy).toString();
    if (copyFolderContent && !AreValidRequirementsOfFolderContentToCopyToDICOMDIR(pathFolderContentToCopyToDICOMDIR))
    {
        ERROR_LOG(QString("No es pot crear el DICOMDIR perquè el path %1 conte un element amb el nom DICOM o DICOMDIR")
                     .arg(pathFolderContentToCopyToDICOMDIR));
        state.setStatus("", false, 4003);
        return state;
    }

    DicomMask studyMask;
    QList<Study*> studyList;

    LocalDatabaseManager localDatabaseManager;

    foreach (StudyToConvert studyToConvert, m_studiesToConvert)
    {
        studyMask.setStudyInstanceUID(studyToConvert.studyUID);
        Patient *patient = localDatabaseManager.retrieve(studyMask);
        if (localDatabaseManager.getLastError() != LocalDatabaseManager::Ok)
        {
            ERROR_LOG(QString("No s'han trobat les dades del estudi %1 a la base de dades ").arg(studyToConvert.studyUID));
            QString error = QString("Error al fer un retrieve study per generar un DICOMDIR; Error: %1; StudyUID: %2")
                    .arg(localDatabaseManager.getLastError())
                    .arg(studyToConvert.studyUID);
            state.setStatus(error, false, -1);
            break;
        }
        else
        {
            state.setStatus("", true, -1);
        }

        // \TODO Això de patient->getStudies().first s'ha de fer perquè queryPatientStudy retorna llista de Patients
        // Nosaltres, en realitat només en volem un sol study.
        Study *study = patient->getStudies().constFirst();
        studyList.append(study);
        foreach (Series *series, study->getSeries())
        {
            totalNumberOfItems += series->getNumberOfItems();
        }
    }

    if (!state.good())
    {
        DirectoryUtilities().deleteDirectory(m_dicomDirPath, false);
        return state;
    }

    // Sumem una imatge més per evitar que arribi el 100 % la progress bar, i així s'esperi a que es crei el dicomdir, que es fa quan s'invoca
    // createDicomdir.Create()
    m_progress = new QProgressDialog(tr("Creating DICOMDIR..."), "", 0, totalNumberOfItems + 1);
    m_progress->setMinimumDuration(0);
    m_progress->setCancelButton(0);
    m_progress->setModal(true);

    if (m_anonymizeDICOMDIR)
    {
        INFO_LOG("El DICOMDIR es creara anonimitzat");
        m_DICOMAnonymizer = new DICOMAnonymizer();
        m_DICOMAnonymizer->setPatientNameAnonymized(m_patientNameAnonymized);
        m_DICOMAnonymizer->setReplacePatientIDInsteadOfRemove(true);
        m_DICOMAnonymizer->setReplaceStudyIDInsteadOfRemove(true);
        m_DICOMAnonymizer->setRemovePrivateTags(true);
    }
    else
    {
        m_DICOMAnonymizer = NULL;
    }

    // Copiem les imatges dels estudis seleccionats al directori desti
    state = copyStudiesToDicomdirPath(studyList);

    if (!state.good())
    {
        m_progress->close();
        DirectoryUtilities().deleteDirectory(m_dicomDirPath, false);
        return state;
    }
    // Una vegada copiada les imatges les creem
    state = createDicomdir(m_dicomDirPath, selectedDevice);

    if (!state.good() && state.code() != 4001)
    {
        // L'error 4001 és que les imatges no compleixen l'estàndard al 100, però el dicomdir es pot utilitzar
        DirectoryUtilities().deleteDirectory(m_dicomDirPath, false);
    }
    else
    {
        if (copyFolderContent)
        {
            // Copiar el contingut del directory s'ha de fer una vegada s'hagi creat el DICOMDIR, perquè si dcmtk detecta al directori on s'ha de crear
            // el DICOMDIR que hi ha fitxers no DICOM fallarà.

            if (!copyFolderContentToDICOMDIR())
            {
                m_progress->close();
                DirectoryUtilities().deleteDirectory(m_dicomDirPath, false);
                state.setStatus("", false, 4002);
                return state;
            }
        }

        createReadmeTxt();
    }

    m_progress->close();

    if (m_DICOMAnonymizer)
    {
        delete m_DICOMAnonymizer;
        m_DICOMAnonymizer = NULL;
    }

    if (m_progress)
    {
        delete m_progress;
        m_progress = NULL;
    }

    return state;
}

Status ConvertToDicomdir::createDicomdir(const QString &dicomdirPath, CreateDicomdir::recordDeviceDicomDir selectedDevice)
{
    CreateDicomdir createDicomdir;
    Status state, stateNotDicomConformance;

    // El DICOM indica per cada profile les imatges han de ser d'uns determinats transfer syntax, per exemple per STD-GEN-CD han de ser Explicit
    // Little Endian per STD-GEN-DVD-JPEG poden ser Explicit Little Endian i suporten ademés alguns transfer syntax JPEG lossy i lossless. El que tenen
    // en comú tots els profiles és que suporten Explicit Little Endian, per això si no convertim els imatges a Little Endian abans de generar el DICOMDIR
    // ens podem trobar que contingui alguna imatge amb alguna transfer syntax, que segons la normativa dicom no sigui acceptada per aquest profile, per això
    // indiquem que no es comprovi la transfer syntax.
    // TENCIÓ si deshabilitem la comprovació de la transfer syntax podem tenir DICOMDIR que no siguin DICOM conformance
    createDicomdir.setCheckTransferSyntax(getConvertDicomdirImagesToLittleEndian());

    createDicomdir.setDevice(selectedDevice);
    // Invoquem el mètode per convertir el directori destí Dicomdir on ja s'han copiat les imatges en un dicomdir
    state = createDicomdir.create(dicomdirPath);
    // Ha fallat crear el dicomdir, ara intentem crear-lo en mode no estricte
    if (!state.good())
    {
        WARN_LOG("Algunes de les imatges no compleixen l'estandard DICOM al 100% es provara de crear el DICOMDIR sense el mode estricte");
        createDicomdir.setStrictMode(false);
        state = createDicomdir.create(dicomdirPath);
        if (state.good())
        {
            return stateNotDicomConformance.setStatus("Alguna de les imatges no complia l'estàndard DICOM", false, 4001);
        }
    }

    // Hem assignat com a valor de progressbar Numero Imatges +1, el +1 és el pas de convertir els fitxers a dicomdir
    m_progress->setValue(m_progress->value() + 1);

    return state;
}

void ConvertToDicomdir::setConvertDicomdirImagesToLittleEndian(bool convertDicomdirImagesToLittleEndian)
{
    m_convertDicomdirImagesToLittleEndian = convertDicomdirImagesToLittleEndian;

    if (convertDicomdirImagesToLittleEndian)
    {
        INFO_LOG("Les imatges del dicomdir es transformaran a LittleEndian");
    }
    else
    {
        INFO_LOG("Les imatges del dicomdir mantindran la seva transfer syntax");
    }
}

bool ConvertToDicomdir::getConvertDicomdirImagesToLittleEndian()
{
    return m_convertDicomdirImagesToLittleEndian;
}

bool ConvertToDicomdir::AreValidRequirementsOfFolderContentToCopyToDICOMDIR(QString path)
{
    QDir dir(path);
    QStringList directoryContent = dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);

    return !directoryContent.contains("DICOMDIR", Qt::CaseInsensitive) &&
           !directoryContent.contains("DICOM", Qt::CaseInsensitive);
}

Status ConvertToDicomdir::copyStudiesToDicomdirPath(QList<Study*> studyList)
{
    StudyToConvert studyToConvert;
    QString m_OldPatientId;
    QString patientNameDir;
    QDir patientDir;
    Status state;
    QChar fillChar = '0';
    Study *study;

    m_patient = 0;

    // Agrupem estudis1 per pacient, com que tenim la llista ordenada per patientId
    while (!m_studiesToConvert.isEmpty())
    {
        studyToConvert = m_studiesToConvert.takeFirst();

        // \TODO: Es fa xapussa per anar ràpid. En realitat s'hauria de refer el mètode perquè funcionés amb una llista d'Study i no
        // mantenir dues llistes, una d'uids i l'altra d'study's. Ara, com que creem studyList a partir de m_studiesToConvert, ens aprofitem
        // de que hi tenim el mateix ordre.
        study = studyList.takeFirst();

        if (study->getInstanceUID() != studyToConvert.studyUID)
        {
            state.setStatus("La xapussa del copyStudiesToDicomdirPath no funciona, hi ha ordre diferent entre la llista studyList i m_studiesToConvert",
                            false, -1);
            break;
        }

        // Si el pacient es diferent creem un nou directori PAtient
        if (m_OldPatientId != studyToConvert.patientId)
        {
            patientNameDir = QString("/PAT%1").arg(m_patient, 5, 10, fillChar);
            m_dicomdirPatientPath = m_dicomDirPath + "/DICOM/" + patientNameDir;
            patientDir.mkpath(m_dicomdirPatientPath);
            m_patient++;
            m_study = 0;
            // Creem una llista amb els directoris creats, per si es produeix algun error esborrar-los
            m_patientDirectories.push_back(m_dicomdirPatientPath);
        }

        state = copyStudyToDicomdirPath(study);

        delete study;
        if (!state.good())
        {
            break;
        }
        m_OldPatientId = studyToConvert.patientId;
    }

    return state;
}

Status ConvertToDicomdir::copyStudyToDicomdirPath(Study *study)
{
    // Creem el directori de l'estudi on es mourà un estudi seleccionat per convertir a dicomdir
    QDir studyDir;
    QChar fillChar = '0';
    QString studyName = QString("/STU%1").arg(m_study, 5, 10, fillChar);
    Status state;

    m_study++;
    m_series = 0;

    // Creem el directori on es guardar l'estudi en format DicomDir
    m_dicomDirStudyPath = m_dicomdirPatientPath + studyName;
    studyDir.mkdir(m_dicomDirStudyPath);

    // Per cada sèrie de l'estudi, creem el directori de la sèrie
    foreach (Series *series, study->getSeries())
    {
        if (series->getNumberOfItems() > 0)
        {
            state = copySeriesToDicomdirPath(series);

            if (!state.good())
            {
                break;
            }
        }
    }

    return state;
}

Status ConvertToDicomdir::copySeriesToDicomdirPath(Series *series)
{
    QDir seriesDir;
    QChar fillChar = '0';
    // Creem el nom del directori de la sèrie, el format és SERXXXXX, on XXXXX és el numero de sèrie dins l'estudi
    QString seriesName = QString("/SER%1").arg(m_series, 5, 10, fillChar);
    Status state;

    m_series++;
    // Creem el directori on es guardarà la sèrie en format DicomDir
    m_dicomDirSeriesPath = m_dicomDirStudyPath + seriesName;
    seriesDir.mkdir(m_dicomDirSeriesPath);

    state = copyImages(series->getImages());

    return state;
}

Status ConvertToDicomdir::copyImages(QList<Image*> images)
{
    Status state;
    m_currentItemNumber = 0;
    // HACK per evitar els casos en que siguin imatges procedents d'un multiframe
    // que copiem més d'una vegada un arxiu
    QString lastPath;
    foreach (Image *imageToCopy, images)
    {
        if (lastPath != imageToCopy->getPath())
        {
            lastPath = imageToCopy->getPath();
            
            m_currentItemNumber++;
            state = copyImageToDicomdirPath(imageToCopy);
            
            // La barra de progrés avança
            m_progress->setValue(m_progress->value() + 1);
            m_progress->repaint();
            
            if (!state.good())
            {
                break;
            }
        }
    }

    return state;
}

Status ConvertToDicomdir::copyImageToDicomdirPath(Image *image)
{
    // Creem el nom del fitxer de l'imatge, el format és IMGXXXXX, on XXXXX és el numero d'imatge dins la sèrie
    QString imageOutputPath = getCurrentItemOutputPath();
    Status state;

    if (getConvertDicomdirImagesToLittleEndian())
    {
        // Convertim la imatge a littleEndian, demanat per la normativa DICOM i la guardem al directori desti
        state = ConvertDicomToLittleEndian().convert(image->getPath(), imageOutputPath);

        if (m_anonymizeDICOMDIR && state.good())
        {
            anonymizeFile(imageOutputPath, imageOutputPath, state, true);
        }
    }
    else
    {
        // Si hem d'anonimitzar el fitxer el que fem és que en comptes de copiar-lo i llavors anonimitzar-lo, és  indicar-li al mètode d'anonimitzar
        // que guardi el fitxer en el lloc on s'hauria hagut de copiar per crear el DICOMDIR, d'aquesta manera la creació de DICOMDIR per imatges
        // que no s'han de convertir a LittleEndian és més ràpid.
        if (m_anonymizeDICOMDIR)
        {
            anonymizeFile(image->getPath(), imageOutputPath, state, false);
        }
        else
        {
            copyFileToDICOMDIRDestination(image->getPath(), imageOutputPath, state);
        }
    }

    return state;
}

QString ConvertToDicomdir::getDICOMDIROutputFilenamePrefix() const
{
    return "IMG";
}

QString ConvertToDicomdir::getCurrentItemOutputPath()
{
    return m_dicomDirSeriesPath + QString("/%1%2").arg(getDICOMDIROutputFilenamePrefix()).arg(m_currentItemNumber, 5, 10, QChar('0'));
}

void ConvertToDicomdir::copyFileToDICOMDIRDestination(const QString &sourceFile, const QString &destinationFile, Status &status)
{
    if (QFile::copy(sourceFile, destinationFile))
    {
        status.setStatus("", true, 0);
    }
    else
    {
        QString errorString = QString("Unable to copy file from %1 to %2").arg(sourceFile, destinationFile);
        ERROR_LOG(qPrintable(errorString));
        status.setStatus(errorString, false, 3001);
    }
}

void ConvertToDicomdir::anonymizeFile(const QString &sourceFile, const QString &destinationFile, Status &status, bool isLittleEndian)
{
    if (m_DICOMAnonymizer->anonymizeDICOMFile(sourceFile, destinationFile))
    {
        status.setStatus("", true, 0);
    }
    else
    {
        QString statusMessage;
        if (isLittleEndian)
        {
            statusMessage = QString("Unable to anonymize Little Endian Image %1").arg(sourceFile);
        }
        else
        {
            statusMessage = QString("Unable to anonymize file %1 to %2").arg(sourceFile, destinationFile);
        }
        
        status.setStatus(statusMessage, false, 3003);
    }
}

void ConvertToDicomdir::createReadmeTxt()
{
    Settings settings;
    QString readmeFilePath = m_dicomDirPath + "/README.TXT";
    QFile file(readmeFilePath);

    if (file.exists())
    {
        INFO_LOG("Ja s'ha trobat un fitxer readme.txt al DICOMDIR provinent del contingut de la carpeta a copiar");
        // Si el fitxer ja existeix vol dir que l'hem copiat del contingut la carpeta que s'ha copiar al crear un DICOMDIR
        // en aquest cas mantenim el Readme.txt existent i no generem el nostre
        return;
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream out(&file);
    out << "The DICOMDIR has been generated with " << ApplicationNameString << " Version " << StarviewerVersionString << "\n";
    out << "E-mail contact : " << OrganizationEmailString << "\n";
    out << ApplicationNameString << " is not responsible for DICOMDIR content." << "\n\n";

    if (!settings.getValue(InputOutputSettings::InstitutionName).toString().isEmpty())
    {
        out << "The DICOMDIR has been created by: " << settings.getValue(InputOutputSettings::InstitutionName).toString() << "\n";

        if (!settings.getValue(InputOutputSettings::InstitutionAddress).toString().isEmpty())
        {
            out << settings.getValue(InputOutputSettings::InstitutionAddress).toString() << "\n";
        }
        if (!settings.getValue(InputOutputSettings::InstitutionZipCode).toString().isEmpty())
        {
            out << settings.getValue(InputOutputSettings::InstitutionZipCode).toString() << " - ";
        }
        if (!settings.getValue(InputOutputSettings::InstitutionTown).toString().isEmpty())
        {
            out << settings.getValue(InputOutputSettings::InstitutionTown).toString() << "\n";
        }
        if (!settings.getValue(InputOutputSettings::InstitutionCountry).toString().isEmpty())
        {
            out << settings.getValue(InputOutputSettings::InstitutionCountry).toString() << "\n";
        }
        if (!settings.getValue(InputOutputSettings::InstitutionPhoneNumber).toString().isEmpty())
        {
            out << settings.getValue(InputOutputSettings::InstitutionPhoneNumber).toString() << "\n";
        }
        if (!settings.getValue(InputOutputSettings::InstitutionEmail).toString().isEmpty())
        {
            out << settings.getValue(InputOutputSettings::InstitutionEmail).toString() << "\n";
        }
    }

    file.close();
}

bool ConvertToDicomdir::copyFolderContentToDICOMDIR()
{
    // TODO:Aquest tall de codi s'hauria de copiar a una Manager de DICOMDIR no hauria d'estar aquí a la UI
    QString folderToCopyPath = Settings().getValue(InputOutputSettings::DICOMDIRFolderPathToCopy).toString();
    bool ok = true;

    INFO_LOG("Es copiara al DICOMDIR el contingut de la carpeta " + folderToCopyPath);

    if (!DirectoryUtilities::copyDirectory(folderToCopyPath, m_dicomDirPath))
    {
        ERROR_LOG(QString("No s'ha pogut copiar el visor DICOM %1 al DICOMDIR %2").arg(folderToCopyPath, m_dicomDirPath));
        ok = false;
    }

    return ok;
}

}
