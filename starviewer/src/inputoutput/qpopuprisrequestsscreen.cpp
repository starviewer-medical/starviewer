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
#include "operation.h"
#include "logging.h"
#include "starviewerapplication.h"
#include "settings.h"

namespace udg {

const int QPopUpRisRequestsScreen::msTimeOutToHidePopUp = 5000;
const int QPopUpRisRequestsScreen::msTimeOutToMovePopUpToBottomRight = 5000;

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

void QPopUpRisRequestsScreen::queryStudiesByAccessionNumberStarted(QString accessionNumber)
{
    m_labelRisRequestDescription->setText(tr("%1 has received a request from RIS to retrieve studies.").arg(ApplicationNameString));
    m_operationDescription->setText(tr("Querying PACS..."));
    m_operationAnimation->show();

    m_studiesRetrievingCounter->setText("");

    m_studiesInstanceUIDToRetrieve.clear();
    m_studiesInstanceUIDRetrieved.clear();
}

void QPopUpRisRequestsScreen::addStudyToRetrieveByAccessionNumber(QString studyInstanceUID)
{
    if (m_studiesInstanceUIDToRetrieve.count() == 0)
    {
        m_operationDescription->setText(tr("Retrieving study"));
    }

    m_studiesInstanceUIDToRetrieve.append(studyInstanceUID);
    refreshLabelStudyCounter();
}

void QPopUpRisRequestsScreen::retrieveStudyByAccessionNumberFinished(QString studyInstanceUID)
{
    m_studiesInstanceUIDRetrieved.append(studyInstanceUID);
    
    if (m_studiesInstanceUIDRetrieved.count() < m_studiesInstanceUIDToRetrieve.count())
    {
        refreshLabelStudyCounter();
    }
    else
    {
        showRetrieveFinished();
    }
}

void QPopUpRisRequestsScreen::retrieveStudyByAccessionNumberFailed(QString studyInstanceUID)
{
    //Si ha fallat l'estudi el treiem de la llista d'estudis a descarregar
    m_studiesInstanceUIDToRetrieve.removeOne(studyInstanceUID);
    
    if (m_studiesInstanceUIDRetrieved.count() < m_studiesInstanceUIDToRetrieve.count())
    {
        refreshLabelStudyCounter();
    }
    else
    {
        showRetrieveFinished();
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
    m_studiesRetrievingCounter->setText(QString(tr("%1 of %2.")).arg(m_studiesInstanceUIDRetrieved.count() + 1).arg(m_studiesInstanceUIDToRetrieve.count()));
}

void QPopUpRisRequestsScreen::showRetrieveFinished()
{
    m_operationAnimation->hide();
    int numberOfRetrievedStudies = m_studiesInstanceUIDRetrieved.count();
    if (numberOfRetrievedStudies == 1)
    {
        m_operationDescription->setText(tr("%1 study has been retrieved.").arg(numberOfRetrievedStudies));
    }
    else
    {
        m_operationDescription->setText(tr("%1 studies have been retrieved.").arg(numberOfRetrievedStudies));
    }
    m_studiesRetrievingCounter->setText("");
    m_qTimer->start(msTimeOutToHidePopUp);
}

void QPopUpRisRequestsScreen::setPatientNameOfRetrievingStudies(QString patientName)
{
    QString popUpText = tr("%1 has received a request from RIS to retrieve studies of patient %2.").arg(ApplicationNameString).arg(patientName);
    m_labelRisRequestDescription->setText(popUpText);
}

void QPopUpRisRequestsScreen::showEvent(QShowEvent *)
{
    //Es situa el PopUp al centre de la pantalla on està la mainWindow del Starviewer
    Settings settings;
    QWidget fakeMainWindow;

    settings.restoreGeometry(QString("geometry"), &fakeMainWindow);

    this->move(QApplication::desktop()->screenGeometry(&fakeMainWindow).center() - this->rect().center());

    QTimer::singleShot(msTimeOutToMovePopUpToBottomRight, this, SLOT(moveToBottomRight()));
}

void QPopUpRisRequestsScreen::timeoutTimer()
{
    this->hide();
    m_qTimer->stop();
}

void QPopUpRisRequestsScreen::moveToBottomRight()
{
   this->move(QApplication::desktop()->availableGeometry(this).bottomRight() - this->rect().bottomRight());
}

QPopUpRisRequestsScreen::~QPopUpRisRequestsScreen()
{
    m_qTimer->stop();
    delete m_qTimer;
}

};
