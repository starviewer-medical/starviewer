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

#include "externalstudyrequestmediator.h"

#include "externalstudyrequestmanager.h"
#include "qapplicationmainwindow.h"

namespace udg {

void ExternalStudyRequestMediator::requestStudyByUid(const QString &studyInstanceUid)
{
    DicomMask mask;
    mask.setStudyInstanceUID(studyInstanceUid);
    ExternalStudyRequestManager::instance()->processRequest(mask);
}

void ExternalStudyRequestMediator::requestStudyByAccessionNumber(const QString &accessionNumber)
{
    DicomMask mask;
    mask.setStudyInstanceUID("");   // this is required for a DIMSE query to return the Study Instance UID
    mask.setAccessionNumber(accessionNumber);
    ExternalStudyRequestManager::instance()->processRequest(mask);
}

ExternalStudyRequestMediator::ExternalStudyRequestMediator(QObject *parent)
    : QObject(parent)
{
    connect(ExternalStudyRequestManager::instance(), &ExternalStudyRequestManager::viewStudyRetrievedFromRequest,
            this, &ExternalStudyRequestMediator::viewStudy);
    connect(ExternalStudyRequestManager::instance(), &ExternalStudyRequestManager::loadStudyRetrievedFromRequest, this,
            &ExternalStudyRequestMediator::loadStudy);
}

void ExternalStudyRequestMediator::viewStudy(const QString &studyInstanceUid)
{
    QApplicationMainWindow::getLastActiveApplicationMainWindow()->viewStudy(studyInstanceUid);
}

void ExternalStudyRequestMediator::loadStudy(const QString &studyInstanceUid)
{
    QApplicationMainWindow::getLastActiveApplicationMainWindow()->loadStudy(studyInstanceUid);
}

} // namespace udg
