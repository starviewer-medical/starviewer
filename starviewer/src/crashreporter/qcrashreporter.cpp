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

#include <QDateTime>

#include "qcrashreporter.h"
#include "../core/starviewerapplication.h"
#include "logging.h"
#include "executablesnames.h"
#include <QTimer>
#include <QNetworkInterface>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QUrl>
#include <QProcess>
#include <QMessageBox>

namespace udg {

QCrashReporter::QCrashReporter(const QStringList &args, QWidget *parent)
    : QWidget(parent)
{
    m_reply = nullptr;
    m_multipart = nullptr;
    m_dumpQFile = nullptr;

    m_crashTime = QDateTime::currentDateTimeUtc();
    m_manager = new QNetworkAccessManager();
    m_closeTimer = new QTimer();
    m_closeTimer->setInterval(1500);

    setupUi(this);
    setWindowIcon(QIcon(":/images/logo/logo.ico"));

    setWindowTitle(tr("%1").arg(ApplicationNameString));
    m_quitPushButton->setText(tr("Quit %1").arg(ApplicationNameString));
    m_restartPushButton->setText(tr("Restart %1").arg(ApplicationNameString));
    m_sendReportCheckBox->setText(tr("Tell %1 about this crash").arg(ApplicationNameString));
    m_informationLabel->setText(tr("We're sorry %1 had a problem and crashed. Please take a moment to send us a crash report to help us diagnose and fix "
                                   "the problem. Your personal information is not sent with this report.  ").arg(ApplicationNameString));
    m_sendReportLabel->setVisible(false);
    m_sendProgressBar->setVisible(false);
    m_sendError->setVisible(false);
    m_sendSuccess->setVisible(false);
    m_abortSendPushButton->setVisible(false);


    m_minidumpUUID = args[2];
    m_minidumpFilename = m_minidumpUUID + ".dmp";
    m_minidumpPath = args[1] + "/" + m_minidumpFilename;

    // Busquem les adreces IP del host.
    QString ipAddresses("");

    Q_FOREACH (QNetworkInterface inter, QNetworkInterface::allInterfaces())
    {
        if (inter.flags().testFlag(QNetworkInterface::IsRunning))
        {
            Q_FOREACH (QNetworkAddressEntry entry, inter.addressEntries())
            {
                if (inter.hardwareAddress() != "00:00:00:00:00:00" && entry.ip().toString().contains(".") && entry.ip().toString() != "127.0.0.1")
                {
                    if (!ipAddresses.isEmpty())
                    {
                        ipAddresses += ", ";
                    }

                    ipAddresses += entry.ip().toString();
                }
            }
        }
    }

    QString hostInformation;
    hostInformation = QString("%1: %2").arg(tr("IP")).arg(ipAddresses);

    // En cas que estem a windows, afegim com a info del host el domini de l'usuari.
#ifdef WIN32
    QString userDomain = QProcessEnvironment::systemEnvironment().value(QString("USERDOMAIN"), QString(""));
    if (!userDomain.isEmpty())
    {
        hostInformation += QString("\n%1: %2").arg(tr("User Domain")).arg(userDomain);
    }
#endif

    m_hostInformationTextEdit->setPlainText(hostInformation);

    connect(this, SIGNAL(resendReport()), this, SLOT(maybeSendReport()));
    connect(m_closeTimer, SIGNAL(timeout()), this, SLOT(close()));

    connect(m_quitPushButton, SIGNAL(clicked(bool)), this, SLOT(onQuitPushButtonClicked()));
    connect(m_restartPushButton, SIGNAL(clicked(bool)), this, SLOT(onRestartPushButtonClicked()));
    connect(m_abortSendPushButton, SIGNAL(clicked(bool)), this, SLOT(onAbortSendPushButtonClicked()));

}

QCrashReporter::~QCrashReporter()
{
    if (m_dumpQFile || m_reply || m_multipart)
    {
        delete m_reply;
        delete m_multipart;
        delete m_dumpQFile;
    }
    delete m_manager;
    delete m_closeTimer;
}

void QCrashReporter::closeEvent(QCloseEvent* event)
{
    if (!m_sendReportCheckBox->isChecked())
    {
        if (m_doRestart)
        {
            restart();
        }
        return QWidget::closeEvent(event);
    }

    if (!m_acceptClose)
    {
        emit maybeSendReport();
        event->ignore();
    }
    else if (!m_timerStarted)
    {
        m_sendReportLabel->setVisible(false);
        m_sendProgressBar->setVisible(false);
        m_abortSendPushButton->setVisible(false);
        if (m_successful)
        {
            m_sendSuccess->setVisible(true);
        }
        else {
            m_sendError->setVisible(true);
        }
        m_closeTimer->start();
        m_timerStarted = true;
        event->ignore();
    }
    else {
        if (m_doRestart)
        {
            restart();
        }
        return QWidget::closeEvent(event);
    }

}

void QCrashReporter::onQuitPushButtonClicked()
{
    maybeSendReport();
}

void QCrashReporter::onRestartPushButtonClicked()
{
    m_doRestart = true;
    maybeSendReport();
}

void QCrashReporter::onAbortSendPushButtonClicked()
{
    m_sendReportCheckBox->setChecked(false);
    close();
}

void QCrashReporter::maybeSendReport()
{
    if (m_sendReportCheckBox->isChecked())
    {
        m_restartPushButton->setVisible(false);
        m_quitPushButton->setVisible(false);
        sendReport();
    }
    else {
        m_acceptClose = true;
        close();
    }
}

void QCrashReporter::sendReport()
{
    if (m_dumpQFile || m_reply || m_multipart)
    {
        delete m_reply;
        delete m_multipart;
        delete m_dumpQFile;
        m_reply = nullptr;
        m_multipart = nullptr;
        m_dumpQFile = nullptr;
    }

    auto createHttpPart = [] (const QString& name, const QString& value) -> QHttpPart {
        QHttpPart part;
        part.setHeader(QNetworkRequest::KnownHeaders::ContentDispositionHeader, QVariant(QString("form-data; name=\"%1\"").arg(name)));
        part.setBody(value.toUtf8());
        return part;
    };

    QString product = ApplicationNameString;
    QString version = StarviewerVersionString;
    QString guid = m_minidumpUUID;
    QString ptime = QString::number(m_crashTime.toUTC().toMSecsSinceEpoch() / 1000) ;
    QString ctime = m_crashTime.toLocalTime().toTimeSpec(Qt::TimeSpec::OffsetFromUTC).toString(Qt::DateFormat::ISODate);
    QString email = m_emailLineEdit->text();
    QString comments = m_descriptionTextEdit->toPlainText();
    QString minidump_filename = m_minidumpFilename;

    if (m_hostInformationCheckBox->isChecked())
    {
        comments += QString("\n// %1:\n").arg(tr("Host information"));
        comments += m_hostInformationTextEdit->toPlainText();
    }

    m_dumpQFile = new QFile(m_minidumpPath);
    if (!m_dumpQFile->open(QFile::OpenModeFlag::ReadOnly))
    {
        ERROR_LOG(QString("Error opening the dump file %1 because [%2]").arg(m_minidumpPath).arg(m_dumpQFile->errorString()));
        delete m_dumpQFile;
        m_dumpQFile = nullptr;
        m_acceptClose = true;
        close();
        return;
    }


    m_multipart = new QHttpMultiPart(QHttpMultiPart::ContentType::FormDataType);
    m_multipart->append(createHttpPart("prod",product));
    m_multipart->append(createHttpPart("ver",version));
    m_multipart->append(createHttpPart("guid",guid));
    m_multipart->append(createHttpPart("ptime",ptime));
    m_multipart->append(createHttpPart("ctime",ctime));
    m_multipart->append(createHttpPart("email",email));
    m_multipart->append(createHttpPart("comments",comments));

    QHttpPart dumpPart;
    dumpPart.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/octet-stream");
    dumpPart.setHeader(QNetworkRequest::KnownHeaders::ContentDispositionHeader,
                       QVariant("form-data; name=\"upload_file_minidump\"; filename=\"" + minidump_filename + "\"")
                       );
    dumpPart.setBodyDevice(m_dumpQFile);
    m_multipart->append(dumpPart);

    QUrl url("http://localhost:8080/receive");
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::Attribute::CacheLoadControlAttribute, QNetworkRequest::CacheLoadControl::AlwaysNetwork);
    request.setAttribute(QNetworkRequest::Attribute::CacheSaveControlAttribute, false);
    request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader, QString("%1 (%2)").arg(ApplicationNameString, StarviewerBuildPlatform));


    this->connect(m_manager,SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),this,SLOT(onManagerSslErrors(QNetworkReply*,QList<QSslError>)));
    m_reply = m_manager->post(request, m_multipart);


    m_sendReportLabel->setVisible(true);
    m_sendProgressBar->setVisible(true);
    m_abortSendPushButton->setVisible(true);
    m_sendProgressBar->setValue(0);

    this->connect(m_reply,SIGNAL(error(QNetworkReply::NetworkError)),SLOT(onReplyError(QNetworkReply::NetworkError)));
    this->connect(m_reply,SIGNAL(finished()),SLOT(onReplyFinished()));
    this->connect(m_reply,SIGNAL(uploadProgress(qint64,qint64)),SLOT(onReplyUploadProgress(qint64,qint64)));
}

