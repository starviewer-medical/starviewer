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

#ifndef UDG_STUDYOPERATIONSSERVICE_H
#define UDG_STUDYOPERATIONSSERVICE_H

#include <QObject>
#include "singleton.h"

#include "studyoperations.h"

#include <QMutex>

namespace udg {

class DicomMask;
class PacsDevice;
class PacsManager;
class Series;
class Study;
class StudyOperationResult;
class WadoRequestManager;

/**
 * @brief The StudyOperationsService class is a singleton that allows to search local and remote studies and transfer studies from or to PACS.
 *
 * All operations are asynchronous.
 */
class StudyOperationsService : public QObject, public Singleton<StudyOperationsService>
{
    Q_OBJECT

public:
    /// Starts an asynchronous search on the given PACS with the given mask and searching for the given target resource. Returns a result that can be used
    /// to observe the progress and obtain the final values.
    StudyOperationResult* searchPacs(const PacsDevice &pacs, const DicomMask &mask, StudyOperations::TargetResource targetResource);

    /// Starts an asynchronous retrieve request on the given PACS for the given study, or the series or instance identified by the given UIDs. Returns a result
    /// that can be used to observe the progres and obtain the final values.
    StudyOperationResult* retrieveFromPacs(const PacsDevice &pacs, const Study *study, const QString &seriesInstanceUid = QString(),
                                           const QString &sopInstanceUid = QString(),
                                           StudyOperations::RetrievePriority priority = StudyOperations::RetrievePriority::Medium);

    /// Starts an asynchronous store request on the given PACS for the given study. Returns a result that can be used to observe the progress and obtain any
    /// errors.
    StudyOperationResult* storeInPacs(const PacsDevice &pacs, const Study *study);

    /// Starts an asynchronous store request on the given PACS for the given list of series; all series must belong to the same study. Returns a result that can
    /// be used to observe the progress and obtain any errors.
    StudyOperationResult* storeInPacs(const PacsDevice &pacs, const QList<Series*> &series);

    /// Saves a setting to know that a study with the given UID is being retrieved. This is saved in order to delete a half-downloaded study in case the
    /// application crashes in the middle of a download.
    void setStudyBeingRetrieved(const QString &studyInstanceUid);
    /// Removes the given Study Instance UID from the setting set in the above method to indicate that it is no longer being retrieved.
    void setStudyNotBeingRetrieved(const QString &studyInstanceUid);
    /// Returns true if one or more studies are being retrieved and false otherwise.
    bool areStudiesBeingRetrieved();
    /// If there are studies marked as being retrieved, this method will delete their images and leave the database in a consistent state. This method is
    /// intended to delete half-downloaded studies in case the application crashes in the middle of a download. It should be called at the start of the
    /// application.
    void deleteStudiesBeingRetrieved();

public slots:
    /// Requests to call all current operations.
    void cancelAllOperations();

signals:
    /// Emitted when any operation is requested.
    void operationRequested(StudyOperationResult *result);

    /// Emitted when a retrieve operation is going to delete a local study to free space.
    void localStudyAboutToBeDeleted(QString studyInstanceUid);

private:
    explicit StudyOperationsService(QObject *parent = nullptr);
    ~StudyOperationsService() override;
    friend Singleton<StudyOperationsService>;

private:
    /// Used to perform DIMSE operations.
    PacsManager *m_pacsManager;
    /// Thread where WADO operations are initiated.
    QThread m_wadoThread;
    /// Used to perform WADO operations.
    WadoRequestManager *m_wadoRequestManager;
    /// Mutex used when modifying the studies being retrieved.
    QMutex m_mutex;
};

} // namespace udg

#endif // UDG_STUDYOPERATIONSSERVICE_H
