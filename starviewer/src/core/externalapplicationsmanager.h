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

#include "singleton.h"

#include <QObject>
#include <QList>
#include <QHash>

namespace udg {

class Volume;
class ExternalApplication;

/**
 * @brief Singleton to manage external applications
 *
 * ExternalApplicationsManager singleton reads and saves the external
 * applications to the settings.
 *
 * It also emits an event when the list of applications is changed, thus the UI
 * can be updated.
 *
 * This singleton is the point where the parameters to be replaced are
 * collected. When the active study changes, setParameters should be
 * called.
 */
class ExternalApplicationsManager : public QObject, public Singleton<ExternalApplicationsManager>
{
    Q_OBJECT
public:
    /// @brief Reads the external applications from the settings.
    /// @return List of external applications
    QList<ExternalApplication> getApplications() const;
    
    /// @brief Writes the given external applications to the settings.
    void setApplications(const QList<ExternalApplication> &applications);

    /// @brief Empties the parameters list.
    /// 
    /// You should call this when no study is active. Parameters will be empty
    /// when an application is launched.
    void cleanParameters();
    
    /// @brief Reads the parameters to replace for the current active study.
    ///
    /// This function should be called when an study becomes active. It reads
    /// the information to fill the parameters from the given volume.
    ///
    /// @param volume Volume that is currently active
    void setParameters(Volume* volume);
    
    /// @return Parameter names and correspondant values.
    const QHash<QString, QString>& getParameters() const;

    /// @brief Launches the given application.
    ///
    /// The given application is launched with the parameters given by
    /// getParameters().
    void launch(const ExternalApplication &application) const;

protected:
    friend class Singleton<ExternalApplicationsManager>;
    explicit ExternalApplicationsManager(QObject *parent = 0);
    ~ExternalApplicationsManager();

signals:
    /// @brief Emmited when the external applications list is set and written to the settings
    void onApplicationsChanged();

private:
    QHash<QString,QString> m_parameters;

};

}

#endif // EXTERNALAPPLICATIONSMANAGER_H
