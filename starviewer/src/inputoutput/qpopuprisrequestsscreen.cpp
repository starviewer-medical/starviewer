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

#include "qpopuprisrequestsscreen.h"

#include "logging.h"
#include "starviewerapplication.h"
#include "studyoperationresult.h"

namespace udg {

QPopUpRISRequestsScreen::QPopUpRISRequestsScreen(QWidget *parent)
 : QNotificationPopup(parent)
{
    setOngoingOperationText(tr("%1 will proceed to retrieve it.").arg(ApplicationNameString));
}

QPopUpRISRequestsScreen::~QPopUpRISRequestsScreen()
{
}

void QPopUpRISRequestsScreen::queryStudiesStarted()
{
    // Si arriba una altra petició mentre hi ha activat el timer per amagar el PopUp o s'està amagant, hem de fer que aquest no s'amagui per
    // mostrar la nova petició
    cancelTriggeredAnimations();

    setHeadline(tr("%1 has received a request from RIS to retrieve studies.").arg(ApplicationNameString));
    setOngoingOperationText(tr("Querying PACS..."));
    showOngoingOperationAnimation();

    m_resultsOfStudiesToRetrieve.clear();
    m_numberOfStudiesRetrieved = 0;
    m_numberOfStudiesToRetrieve = 0;
    m_numberOfStudiesFailedToRetrieve = 0;
}

void QPopUpRISRequestsScreen::addStudyToRetrieveFromPacs(StudyOperationResult *result)
{
    m_numberOfStudiesToRetrieve++;

    m_resultsOfStudiesToRetrieve.append(result);
    refreshScreenRetrieveStatus();

    connect(result, &StudyOperationResult::finishedSuccessfully, this, &QPopUpRISRequestsScreen::onStudyRetrieveFinished);
    connect(result, &StudyOperationResult::finishedWithPartialSuccess, this, &QPopUpRISRequestsScreen::onStudyRetrieveFinished);
    connect(result, &StudyOperationResult::finishedWithError, this, &QPopUpRISRequestsScreen::onStudyRetrieveFailed);
    connect(result, &StudyOperationResult::cancelled, this, &QPopUpRISRequestsScreen::onStudyRetrieveCancelled);
}

void QPopUpRISRequestsScreen::addStudyRetrievedFromDatabase(Study *study)
{
    m_numberOfStudiesToRetrieve++;
    m_numberOfStudiesRetrieved++;
    refreshScreenRetrieveStatus();
}

void QPopUpRISRequestsScreen::onStudyRetrieveFinished(StudyOperationResult *result)
{
    ensureEndPosition();

    // If the result is not in the list it means that it is from an old RIS petition that must be ignored
    if (m_resultsOfStudiesToRetrieve.contains(result))
    {
        m_numberOfStudiesRetrieved++;
        refreshScreenRetrieveStatus();
    }
}

void QPopUpRISRequestsScreen::onStudyRetrieveFailed(StudyOperationResult *result)
{
    ensureEndPosition();

    // If the result is not in the list it means that it is from an old RIS petition that must be ignored
    if (m_resultsOfStudiesToRetrieve.contains(result))
    {
        m_numberOfStudiesFailedToRetrieve++;
        refreshScreenRetrieveStatus();
    }
}

void QPopUpRISRequestsScreen::onStudyRetrieveCancelled(StudyOperationResult *result)
{
    // On cancellation, remove the result from the list
    // If the result is not in the list it means that it is from an old RIS petition that must be ignored
    if (m_resultsOfStudiesToRetrieve.removeOne(result))
    {
        m_numberOfStudiesToRetrieve--;
        refreshScreenRetrieveStatus();
    }
}

void QPopUpRISRequestsScreen::refreshScreenRetrieveStatus()
{
    if (m_numberOfStudiesRetrieved + m_numberOfStudiesFailedToRetrieve < m_numberOfStudiesToRetrieve)
    {
        setOngoingOperationText(tr("Retrieving study %1 of %2.").arg(m_numberOfStudiesRetrieved + m_numberOfStudiesFailedToRetrieve + 1)
                                .arg(m_numberOfStudiesToRetrieve));
    }
    else
    {
        showRetrieveFinished();
        hideWithDelay();
    }
}

void QPopUpRISRequestsScreen::ensureEndPosition()
{
    // When the study has finished downloading, if the pop-up is not yet in the corner we force it to move there without delay so it does not bother the user
    if (isMoveAnimationOnDelayPeriod())
    {
        cancelTriggeredAnimations();
        startMoveAnimation();
    }
}

void QPopUpRISRequestsScreen::setTimeOutToHidePopUpAfterStudiesHaveBeenRetrieved(int timeOutms)
{
    setHideDelay(timeOutms);
}

void QPopUpRISRequestsScreen::showNotStudiesFoundMessage()
{
    setOngoingOperationText(tr("No studies found."));
    hideOngoingOperationAnimation();
    hideWithDelay();
}

void QPopUpRISRequestsScreen::showRetrieveFinished()
{
    hideOngoingOperationAnimation();

    if (m_numberOfStudiesRetrieved == 0)
    {
        if (m_numberOfStudiesFailedToRetrieve == 0)
        {
            setOngoingOperationText(tr("No studies found.").arg(m_numberOfStudiesRetrieved));
        }
        else 
        {
            setOngoingOperationText(tr("Unable to retrieve requested studies.").arg(ApplicationNameString));
        }
    }
    else if (m_numberOfStudiesFailedToRetrieve == 0)
    {
        if (m_numberOfStudiesRetrieved == 1)
        {
            setOngoingOperationText(tr("%1 study retrieved.").arg(m_numberOfStudiesRetrieved));
        }
        else
        {
            setOngoingOperationText(tr("%1 studies retrieved.").arg(m_numberOfStudiesRetrieved));
        }
    }
    else
    {
        setOngoingOperationText(tr("%1 studies retrieved, %2 failed.").arg(QString::number(m_numberOfStudiesRetrieved),
                                                                           QString::number(m_numberOfStudiesFailedToRetrieve)));
    }
}

}
