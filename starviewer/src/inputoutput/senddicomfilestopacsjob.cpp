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

#include "senddicomfilestopacsjob.h"

#include "logging.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "senddicomfilestopacs.h"
#include "usermessage.h"

namespace udg {

SendDICOMFilesToPACSJob::SendDICOMFilesToPACSJob(PacsDevice pacsDevice, QList<Image*> imagesToSend)
 : PACSJob(pacsDevice)
{
    Q_ASSERT(imagesToSend.count() > 0);
    Q_ASSERT(imagesToSend.at(0)->getParentSeries());
    Q_ASSERT(imagesToSend.at (0)->getParentSeries()->getParentStudy());
    Q_ASSERT(imagesToSend.at(0)->getParentSeries()->getParentStudy()->getParentPatient());

    m_sendDICOMFilesToPACS = new SendDICOMFilesToPACS(getPacsDevice());

    m_imagesToSend = imagesToSend;
    m_sendRequestStatus = PACSRequestStatus::SendOk;
}

SendDICOMFilesToPACSJob::~SendDICOMFilesToPACSJob()
{
    delete m_sendDICOMFilesToPACS;
}

PACSJob::PACSJobType SendDICOMFilesToPACSJob::getPACSJobType()
{
    return PACSJob::SendDICOMFilesToPACSJobType;
}

void SendDICOMFilesToPACSJob::run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread)
{
    Q_UNUSED(self)
    Q_UNUSED(thread)

    m_lastDICOMFileSeriesInstanceUID = "";
    m_numberOfSeriesSent = 0;

    if (m_imagesToSend.count() > 0)
    {
        INFO_LOG("S'enviaran fitxers de l' estudi " + m_imagesToSend.at(0)->getParentSeries()->getParentStudy()->getInstanceUID() +
            " al PACS " + getPacsDevice().getAETitle());

        // S'ha d'especificar com a DirectConnection, perquè sinó aquest signal l'aten qui ha creat el Job, que és la interfície, per tant
        // no s'atendria fins que la interfície estigui lliure poguent provocar comportaments incorrectes
        connect(m_sendDICOMFilesToPACS, SIGNAL(DICOMFileSent(Image *, int)), SLOT(DICOMFileSent(Image *, int)), Qt::DirectConnection);

        m_sendRequestStatus = m_sendDICOMFilesToPACS->send(getFilesToSend());

        if (m_sendRequestStatus == PACSRequestStatus::SendOk || m_sendRequestStatus == PACSRequestStatus::SendSomeDICOMFilesFailed ||
            m_sendRequestStatus == PACSRequestStatus::SendWarningForSomeImages)
        {
            /// Si s'han envait imatges indiquem que s'ha enviat la última sèrie
            m_numberOfSeriesSent++;
            emit DICOMSeriesSent(m_selfPointer.toStrongRef(), m_numberOfSeriesSent);
        }
    }
}

void SendDICOMFilesToPACSJob::requestCancelJob()
{
    INFO_LOG(QString("S'ha demanat la cancel.lacio del Job d'enviament d'imatges de l'estudi %1 al PACS %2")
                .arg(getStudyOfDICOMFilesToSend()->getInstanceUID(), getPacsDevice().getAETitle()));
    m_sendDICOMFilesToPACS->requestCancel();
}

PACSRequestStatus::SendRequestStatus SendDICOMFilesToPACSJob::getStatus()
{
    return m_sendRequestStatus;
}

