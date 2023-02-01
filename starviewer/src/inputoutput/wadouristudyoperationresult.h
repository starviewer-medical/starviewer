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

#ifndef UDG_WADOURISTUDYOPERATIONRESULT_H
#define UDG_WADOURISTUDYOPERATIONRESULT_H

#include "studyoperationresult.h"

namespace udg {

class WadoUriRequest;

/**
 * @brief The WadoUriStudyOperationResult class is the subclass of StudyOperationResult for WADO-URI operations.
 */
class WadoUriStudyOperationResult : public StudyOperationResult
{
    Q_OBJECT

public:
    /// Stores a reference to the request and other needed data and creates the needed connections to the request's signals.
    explicit WadoUriStudyOperationResult(WadoUriRequest *request, QObject *parent = nullptr);
    ~WadoUriStudyOperationResult() override;

    /// Returns the operation type.
    OperationType getOperationType() const override;

    /// Sets the request study.
    void setRequestStudy(const Study *requestStudy);

    /// Cancels the underlying request.
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
    WadoUriRequest *m_request;
};

} // namespace udg

#endif // UDG_WADOURISTUDYOPERATIONRESULT_H
