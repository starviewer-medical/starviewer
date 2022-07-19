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

#ifndef UDG_EXTERNALSTUDYREQUESTMEDIATOR_H
#define UDG_EXTERNALSTUDYREQUESTMEDIATOR_H

#include <QObject>
#include "singleton.h"

namespace udg {

/**
 * @brief The ExternalStudyRequestMediator class allows to request a study to ExternalStudyRequestManager and assign the received study to the last active main
 *        window.
 */
class ExternalStudyRequestMediator : public QObject, public Singleton<ExternalStudyRequestMediator>
{
    Q_OBJECT

public:
    /// Requests the study with the given Study Instance UID through the ExternalStudyRequestManager.
    void requestStudyByUid(const QString &studyInstanceUid);
    /// Requests the study with the given Accession Number through the ExternalStudyRequestManager.
    void requestStudyByAccessionNumber(const QString &accessionNumber);

private:
    explicit ExternalStudyRequestMediator(QObject *parent = nullptr);
    friend Singleton<ExternalStudyRequestMediator>;

private slots:
    /// Calls QApplicationMainWindow::viewStudy on the last active main window.
    void viewStudy(const QString &studyInstanceUid);
    /// Calls QApplicationMainWindow::loadStudy on the last active main window.
    void loadStudy(const QString &studyInstanceUid);
};

} // namespace udg

#endif // UDG_EXTERNALSTUDYREQUESTMEDIATOR_H