// TODO:Centralitzem la contrucció dels missatges d'error perquè a totes les interfícies en puguin utilitzar un, i no calgui tenir el tractament d'errors
// duplicat ni traduccions, però és el millor lloc aquí posar aquest codi?
QString SendDICOMFilesToPACSJob::getStatusDescription()
{
    QString message;
    QString errorDetails = "\n\n" + tr("Details:") + "\n" + m_sendDICOMFilesToPACS->getResponseStatus().toString();
    QString studyID = getStudyOfDICOMFilesToSend()->getID();
    QString patientName = getStudyOfDICOMFilesToSend()->getParentPatient()->getFullName();
    QString pacsAETitle = getPacsDevice().getAETitle();

    switch (getStatus())
    {
        case PACSRequestStatus::SendOk:
            message = tr("Images from study %1 of patient %2 have been successfully sent to PACS %3.").arg(studyID, patientName, pacsAETitle);
            break;
        case PACSRequestStatus::SendCanNotConnectToPACS:
            message = tr("Unable to send DICOM images from study %1 of patient %2 because cannot connect to PACS %3.")
                .arg(studyID, patientName, pacsAETitle);
            message += "\n\n";
            message += tr("Make sure that your computer is connected to the network and the PACS parameters are correct.");
            message += "\n";
            message += UserMessage::getProblemPersistsAdvice();
            break;
        case PACSRequestStatus::SendAllDICOMFilesFailed:
        case PACSRequestStatus::SendUnknowStatus:
            message = tr("Sending of images from study %1 of patient %2 to PACS %3 has failed.")
                .arg(studyID, patientName, pacsAETitle);
            message += "\n\n";
            message += tr("Wait a few minutes and try again, if the problem persists contact with an administrator.");
            message += errorDetails;
            break;
        case PACSRequestStatus::SendSomeDICOMFilesFailed:
            message = tr("%1 images from study %2 of patient %3 cannot be sent because PACS %4 has rejected them.").arg(
                QString().setNum(m_sendDICOMFilesToPACS->getNumberOfDICOMFilesSentFailed()), studyID, patientName, pacsAETitle);
            message += "\n\n";
            message += tr("Please contact with an administrator to solve the problem.");
            message += errorDetails;
            break;
        case PACSRequestStatus::SendWarningForSomeImages:
            message = tr("Images from study %1 of patient %2 have been sent, but it's possible that PACS %3 has changed some data of them.").arg(
                studyID, patientName, pacsAETitle);
            message += errorDetails;
            break;
        case PACSRequestStatus::SendCancelled:
            message = tr("Sending of images from study %1 of patient %2 to PACS %3 has been cancelled.").arg(
                studyID, patientName, pacsAETitle);
            break;
        case PACSRequestStatus::SendPACSConnectionBroken:
            message = tr("The connection with PACS %1 has been broken while sending images from study %2 of patient %3.").arg(pacsAETitle, studyID, patientName);
            message += "\n\n";
            message += tr("Wait a few minutes and try again, if the problem persist contact with an administrator.");
            break;
        default:
            message = tr("An unknown error has occurred while sending images from study %1 of patient %2 to PACS %3.").arg(
                studyID, patientName, pacsAETitle);
            message += "\n\n";
            message += UserMessage::getCloseWindowsAndTryAgainAdvice();
            message += "\n";
            message += UserMessage::getProblemPersistsAdvice();
            message += errorDetails;
            break;
    }

    return message;
}

QList<Image*> SendDICOMFilesToPACSJob::getFilesToSend()
{
    return m_imagesToSend;
}

Study* SendDICOMFilesToPACSJob::getStudyOfDICOMFilesToSend()
{
    return m_imagesToSend.at(0)->getParentSeries()->getParentStudy();
}

void SendDICOMFilesToPACSJob::DICOMFileSent(Image *imageSent, int numberOfDICOMFilesSent)
{
    // Pressuposem que les imatges venen agrupades per sèries, sino és així s'ha de modificar aquest codi, perquè sinó es comptabilitzaran més series enviades
    // de les que realment s'han enviat
    emit DICOMFileSent(m_selfPointer.toStrongRef(), numberOfDICOMFilesSent);

    if (imageSent->getParentSeries()->getInstanceUID() != m_lastDICOMFileSeriesInstanceUID && !m_lastDICOMFileSeriesInstanceUID.isEmpty())
    {
        m_numberOfSeriesSent++;
        emit DICOMSeriesSent(m_selfPointer.toStrongRef(), m_numberOfSeriesSent);
    }

    m_lastDICOMFileSeriesInstanceUID = imageSent->getParentSeries()->getInstanceUID();
}

};
