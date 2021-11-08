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

#ifndef UDG_DIMSERETRIEVESTUDYOPERATIONRESULT_H
#define UDG_DIMSERETRIEVESTUDYOPERATIONRESULT_H

#include "studyoperationresult.h"

#include "pacsjob.h"

namespace udg {

class PacsManager;
class RetrieveDICOMFilesFromPACSJob;

/**
 * @brief The DimseRetrieveStudyOperationResult class is the subclass of StudyOperationResult for DIMSE Retrieve operations.
 */
class DimseRetrieveStudyOperationResult : public StudyOperationResult
{
    Q_OBJECT

public:
    /// Stores references to the job and PacsManager and creates the needed connections to the job's signals.
    DimseRetrieveStudyOperationResult(PACSJobPointer job, PacsManager *manager, QObject *parent = nullptr);

    /// Returns a null QString.
    QString getSeriesInstanceUid() const override;

    /// Requests to the PacsManger that the job is cancelled.
    void cancel() override;

private slots:
    /// Gets the results or errors from the job.
    void onJobFinished();
    /// Called when the job is finally cancelled.
    void onJobCancelled();

private:
    /// Pointer to the job, needed to cancel it.
    QSharedPointer<RetrieveDICOMFilesFromPACSJob> m_job;
    /// Pointer to the PacsManager, needed to cancel the job.
    PacsManager *m_pacsManager;
};

} // namespace udg

#endif // UDG_DIMSERETRIEVESTUDYOPERATIONRESULT_H
