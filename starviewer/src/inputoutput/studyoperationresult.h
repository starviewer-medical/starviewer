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

#ifndef UDG_STUDYOPERATIONRESULT_H
#define UDG_STUDYOPERATIONRESULT_H

#include <QObject>

#include <future>

namespace udg {

class Image;
class Patient;
class Series;

/**
 * @brief The StudyOperationResult class is the base class used for representing a StudyOperationsService's result.
 *
 * It allows to obtain the result of the operation either synchronously (using getters directly) or asynchronously (listening to its signals and using getters
 * only when the result is ready). It also allows to cancel the operation.
 *
 * The tool to allow this sync or async operation is the combination of std::promise and std::future. There is a communication channel between both where
 * promise is the entry (write-only) and future is the exit (read-only). A read from a future not yet ready blocks until it is ready. A future is made ready
 * setting a value to the corresponding promise. Read https://en.cppreference.com/w/cpp/header/future for more information.
 */
class StudyOperationResult : public QObject
{
    Q_OBJECT

public:
    /// Types of result that can be represented by StudyOperationResult.
    enum class ResultType { Studies, Series, Instances, Error, Nothing };

    /// Creates an unfinished result. Most getters will block if it's not finished.
    explicit StudyOperationResult(QObject *parent = nullptr);

    /// Returns the type of result contained by this object, which determines which getters make sense.
    ResultType getResultType() const;

    /// Returns a list of studies. It is in fact a list of patients due to the program structure.
    const QList<Patient*>& getStudies() const;
    /// Returns a list of series.
    const QList<Series*>& getSeries() const;
    /// Returns a list of instances. The Image class is used due to the program structure but it does not imply that they are actually images.
    const QList<Image*>& getInstances() const;

    /// Returns the description of an encountered error, if any.
    const QString& getErrorText() const;

    /// Returns the Study Instance UID of the study to which series or instances belong.
    virtual QString getStudyInstanceUid() const = 0;
    /// Returns the Series Instance UID of the series to which instances belong.
    virtual QString getSeriesInstanceUid() const = 0;

    /// Requests to cancel the underlying operation.
    virtual void cancel() = 0;

signals:
    /// Emitted when the operation has finished without errors.
    void finishedSuccessfully(StudyOperationResult *result);
    /// Emitted when the operation has finished with some error.
    void finishedWithError(StudyOperationResult *result);
    /// Emitted when the operation has been cancelled.
    void cancelled(StudyOperationResult *result);

protected:
    /// Must be called by subclasses to set the given studies as the result.
    void setStudies(QList<Patient*> studies);
    /// Must be called by subclasses to set the given series as the result.
    void setSeries(QList<Series*> series);
    /// Must be called by subclasses to set the given instances as the result.
    void setInstances(QList<Image*> instances);
    /// Must be called by subclasses to set the given error as the result.
    void setErrorText(QString text);
    /// Must be called by subclasses to mark the operation as cancelled.
    void setCancelled();

private:
    /// Type of result contained.
    ResultType m_resultType;

    /// Resulting studies.
    QList<Patient*> m_studies;
    /// Resulting series.
    QList<Series*> m_series;
    /// Resulting instances.
    QList<Image*> m_instances;

    /// Error description, if any.
    QString m_errorText;

    /// Used to unblock getters when a result is available.
    std::promise<void> m_promise;
    /// Used to block getters until the underlying operation finishes and a result is available.
    std::future<void> m_future;
};

} // namespace udg

#endif // UDG_STUDYOPERATIONRESULT_H