bool QCrashReporter::restart()
{
    QString starviewerPath = QCoreApplication::applicationDirPath() + "/" + STARVIEWER_EXE;
#ifdef WIN32
    // En windows per poder executar l'starviewer hem de tenir en compte que si està en algun directori que conte espais
    // com el directori C:\Program Files\Starviewer\starviewer.exe, hem de posar el path entre cometes
    // per a que no ho interpreti com a paràmetres, per exemple "C:\Program Files\Starviewer\starviewer.exe"

    // Afegim les cometes per si algun dels directori conté espai
     starviewerPath = "\"" + starviewerPath + "\"";
#endif

    QProcess process;
    process.startDetached(starviewerPath);

    return true;
}

void QCrashReporter::onReplyError(QNetworkReply::NetworkError code)
{
    ERROR_LOG(QString("Error sending the crash report %1 because of [%2]").arg(m_minidumpPath).arg(m_reply->errorString()));
    m_reply->close();
}

void QCrashReporter::onReplyFinished()
{
    if (m_reply->error() == QNetworkReply::NetworkError::NoError)
    {
        INFO_LOG(QString("Crash report sent: %1").arg(m_minidumpPath));

        QMessageBox successMsgbox(QMessageBox::Icon::Information,tr("Success"), tr("The crash report has been sent."));
        m_enableMsgboxes && successMsgbox.exec();
        m_acceptClose = true;
        m_successful = true;
        close();
    }
    else {
        QMessageBox confirmMsgbox(QMessageBox::Icon::Warning,
                                  tr("Send error"),
                                  tr("The crash report could not be sent to Starviewer developers because [%1] error happened, do you want to try it again?").arg(m_reply->errorString())
                                  );
        confirmMsgbox.addButton(QMessageBox::StandardButton::Yes);
        confirmMsgbox.addButton(QMessageBox::StandardButton::No);

        if (m_enableMsgboxes && confirmMsgbox.exec() == QMessageBox::Yes)
        {
            emit resendReport();
        }
        else {
            m_acceptClose = true;
            close();
        }
    }
}

void QCrashReporter::onReplyUploadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal > 0)
    {
        m_sendProgressBar->setValue((bytesTotal / bytesReceived)*100);

    }
}

void QCrashReporter::onManagerSslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    reply->ignoreSslErrors();
}

};
