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

#include "downloadedfilesprocessor.h"

#include "dicomtagreader.h"
#include "directoryutilities.h"
#include "logging.h"
#include "patient.h"
#include "patientfiller.h"
#include "studyoperationsservice.h"

namespace udg {

DownloadedFilesProcessor::DownloadedFilesProcessor(const PacsDevice &pacsDevice, QObject *parent)
    : QObject(parent)
{
    DICOMSource dicomSource;
    dicomSource.addRetrievePACS(pacsDevice);

    m_patientFiller = new PatientFiller(dicomSource);
    m_patientFiller->moveToThread(&m_thread);
    m_thread.start();

    connect(this, &DownloadedFilesProcessor::processFileCalled, m_patientFiller, &PatientFiller::processDICOMFile);
    connect(this, &DownloadedFilesProcessor::finishDownloadStudyCalled, m_patientFiller, &PatientFiller::finishFilesProcessing);

    connect(m_patientFiller, &PatientFiller::patientProcessed,
            &m_localDatabaseManager, static_cast<void(LocalDatabaseManager::*)(Patient*)>(&LocalDatabaseManager::save), Qt::DirectConnection);
    connect(m_patientFiller, &PatientFiller::patientProcessed, &m_thread, &QThread::quit, Qt::DirectConnection);
}

DownloadedFilesProcessor::~DownloadedFilesProcessor()
{
    m_thread.quit();
    m_thread.wait();
    delete m_patientFiller;
}

void DownloadedFilesProcessor::beginDownloadStudy(const QString &studyInstanceUid)
{
    m_studyInstanceUid = studyInstanceUid;
    StudyOperationsService::instance()->setStudyBeingRetrieved(studyInstanceUid);
}

void DownloadedFilesProcessor::processFile(const QString &path)
{
    processFile(new DICOMTagReader(path));
}

void DownloadedFilesProcessor::processFile(const DICOMTagReader *dicomTagReader)
{
    emit processFileCalled(dicomTagReader);
}

LocalDatabaseManager::LastError DownloadedFilesProcessor::finishDownloadStudy()
{
    emit finishDownloadStudyCalled();
    m_thread.wait();
    StudyOperationsService::instance()->setStudyNotBeingRetrieved(m_studyInstanceUid);

    return m_localDatabaseManager.getLastError();
}

void DownloadedFilesProcessor::cancelDownloadStudy()
{
    m_thread.quit();
    m_thread.wait();
    deletePartiallyDownloadedStudy();
    StudyOperationsService::instance()->setStudyNotBeingRetrieved(m_studyInstanceUid);
}

void DownloadedFilesProcessor::deletePartiallyDownloadedStudy()
{
    if (!m_studyInstanceUid.isEmpty())
    {
        if (m_localDatabaseManager.studyExists(m_studyInstanceUid))
        {
            INFO_LOG(QString("Study %1 exists in database, its directory won't be deleted.").arg(m_studyInstanceUid));
        }
        else
        {
            INFO_LOG(QString("Study %1 does not exist in database, its directory will be deleted.").arg(m_studyInstanceUid));
            DirectoryUtilities().deleteDirectory(m_localDatabaseManager.getStudyPath(m_studyInstanceUid), true);
        }
    }
}

} // namespace udg
