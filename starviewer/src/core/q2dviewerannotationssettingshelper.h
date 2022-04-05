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

#ifndef UDG_Q2DVIEWERANNOTATIONSSETTINGSHELPER_H
#define UDG_Q2DVIEWERANNOTATIONSSETTINGSHELPER_H

#include "settings.h"

namespace udg {

/**
 * @brief The Q2DViewerAnnotationsSettings struct holds annotations settings for a 2D viewer.
 */
struct Q2DViewerAnnotationsSettings
{
    QString topLeft;
    QString topRight;
    QString bottomLeft;
    QString bottomRight;
    bool topOrientation;
    bool bottomOrientation;
    bool leftOrientation;
    bool rightOrientation;
    bool invertSidesMG;

    bool operator!=(const Q2DViewerAnnotationsSettings &other) const;   // TODO use = default with C++20
};

/**
 * @brief The Q2DViewerAnnotationsSettingsHelper class is a helper class to ease dealing with 2D viewer annotations settings.
 *
 * It allows reading and writing settings for any modality and the "Default" modality and getting default values. Changes are saved in the destructor.
 */
class Q2DViewerAnnotationsSettingsHelper
{
public:
    /// Get default value of settings to store in the SettingsRegistry. It contains settings for "Default" and "MG" modalities.
    static Settings::SettingListType getSettingsDefaultValue();

    /// Reads current settings for each modality and stores them in the map. If settings for "Default" or "MG" are missing, fills them from default values and
    /// saves them in Settings.
    Q2DViewerAnnotationsSettingsHelper();
    /// Saves changes to Settings.
    ~Q2DViewerAnnotationsSettingsHelper();

    /// Returns default settings for the given modality.
    Q2DViewerAnnotationsSettings getDefaultSettings(const QString &modality) const;

    /// Returns settings for the "Default" modality.
    Q2DViewerAnnotationsSettings getDefaultSettings() const;
    /// Sets settings for the "Default" modality.
    void setDefaultSettings(const Q2DViewerAnnotationsSettings &annotationsSettings);

    /// Returns settings for the given modality.
    Q2DViewerAnnotationsSettings getSettings(const QString &modality) const;
    /// Sets settings for the given modality.
    void setSettings(const QString &modality, const Q2DViewerAnnotationsSettings &annotationsSettings);
    /// Deletes settings for the given modality.
    void deleteSettings(const QString &modality);

    /// Returns the list of modalities, excluding "Default", that have specific settings.
    QStringList getModalitiesWithSpecificSettings() const;

private:
    /// Map containing the annoatations settings for each modality.
    QMap<QString, Q2DViewerAnnotationsSettings> m_settingsPerModality;
    /// Set to true when there is any modification. If true, the destructor will save changes to Settings.
    bool m_modified;
};

} // namespace udg

#endif // UDG_Q2DVIEWERANNOTATIONSSETTINGSHELPER_H
