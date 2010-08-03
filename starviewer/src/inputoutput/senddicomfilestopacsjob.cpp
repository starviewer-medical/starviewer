#include "senddicomfilestopacsjob.h"

#include "status.h"

#include "logging.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "starviewerapplication.h"
#include "senddicomfilestopacs.h"

namespace udg
{

SendDICOMFilesToPACSJob::SendDICOMFilesToPACSJob(PacsDevice pacsDevice, QList<Image*> imagesToSend):PACSJob(pacsDevice)
{
    Q_ASSERT(imagesToSend.count() > 0);
    Q_ASSERT(imagesToSend.at(0)->getParentSeries());
    Q_ASSERT(imagesToSend.at (0)->getParentSeries()->getParentStudy());
    Q_ASSERT(imagesToSend.at(0)->getParentSeries()->getParentStudy()->getParentPatient());

    m_sendDICOMFilesToPACS = new SendDICOMFilesToPACS(getPacsDevice());

    m_imagesToSend = imagesToSend;
    m_sendRequestStatus = PACSRequestStatus::OkSend;
}

SendDICOMFilesToPACSJob::~SendDICOMFilesToPACSJob()
{
    delete m_sendDICOMFilesToPACS;
}

PACSJob::PACSJobType SendDICOMFilesToPACSJob::getPACSJobType()
{
    return PACSJob::SendDICOMFilesToPACSJobType;
}

void SendDICOMFilesToPACSJob::run()
{
    m_lastDICOMFileSeriesInstanceUID = "";
    m_numberOfSeriesSent = 0;

    if (m_imagesToSend.count() > 0)
    {
        INFO_LOG( "S'enviaran fitxers de l' estudi " + m_imagesToSend.at(0)->getParentSeries()->getParentStudy()->getInstanceUID() +
            " al PACS " + getPacsDevice().getAETitle() );

        /*S'ha d'especificar com a DirectConnection, perquè sinó aquest signal l'aten qui ha creat el Job, que és la interfície, per tant
         no s'atendria fins que la interfície estigui lliure poguent provocar comportaments incorrectes*/
        connect(m_sendDICOMFilesToPACS, SIGNAL( DICOMFileSent(Image *, int) ), SLOT( DICOMFileSent(Image *, int) ), Qt::DirectConnection );

        m_sendRequestStatus = m_sendDICOMFilesToPACS->send(getFilesToSend());

        if (m_sendRequestStatus == PACSRequestStatus::OkSend || m_sendRequestStatus == PACSRequestStatus::SomeDICOMFilesSentFailed ||
            m_sendRequestStatus == PACSRequestStatus::WarningSend)
        {
            ///Si s'han envait imatges indiquem que s'ha enviat la última sèrie
            m_numberOfSeriesSent++;
            emit DICOMSeriesSent(this, m_numberOfSeriesSent);
        }
    }
}

void SendDICOMFilesToPACSJob::requestCancelJob()
{
    INFO_LOG(QString("S'ha demanat la cancel·lació del Job d'enviament d'imatges de l'estudi %1 al PACS %2").arg(getStudyOfDICOMFilesToSend()->getInstanceUID(),
        getPacsDevice().getAETitle()));
    m_sendDICOMFilesToPACS->requestCancel();
}

PACSRequestStatus::SendRequestStatus SendDICOMFilesToPACSJob::getStatus()
{
    return m_sendRequestStatus;
}

/*TODO:Centralitzem la contrucció dels missatges d'error perquè a totes les interfícies en puguin utilitzar un, i no calgui tenir el tractament d'errors duplicat
       ni traduccions, però és el millor lloc aquí posar aquest codi? */
QString SendDICOMFilesToPACSJob::getStatusDescription()
{
    QString message;
    QString studyID = getStudyOfDICOMFilesToSend()->getID();
    QString patientName = getStudyOfDICOMFilesToSend()->getParentPatient()->getFullName();
    QString pacsAETitle = getPacsDevice().getAETitle();

    switch (getStatus())
    {
        case PACSRequestStatus::OkSend:
            message = tr("Study %1 of patient %2 has been sent succesfully to PACS %3").arg(studyID, patientName, pacsAETitle);
            break;
        case PACSRequestStatus::CanNotConnectPACSToSend:
            message = tr("%1 can't send DICOM files of study %2 of patient %3 because can't connect to PACS %4.\n").arg(ApplicationNameString, 
                studyID, patientName, pacsAETitle);
            message += tr("\nBe sure that your computer is connected on network and the PACS parameters are correct.");
            message += tr("\nIf the problem persists contact with an administrator.");
            break;
        case PACSRequestStatus::FailureSend:
        case PACSRequestStatus::UnknowStatusSend:
            message = tr("The send of DICOM files from study %1 of patient %2 to PACS %3 has failed.\n\n").arg(studyID, patientName, pacsAETitle); 
            message += tr("Wait a few minutes and try again, if the problem persist contact with an administrator.");
            break;
        case PACSRequestStatus::SomeDICOMFilesSentFailed:
            message = tr("%1 DICOM files from study %2 of patient %3 can't be send because PACS %4 has rejected them.\n\n").arg(
                QString().setNum(m_sendDICOMFilesToPACS->getNumberOfDICOMFilesSentFailed()), studyID, patientName, pacsAETitle);
            message += tr("Please contact with an administrator to solve the problem.");
            break;
        case PACSRequestStatus::WarningSend:
            message = tr("The study %1 of patient %2 has been sent, but it's possible that the PACS %3 has changed some data of the images.").arg(
                studyID, patientName, pacsAETitle);
            break;
        case PACSRequestStatus::CancelledSend:
            message = tr("The sent of study %1 from patient %2 to PACS %3 has been cnacelled.").arg(
                studyID, patientName, pacsAETitle);
            break;
        case PACSRequestStatus::PACSConnectionBroken:
            message = tr("The connection with PACS %1 has been broken sending the DICOM Files from study %2 of patient %3. ").arg(pacsAETitle, studyID, patientName);
            message += tr("Wait a few minutes and try again, if the problem persist contact with an administrator.");
            break;
        default:
            message = tr("An unknown error has ocurred sending DICOM files from study %1 of patient %2 to PACS %3 .").arg(
                studyID, patientName, pacsAETitle);
            message += tr("\n\nClose all %1 windows and try again."
                         "\nIf the problem persists contact with an administrator.").arg(ApplicationNameString);
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
    /*Pressuposem que les imatges venen agrupades per sèries, sino és així s'ha de modificar aquest codi, perquè sinó es comptabilitzaran més series enviades
      de les que realment s'han enviat*/
    emit DICOMFileSent(this, numberOfDICOMFilesSent);

    if (imageSent->getParentSeries()->getInstanceUID() != m_lastDICOMFileSeriesInstanceUID && !m_lastDICOMFileSeriesInstanceUID.isEmpty())
    {
        m_numberOfSeriesSent++;
        emit DICOMSeriesSent(this, m_numberOfSeriesSent);
    }

    m_lastDICOMFileSeriesInstanceUID = imageSent->getParentSeries()->getInstanceUID();
}

};