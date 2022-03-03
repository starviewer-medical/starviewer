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

#ifndef UDG_WADOSEARCHSTUDYOPERATIONRESULT_H
#define UDG_WADOSEARCHSTUDYOPERATIONRESULT_H

#include "studyoperationresult.h"

namespace udg {

class WadoSearchRequest;

/**
 * @brief The WadoSearchStudyOperationResult class is the subclass of StudyOperationResult for WADO Search operations.
 */
class WadoSearchStudyOperationResult : public StudyOperationResult
{
    Q_OBJECT

public:
    /// Stores a reference to the request and other needed data and creates the needed connections to the request's signals.
    explicit WadoSearchStudyOperationResult(WadoSearchRequest *request, QObject *parent = nullptr);
    ~WadoSearchStudyOperationResult() override;

    /// Returns the operation type.
    OperationType getOperationType() const override;

    /// Requests to cancel the underlying operation.
    void cancel() override;

private slots:
    /// Emits the started signal.
    void onRequestStarted();
    /// Gets the results and errors from the request.
    void onRequestFinished();
    /// Called when the request is finally cancelled.
    void onRequestCancelled();

private:
    /// Pointer to the underlying request object.
    WadoSearchRequest *m_request;
};

} // namespace udg

#endif // UDG_WADOSEARCHSTUDYOPERATIONRESULT_H
