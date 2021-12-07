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

#ifndef UDG_WADOREQUEST_H
#define UDG_WADOREQUEST_H

#include <QObject>

#include "pacsdevice.h"

class QNetworkAccessManager;
class QNetworkReply;

namespace udg {

/**
 * @brief The WadoRequest class is the base abstract class for all WADO operations implementations.
 *
 * Instances of its subclasses will live in the main thread but some methods will be called from the WADO thread in StudyOperationsService. More specifically,
 * a QNetworkAccessManager must be set from that other thread before calling start() from that same thread.
 */
class WadoRequest : public QObject
{
    Q_OBJECT

public:
    /// Possible status of a request.
    enum class Status { Clean, Warnings, Errored, Cancelled };

    /// Creates an instance of this class with a null QNetworkAccessManager.
    explicit WadoRequest(const PacsDevice &pacs, QObject *parent = nullptr);
    /// Deletes the internal QNetworkReply, if any.
    ~WadoRequest() override;

    /// Returns the PACS device that will receive the request.
    const PacsDevice& getPacsDevice() const;

    /// To be called from the thread where QNetworkAccessManager lives. Assigns the given QNetworkAccessManager and calls startInternal().
    void start(QNetworkAccessManager *networkAccessManager);
    /// Cancels the request.
    void cancel();

    /// Returns the status of the request.
    Status getStatus() const;
    /// Returns the description of all the errors and warnings encountered.
    const QString& getErrorsDescription() const;

signals:
    /// Emitted when the operation is started.
    void started();
    /// Emitted when the operation is finished.
    void finished();
    /// Emitted when the operaiton is cancelled.
    void cancelled();

protected:
    /// QNetworkAccessManager instance that will be used to perform WADO (HTTP) communications.
    QNetworkAccessManager *m_networkAccessManager;
    /// The QNetworkReply returned by QNetworkAccessManager can be stored here.
    QNetworkReply *m_reply;
    /// Status of this request.
    Status m_status;
    /// A string containing a description of all encountered errors.
    QString m_errorsDescription;

private:
    /// Called from start(). Starts the WADO operation using the stored QNetworkAccessManager.
    virtual void startInternal() = 0;

private:
    /// PACS that will receive the request.
    PacsDevice m_pacs;
};

} // namespace udg

#endif // UDG_WADOREQUEST_H
