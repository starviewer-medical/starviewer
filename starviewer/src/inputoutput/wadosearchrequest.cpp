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

#include "wadosearchrequest.h"

#include "dicomdictionary.h"
#include "dicomjsondecoder.h"
#include "logging.h"
#include "studyoperations.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>

namespace udg {

namespace {

// Builds and returns a WADO query string from the given mask.
QUrlQuery buildQueryString(const DicomMask &mask)
{
    QUrlQuery query;

    if (!mask.getPatientName().isEmpty())
    {
        query.addQueryItem(DICOMPatientName.getName(), mask.getPatientName());
    }
    if (!mask.getPatientID().isEmpty())
    {
        query.addQueryItem(DICOMPatientID.getName(), mask.getPatientID());
    }
    if (!mask.getPatientBirthRangeAsDICOMFormat().isEmpty())
    {
        query.addQueryItem(DICOMPatientBirthDate.getName(), mask.getPatientBirthRangeAsDICOMFormat());
    }
    if (!mask.getPatientSex().isEmpty())
    {
        query.addQueryItem(DICOMPatientSex.getName(), mask.getPatientSex());
    }
    if (!mask.getPatientAge().isEmpty())
    {
        query.addQueryItem(DICOMPatientAge.getName(), mask.getPatientAge());
    }

    if (!mask.getStudyInstanceUID().isEmpty())
    {
        query.addQueryItem(DICOMStudyInstanceUID.getName(), mask.getStudyInstanceUID());
    }
    if (!mask.getStudyID().isEmpty())
    {
        query.addQueryItem(DICOMStudyID.getName(), mask.getStudyID());
    }
    if (!mask.getStudyDateRangeAsDICOMFormat().isEmpty())
    {
        query.addQueryItem(DICOMStudyDate.getName(), mask.getStudyDateRangeAsDICOMFormat());
    }
    if (!mask.getStudyTimeRangeAsDICOMFormat().isEmpty())
    {
        query.addQueryItem(DICOMStudyTime.getName(), mask.getStudyTimeRangeAsDICOMFormat());
    }
    if (!mask.getStudyDescription().isEmpty())
    {
        query.addQueryItem(DICOMStudyDescription.getName(), mask.getStudyDescription());
    }
    if (!mask.getStudyModality().isEmpty())
    {
        query.addQueryItem(DICOMModalitiesInStudy.getName(), mask.getStudyModality());
    }
    if (!mask.getAccessionNumber().isEmpty())
    {
        query.addQueryItem(DICOMAccessionNumber.getName(), mask.getAccessionNumber());
    }
    if (!mask.getReferringPhysiciansName().isEmpty())
    {
        query.addQueryItem(DICOMReferringPhysicianName.getName(), mask.getReferringPhysiciansName());
    }

    if (!mask.getSeriesInstanceUID().isEmpty())
    {
        query.addQueryItem(DICOMSeriesInstanceUID.getName(), mask.getSeriesInstanceUID());
    }
    if (!mask.getSeriesNumber().isEmpty())
    {
        query.addQueryItem(DICOMSeriesNumber.getName(), mask.getSeriesNumber());
    }
    if (!mask.getSeriesDateRangeAsDICOMFormat().isEmpty())
    {
        query.addQueryItem(DICOMSeriesDate.getName(), mask.getSeriesDateRangeAsDICOMFormat());
    }
    if (!mask.getSeriesTimeRangeAsDICOMFormat().isEmpty())
    {
        query.addQueryItem(DICOMSeriesTime.getName(), mask.getSeriesTimeRangeAsDICOMFormat());
    }
    if (!mask.getSeriesModality().isEmpty())
    {
        query.addQueryItem(DICOMModality.getName(), mask.getSeriesModality());
    }
    if (!mask.getSeriesDescription().isEmpty())
    {
        query.addQueryItem(DICOMSeriesDescription.getName(), mask.getSeriesDescription());
    }
    if (!mask.getSeriesProtocolName().isEmpty())
    {
        query.addQueryItem(DICOMProtocolName.getName(), mask.getSeriesProtocolName());
    }
    if (!mask.getRequestedProcedureID().isEmpty())
    {
        query.addQueryItem(DICOMRequestAttributesSequence.getName() + "." + DICOMRequestedProcedureID.getName(), mask.getRequestedProcedureID());
    }
    if (!mask.getScheduledProcedureStepID().isEmpty())
    {
        query.addQueryItem(DICOMRequestAttributesSequence.getName() + "." + DICOMScheduledProcedureStepID.getName(), mask.getScheduledProcedureStepID());
    }
    if (!mask.getPPSStartDateAsRangeDICOMFormat().isEmpty())
    {
        query.addQueryItem(DICOMPerformedProcedureStepStartDate.getName(), mask.getPPSStartDateAsRangeDICOMFormat());
    }
    if (!mask.getPPSStartTimeAsRangeDICOMFormat().isEmpty())
    {
        query.addQueryItem(DICOMPerformedProcedureStepStartTime.getName(), mask.getPPSStartTimeAsRangeDICOMFormat());
    }

    if (!mask.getSOPInstanceUID().isEmpty())
    {
        query.addQueryItem(DICOMSOPInstanceUID.getName(), mask.getSOPInstanceUID());
    }
    if (!mask.getImageNumber().isEmpty())
    {
        query.addQueryItem(DICOMInstanceNumber.getName(), mask.getImageNumber());
    }

    // All fields used in QStudyTreeWidget
    static const QStringList FieldsToInclude{
        // For studies
        // DICOMStudyDate.getName(), DICOMStudyTime.getName(), DICOMAccessionNumber.getName(), DICOMModalitiesInStudy.getName(),
        // DICOMReferringPhysicianName.getName(), DICOMPatientName.getName(), DICOMPatientID.getName(), DICOMPatientBirthDate.getName(),
        // DICOMStudyInstanceUID.getName(), DICOMStudyID.getName(),
        // All the above required by the standard to be present, no need to include them here
        DICOMInstitutionName.getName(), DICOMStudyDescription.getName(), DICOMPatientAge.getName(),
        // For series
        // DICOMModality.getName(), DICOMSeriesDescription.getName(), DICOMSeriesInstanceUID.getName(), DICOMSeriesNumber.getName(),
        // DICOMPerformedProcedureStepStartDate.getName(), DICOMPerformedProcedureStepStartTime.getName(),
        // DICOMScheduledProcedureStepID.getName(), DICOMRequestedProcedureID.getName(),
        // All the above required by the standard to be present, no need to include them here
        DICOMSeriesDate.getName(), DICOMSeriesTime.getName(), DICOMProtocolName.getName()
        // For instances
        // DICOMSOPInstanceUID.getName(), DICOMInstanceNumber.getName()
        // All the above required by the standard to be present, no need to include them here
    };

    query.addQueryItem("includefield", FieldsToInclude.join(','));

    return query;
}

}

WadoSearchRequest::WadoSearchRequest(const PacsDevice &pacs, const DicomMask &mask, StudyOperations::TargetResource targetResource, QObject *parent)
    : WadoRequest(pacs, parent), m_mask(mask), m_targetResource(targetResource)
{
}

const DicomMask& WadoSearchRequest::getDicomMask() const
{
    return m_mask;
}

StudyOperations::TargetResource WadoSearchRequest::getTargetResource() const
{
    return m_targetResource;
}

const QList<Patient*>& WadoSearchRequest::getStudies() const
{
    return m_studies;
}

const QList<Series*>& WadoSearchRequest::getSeries() const
{
    return m_series;
}

const QList<Image*>& WadoSearchRequest::getInstances() const
{
    return m_instances;
}

// This is run in the WADO thread.
void WadoSearchRequest::startInternal()
{
    QString urlString = getPacsDevice().getBaseUri().toString(QUrl::StripTrailingSlash);

    switch (m_targetResource)
    {
        case StudyOperations::TargetResource::Studies:
            urlString += "/studies";
            break;
        case StudyOperations::TargetResource::Series:
            urlString += "/series";
            break;
        case StudyOperations::TargetResource::Instances:
            urlString += "/instances";
            break;
    }

    QUrl fullUrl(urlString);
    fullUrl.setQuery(buildQueryString(m_mask));

    QNetworkRequest request(fullUrl);
    request.setRawHeader("Accept", "application/json");

    m_reply = m_networkAccessManager->get(request);

    emit started();

    connect(m_reply, &QNetworkReply::finished, this, &WadoSearchRequest::onReplyFinished, Qt::DirectConnection);
}

// This is run in the WADO thread.
void WadoSearchRequest::onReplyFinished()
{
    if (m_reply->error() == QNetworkReply::NoError)
    {
        QByteArray jsonText = m_reply->readAll();

        DicomJsonDecoder dicomJsonDecoder;
        dicomJsonDecoder.setPacsDevice(getPacsDevice(), true);

        switch (m_targetResource)
        {
            case StudyOperations::TargetResource::Studies:
                m_studies = dicomJsonDecoder.getStudies(jsonText);
                break;
            case StudyOperations::TargetResource::Series:
                m_series = dicomJsonDecoder.getSeries(jsonText);
                break;
            case StudyOperations::TargetResource::Instances:
                m_instances = dicomJsonDecoder.getInstances(jsonText);
                break;
        }

        const QStringList &errors = dicomJsonDecoder.getErrors();

        for (const QString &error : errors)
        {
            ERROR_LOG(error);
        }

        if (!errors.isEmpty())
        {
            m_errorsDescription = errors.join('\n');

            if (m_studies.isEmpty() && m_series.isEmpty() && m_instances.isEmpty())
            {
                m_status = Status::Errored;
            }
            else
            {
                m_status = Status::Warnings;
            }
        }

        emit finished();
    }
    else if (m_reply->error() == QNetworkReply::OperationCanceledError)
    {
        m_status = Status::Cancelled;
        emit cancelled();
    }
    else
    {
        m_status = Status::Errored;
        m_errorsDescription = m_reply->errorString();
        ERROR_LOG(m_errorsDescription);
        emit finished();
    }
}

} // namespace udg
