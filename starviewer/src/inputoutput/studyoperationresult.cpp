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

#include "studyoperationresult.h"

#include "image.h"
#include "patient.h"
#include "series.h"

namespace udg {

namespace {

// Moves all items (QObjects) in the given list to the thread represented by the given QThread.
template<class T>
void moveAllItemsToThread(const QList<T*> list, QThread *thread)
{
    for (T *item : list)
    {
        item->moveToThread(thread);
    }
}

}

StudyOperationResult::StudyOperationResult(QObject *parent)
    : QObject(parent), m_requestLevel(RequestLevel::Studies), m_requestStudy(nullptr), m_resultType(ResultType::Nothing)
{
    m_future = m_promise.get_future();
}

const PacsDevice& StudyOperationResult::getRequestPacsDevice() const
{
    return m_requestPacsDevice;
}

StudyOperationResult::RequestLevel StudyOperationResult::getRequestLevel() const
{
    return m_requestLevel;
}

const QString& StudyOperationResult::getRequestStudyInstanceUid() const
{
    return m_requestStudyInstanceUid;
}

const QString& StudyOperationResult::getRequestSeriesInstanceUid() const
{
    return m_requestSeriesInstanceUid;
}

const QString& StudyOperationResult::getRequestSopInstanceUid() const
{
    return m_requestSopInstanceUid;
}

const Study* StudyOperationResult::getRequestStudy() const
{
    return m_requestStudy;
}

StudyOperationResult::ResultType StudyOperationResult::getResultType() const
{
    return m_resultType;
}

const QList<Patient*>& StudyOperationResult::getStudies() const
{
    if (m_future.valid())
    {
        m_future.wait();
    }

    return m_studies;
}

const QList<Series*>& StudyOperationResult::getSeries() const
{
    if (m_future.valid())
    {
        m_future.wait();
    }

    return m_series;
}

const QList<Image*>& StudyOperationResult::getInstances() const
{
    if (m_future.valid())
    {
        m_future.wait();
    }

    return m_instances;
}

const QString& StudyOperationResult::getStudyInstanceUid() const
{
    if (m_future.valid())
    {
        m_future.wait();
    }

    return m_studyInstanceUid;
}

const QString& StudyOperationResult::getErrorText() const
{
    if (m_future.valid())
    {
        m_future.wait();
    }

    return m_errorText;
}

void StudyOperationResult::setStudies(QList<Patient*> studies)
{
    if (m_future.valid())
    {
        m_resultType = ResultType::Studies;
        m_studies = std::move(studies);
        moveAllItemsToThread(m_studies, this->thread());
        m_promise.set_value();

        emit finishedSuccessfully(this);
        emit finished(this);
    }
}

void StudyOperationResult::setSeries(QList<Series*> series)
{
    if (m_future.valid())
    {
        m_resultType = ResultType::Series;
        m_series = std::move(series);
        moveAllItemsToThread(m_series, this->thread());
        m_promise.set_value();

        emit finishedSuccessfully(this);
        emit finished(this);
    }
}

void StudyOperationResult::setInstances(QList<Image*> instances)
{
    if (m_future.valid())
    {
        m_resultType = ResultType::Instances;
        m_instances = std::move(instances);
        moveAllItemsToThread(m_instances, this->thread());
        m_promise.set_value();

        emit finishedSuccessfully(this);
        emit finished(this);
    }
}

void StudyOperationResult::setStudyInstanceUid(QString studyInstanceUid, QString errorText)
{
    if (m_future.valid())
    {
        m_resultType = ResultType::StudyInstanceUid;
        m_studyInstanceUid = std::move(studyInstanceUid);
        m_errorText = std::move(errorText);
        m_promise.set_value();

        if (m_errorText.isNull())
        {
            emit finishedSuccessfully(this);
        }
        else
        {
            emit finishedWithPartialSuccess(this);
        }

        emit finished(this);
    }
}

void StudyOperationResult::setErrorText(QString text)
{
    if (m_future.valid())
    {
        m_resultType = ResultType::Error;
        m_errorText = std::move(text);
        m_promise.set_value();

        emit finishedWithError(this);
        emit finished(this);
    }
}

void StudyOperationResult::setNothing(QString errorText)
{
    if (m_future.valid())
    {
        m_errorText = std::move(errorText);
        m_promise.set_value();

        if (m_errorText.isNull())
        {
            emit finishedSuccessfully(this);
        }
        else
        {
            emit finishedWithPartialSuccess(this);
        }

        emit finished(this);
    }
}

void StudyOperationResult::setCancelled()
{
    if (m_future.valid())
    {
        m_promise.set_value();

        emit cancelled(this);
    }
}

} // namespace udg
