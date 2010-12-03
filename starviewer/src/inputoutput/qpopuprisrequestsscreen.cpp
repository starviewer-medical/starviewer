/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qpopuprisrequestsscreen.h"

#include <QTimer>
#include <QDesktopWidget>
#include <QRect>
#include <QApplication>
#include <QMovie>

#include "qpopuprisrequestsscreen.h"
#include "logging.h"
#include "starviewerapplication.h"
#include "retrievedicomfilesfrompacsjob.h"
#include "study.h"
#include "pacsrequeststatus.h"
#include "patient.h"

namespace udg {

const int QPopUpRisRequestsScreen::msTimeOutToHidePopUp = 5000;

QPopUpRisRequestsScreen::QPopUpRisRequestsScreen( QWidget *parent ): QDialog( parent )
{
    setupUi(this);
    this->setWindowFlags(Qt::SubWindow | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);

    m_qTimer = new QTimer();
    connect(m_qTimer,SIGNAL(timeout()),SLOT(timeoutTimer()));

    QMovie *operationAnimation = new QMovie(this);
    operationAnimation->setFileName(":/images/loader.gif");
    m_operationAnimation->setMovie(operationAnimation);
    operationAnimation->start();
}

void QPopUpRisRequestsScreen::queryStudiesByAccessionNumberStarted()
{
    m_labelRisRequestDescription->setText(tr("%1 has received a request from RIS to retrieve studies.").arg(ApplicationNameString));
    m_operationDescription->setText(tr("Querying PACS..."));
    m_operationAnimation->show();

    m_studiesRetrievingCounter->setText("");

    m_numberOfStudiesRetrieved = 0;
    m_numberOfStudiesToRetrieve = 0;
}

void QPopUpRisRequestsScreen::addStudyToRetrieveByAccessionNumber(RetrieveDICOMFilesFromPACSJob *retrieveDICOMFilesFromPACSJob)
{
    if (m_numberOfStudiesToRetrieve == 0)
    {
        //Si és el primer estudi indiquem que comencem a descarregar i indiquem el nom del pacient
        m_operationDescription->setText(tr("Retrieving study"));
        showPatientNameOfRetrievingStudies(retrieveDICOMFilesFromPACSJob->getStudyToRetrieveDICOMFiles()->getParentPatient());
    }

    m_numberOfStudiesToRetrieve++;
    refreshLabelStudyCounter();
    connect(retrieveDICOMFilesFromPACSJob, SIGNAL(PACSJobFinished(PACSJob*)), SLOT(retrieveDICOMFilesFromPACSJobFinished(PACSJob *)));
    connect(retrieveDICOMFilesFromPACSJob, SIGNAL(PACSJobCancelled(PACSJob*)), SLOT(retrieveDICOMFilesFromPACSJobCancelledOrFailed(PACSJob *)));

}

void QPopUpRisRequestsScreen::retrieveDICOMFilesFromPACSJobFinished(PACSJob *pacsJob)
{
    RetrieveDICOMFilesFromPACSJob *retrieveDICOMFilesFromPACSJob = qobject_cast<RetrieveDICOMFilesFromPACSJob*>(pacsJob);

    if (retrieveDICOMFilesFromPACSJob == NULL)
    {
        ERROR_LOG("El PACSJob que ha finalitzat no és un RetrieveDICOMFilesFromPACSJob");
    }
    else
    {
        if (retrieveDICOMFilesFromPACSJob->getStatus() == PACSRequestStatus::RetrieveOk || 
            retrieveDICOMFilesFromPACSJob->getStatus() == PACSRequestStatus::RetrieveSomeDICOMFilesFailed)
        {
            m_numberOfStudiesRetrieved++;
            
            if (m_numberOfStudiesRetrieved < m_numberOfStudiesToRetrieve)
            {
                refreshLabelStudyCounter();
            }
            else
            {
                showRetrieveFinished();
            }
        }
        else
        {
            retrieveDICOMFilesFromPACSJobCancelledOrFailed(pacsJob);
        }
    }
}

void QPopUpRisRequestsScreen::retrieveDICOMFilesFromPACSJobCancelledOrFailed(PACSJob *pacsJob)
{
    RetrieveDICOMFilesFromPACSJob *retrieveDICOMFilesFromPACSJob = qobject_cast<RetrieveDICOMFilesFromPACSJob*>(pacsJob);

    if (retrieveDICOMFilesFromPACSJob == NULL)
    {
        ERROR_LOG("El PACSJob que ha finalitzat no és un RetrieveDICOMFilesFromPACSJob");
    }
    else
    {
        //Si ha fallat l'estudi el treiem de la llista d'estudis a descarregar
        m_numberOfStudiesToRetrieve--;
        
        if (m_numberOfStudiesRetrieved < m_numberOfStudiesToRetrieve)
        {
            refreshLabelStudyCounter();
        }
        else
        {
            showRetrieveFinished();
        }
    }
}

void QPopUpRisRequestsScreen::showNotStudiesFoundMessage()
{
    m_operationDescription->setText(tr("No studies found."));
    m_operationAnimation->hide();
    m_qTimer->start(msTimeOutToHidePopUp);
}

void QPopUpRisRequestsScreen::refreshLabelStudyCounter()
{
    m_studiesRetrievingCounter->setText(QString(tr("%1 of %2.")).arg(m_numberOfStudiesRetrieved + 1).arg(m_numberOfStudiesToRetrieve));
}

void QPopUpRisRequestsScreen::showRetrieveFinished()
{
    m_operationAnimation->hide();

    if (m_numberOfStudiesRetrieved == 0)
    {
        m_operationDescription->setText(tr("No studies has been retrieved.").arg(m_numberOfStudiesRetrieved));
    }
    else
    if (m_numberOfStudiesRetrieved  == 1)
    {
        m_operationDescription->setText(tr("%1 study has been retrieved.").arg(m_numberOfStudiesRetrieved));
    }
    else
    {
        m_operationDescription->setText(tr("%1 studies have been retrieved.").arg(m_numberOfStudiesRetrieved));
    }
    m_studiesRetrievingCounter->setText("");
    m_qTimer->start(msTimeOutToHidePopUp);
}

void QPopUpRisRequestsScreen::showPatientNameOfRetrievingStudies(Patient *patient)
{
    QString popUpText = tr("%1 has received a request from RIS to retrieve studies of patient %2.").arg(ApplicationNameString).arg(patient->getFullName());
    m_labelRisRequestDescription->setText(popUpText);
}

void QPopUpRisRequestsScreen::showEvent(QShowEvent *)
{
    //Es situa el PopUp a baix a l'esquerre de la pantalla on està la interfície activa del Starviewer
    QDesktopWidget desktopWidget;

    QRect screenGeometryActiveWindow = desktopWidget.availableGeometry(QApplication::activeWindow()); //Agafem les dimensions de la pantalla on està la finestra activa de l'starviewer

    this->move(screenGeometryActiveWindow.x() + screenGeometryActiveWindow.width() - this->width() - 10, screenGeometryActiveWindow.y() + screenGeometryActiveWindow.height() - this->height() -10);
}

void QPopUpRisRequestsScreen::timeoutTimer()
{
    this->hide();
    m_qTimer->stop();
}

QPopUpRisRequestsScreen::~QPopUpRisRequestsScreen()
{
    m_qTimer->stop();
    delete m_qTimer;
}

};
