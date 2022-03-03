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

#ifndef UDG_WADOSEARCHREQUEST_H
#define UDG_WADOSEARCHREQUEST_H

#include "wadorequest.h"

#include "dicommask.h"

#include <QList>

namespace udg {

namespace StudyOperations {
enum class TargetResource;
}

class Patient;

/**
 * @brief The WadoSearchRequest class implements the WADO Search transaction (PS3.18§10.6).
 */
class WadoSearchRequest : public WadoRequest
{
    Q_OBJECT

public:
    explicit WadoSearchRequest(const PacsDevice &pacs, const DicomMask &mask, StudyOperations::TargetResource targetResource, QObject *parent = nullptr);

    /// Returns the mask with the query parameters.
    const DicomMask& getDicomMask() const;
    /// Returns the target resource that will be requested.
    StudyOperations::TargetResource getTargetResource() const;

    /// Returns the studies obtained from the search, if any.
    const QList<Patient*>& getStudies() const;
    /// Returns the series obtained from the search, if any.
    const QList<Series*>& getSeries() const;
    /// Returns the instances obtained from the search, if any.
    const QList<Image*>& getInstances() const;

private:
    /// Starts the search transaction.
    void startInternal() override;

private slots:
    /// Called when the underlying QNetworkReply finishes.
    void onReplyFinished();

private:
    /// Mask with the query parameters.
    DicomMask m_mask;
    /// Target resource that will be requested.
    StudyOperations::TargetResource m_targetResource;

    /// Studies obtained from the search, if any.
    QList<Patient*> m_studies;
    /// Series obtained from the search, if any.
    QList<Series*> m_series;
    /// Instances obtained from the search, if any.
    QList<Image*> m_instances;
};

} // namespace udg

#endif // UDG_WADOSEARCHREQUEST_H
