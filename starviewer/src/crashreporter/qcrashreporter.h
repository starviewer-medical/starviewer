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

#ifndef UDGQCRASHREPORTER_H
#define UDGQCRASHREPORTER_H

#include <QWidget>
#include <QDateTime>
#include <QNetworkReply>
#include "ui_qcrashreporterbase.h"

class QHttpMultiPart;

namespace udg {

/**
    Aquesta classe és l'encarregada de fer tota la gestió del crash reporter. Bàsicament el que fa és informar a l'usuari que l'aplicació ha petat,
    permet veure la trama del problema i enviar la informació a l'equip de desenvolupament si ho creu oportú.
  */
class QCrashReporter : public QWidget, Ui::QCrashReporterBase {
Q_OBJECT

public:
    explicit QCrashReporter(const QStringList &argv, QWidget *parent = 0);
    virtual ~QCrashReporter();

protected:
    virtual void closeEvent(QCloseEvent* event);

private slots:
    void onQuitPushButtonClicked();
    void onRestartPushButtonClicked();
    void onAbortSendPushButtonClicked();
    void onReplyError(QNetworkReply::NetworkError code);
    void onReplyFinished();
    void onReplyUploadProgress(qint64 bytesReceived, qint64 bytesTotal);
    /// No SSL is used, so all SSL errors are ignored.
    void onManagerSslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

    void maybeSendReport();

signals:
    void resendReport();

private:

    /// Restart executable
    bool restart();
    void sendReport();

    /// Restart on exit
    bool m_doRestart = false;
    /// Close the window on close() event.
    bool m_acceptClose = false;
    /// Indicates if crash report has been sent successfully
    bool m_successful = false;
    /// Indicates that timer has timed out, so we can definetively close.
    bool m_timerStarted = false;
    /// Show error and success message boxes to the user (might be more intrusive)
    static constexpr bool m_enableMsgboxes = false;
    QString m_minidumpUUID;
    QString m_minidumpPath;
    QString m_minidumpFilename;
    QNetworkReply* m_reply;
    QHttpMultiPart* m_multipart;
    QFile* m_dumpQFile;
    QNetworkAccessManager* m_manager;
    QTimer* m_closeTimer;

    /// Set on the constructor in order to precisely specify the time of the crash (this is motivated by the fact that an user may left the crash reporting window several minutes.
    QDateTime m_crashTime;

};

}

#endif
