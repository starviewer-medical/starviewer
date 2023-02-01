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

#ifndef UDGSTUDYLAYOUTCONFIGSETTINGSCONVERTER_H
#define UDGSTUDYLAYOUTCONFIGSETTINGSCONVERTER_H

#include "settings.h"
#include "studylayoutconfig.h"

namespace udg {

/** 
    Classe que ens converteix un objecte StudyLayoutConfig al format adequat per llegir-lo des de o guardar-lo a settings correctament.
  */
class StudyLayoutConfigSettingsConverter {
public:
    StudyLayoutConfigSettingsConverter();
    ~StudyLayoutConfigSettingsConverter();

    /// Mètode per obtenir un StudyLayoutConfig d'un item de settings
    StudyLayoutConfig fromSettingsListItem(const Settings::SettingsListItemType &item) const;

    /// Mètode per convertir un StudyLayoutConfig a un item de settings
    Settings::SettingsListItemType toSettingsListItem(const StudyLayoutConfig &config) const;

private:
    /// Constants per definir el nom de cada camp dels settings
    static const QString ModalityKey;
    static const QString MaxNumberOfViewersKey;
    static const QString ExclusionCriteriaKey;
    static const QString UnfoldByKey;
    static const QString UnfoldDirectionKey;
    /// Constant per definir el separador de llistats d'Strings als settings
    static const QString StringListSeparator;

    /// Constants per definir els valors d'string amb que es guarden els valors enumerats
    static const QString Survey;
    static const QString Localizer;
    static const QString CTAttenuationCorrection;
    static const QString LeftToRight;
    static const QString TopToBottom;
    static const QString Images;
    static const QString Series;

private:
    /// Mètodes que ens transformen el valor enumerat d'StudyLayoutConfig a un l'string corresponent de settings
    QString getAsSettingsStringValue(const StudyLayoutConfig::ExclusionCriteriaType &criteria) const;
    QString getAsSettingsStringValue(const StudyLayoutConfig::UnfoldDirectionType &direction) const;
    QString getAsSettingsStringValue(const StudyLayoutConfig::UnfoldType &unfoldBy) const;
    
    /// Mètodes que ens transformen el valor de com s'han guardat als settings a l'enum corresponent d'StudyLayoutConfig
    QList<StudyLayoutConfig::ExclusionCriteriaType> getExclusionCriteriaFromSettingsValue(const QString &value) const;
    StudyLayoutConfig::UnfoldDirectionType getUnfoldDirectionFromSettingsValue(const QString &value) const;
    StudyLayoutConfig::UnfoldType getUnfoldTypeFromSettingsValue(const QString &value) const;
};

}

#endif
