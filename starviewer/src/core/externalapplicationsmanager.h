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

#ifndef EXTERNALAPPLICATIONSMANAGER_H
#define EXTERNALAPPLICATIONSMANAGER_H

#include <QObject>
#include <QList>
#include "singleton.h"
#include "externalapplication.h"

namespace udg {

/**
 * @brief Singleton to manage external applications
 *
 * ExternalApplicationsManager singleton reads and saves the external applications to the settings.
 * It also emits an event when the list of applications is changed, thus the UI can be updated.
 */
class ExternalApplicationsManager : public QObject, public Singleton<ExternalApplicationsManager>
{
    Q_OBJECT
public:
    /**
     * @brief Reads the external applications from the settings.
     * @return List of external applications
     */
    QList<ExternalApplication> getApplications() const;
    /**
     * @brief Writes the given external applications to the settings.
     */
    void setApplications(const QList<ExternalApplication>& applications);

protected:
    friend class Singleton<ExternalApplicationsManager>;
    explicit ExternalApplicationsManager(QObject *parent = 0);
    ~ExternalApplicationsManager();

signals:
    /**
     * @brief Emmited when the external applications list is set and written to the settings
     */
    void onApplicationsChanged();

public slots:
};

}

#endif // EXTERNALAPPLICATIONSMANAGER_H
