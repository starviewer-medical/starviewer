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

#include "q2dviewerannotationssettingshelper.h"

#include "coresettings.h"

namespace udg {

namespace {

// Several keys used in this class.
const QString Default("Default");
const QString Modality("Modality");
const QString TopLeft("TopLeft");
const QString TopRight("TopRight");
const QString BottomLeft("BottomLeft");
const QString BottomRight("BottomRight");
const QString TopOrientation("TopOrientation");
const QString BottomOrientation("BottomOrientation");
const QString LeftOrientation("LeftOrientation");
const QString RightOrientation("RightOrientation");
const QString InvertSidesMG("InvertSidesMG");

// Default settings for the "Default" modality.
const Q2DViewerAnnotationsSettings DefaultDefaultSettings{
    // top left
    "{%imageWidth%} x {%imageHeight%}\n"
    "{%lutName%} WW: {%windowWidth%} WL: {%windowCenter%}\n" +
    QString("{%threshold:%1: $&%%}").arg(QObject::tr("Threshold")),
    // top right
    "{%InstitutionName%}\n"
    "{%PatientName%}\n"
    "{%patientAge%} {%PatientSex%} {%PatientID%}\n" +
                                                //: accession number
    QString("{%AccessionNumber:%1: $&%}\n").arg(QObject::tr("Acc")) +
    "{%studyDate%}\n"
    "{%seriesTime%}\n"
    "{%imageTime%}",
    // bottom left
                                         //: location
    QString("{%location:%1: $&%}\n").arg(QObject::tr("Loc")) +
    QString("{%slice:%1: $&%} {%phase:%2: $&%} {%thickness:%3: $& mm%}").arg(QObject::tr("Slice"), QObject::tr("Phase"), QObject::tr("Thickness")),
    // bottom right
                                           //: laterality
    QString("{%laterality:%1: $&%}\n").arg(QObject::tr("Lat")) +
    QString("{%fusionBalance:%1: $&%}\n").arg(QObject::tr("Fusion")) +
    "{%seriesLabel%}\n" +
    QString("{%ReferringPhysicianName:%1: $&%}").arg(QObject::tr("Physician")),
    // top/bottom/left/right orientation
    true, true, true, true,
    // invert sides MG
    false
};

// Default settings for "MG" modality.
const Q2DViewerAnnotationsSettings DefaultMGSettings{
    // top left
    "",
    // top right
    "{%InstitutionName%}\n"
    "{%PatientName%}\n"
    "{%patientAge%} {%PatientSex%} {%PatientID%}\n" +
                                                //: accession number
    QString("{%AccessionNumber:%1: $&%}\n").arg(QObject::tr("Acc")) +
    "{%studyDate%}\n"
    "{%seriesTime%}\n"
    "{%imageTime%}",
    // bottom left
    "",
    // bottom right
    "{%ImageLaterality%} {%mgProjection%}",
    // top/bottom/left/right orientation
    true, true, false, true,
    // invert sides MG
    true
};

// Returns a struct instance with values corresponding to the given map.
Q2DViewerAnnotationsSettings mapToStruct(const Settings::SettingsListItemType &map)
{
    Q2DViewerAnnotationsSettings annotationSettingsStruct;
    annotationSettingsStruct.topLeft = map[TopLeft].toString();
    annotationSettingsStruct.topRight = map[TopRight].toString();
    annotationSettingsStruct.bottomLeft = map[BottomLeft].toString();
    annotationSettingsStruct.bottomRight = map[BottomRight].toString();
    annotationSettingsStruct.topOrientation = map[TopOrientation].toBool();
    annotationSettingsStruct.bottomOrientation = map[BottomOrientation].toBool();
    annotationSettingsStruct.leftOrientation = map[LeftOrientation].toBool();
    annotationSettingsStruct.rightOrientation = map[RightOrientation].toBool();
    annotationSettingsStruct.invertSidesMG = map[InvertSidesMG].toBool();
    return annotationSettingsStruct;
}

// Returns a map (to save in Settings) with values corresponding to the given modality and struct.
Settings::SettingsListItemType structToMap(const QString &modality, const Q2DViewerAnnotationsSettings &annotationsSettingsStruct)
{
    return
    {
        {Modality, modality},
        {TopLeft, annotationsSettingsStruct.topLeft},
        {TopRight, annotationsSettingsStruct.topRight},
        {BottomLeft, annotationsSettingsStruct.bottomLeft},
        {BottomRight, annotationsSettingsStruct.bottomRight},
        {TopOrientation, annotationsSettingsStruct.topOrientation},
        {BottomOrientation, annotationsSettingsStruct.bottomOrientation},
        {LeftOrientation, annotationsSettingsStruct.leftOrientation},
        {RightOrientation, annotationsSettingsStruct.rightOrientation},
        {InvertSidesMG, annotationsSettingsStruct.invertSidesMG}
    };
}

}

bool Q2DViewerAnnotationsSettings::operator!=(const Q2DViewerAnnotationsSettings &other) const
{
    return this->topLeft != other.topLeft || this->topRight != other.topRight || this->bottomLeft != other.bottomLeft || this->bottomRight != other.bottomRight
            || this->topOrientation != other.topOrientation || this->bottomOrientation != other.bottomOrientation
            || this->leftOrientation != other.leftOrientation || this->rightOrientation != other.rightOrientation || this->invertSidesMG != other.invertSidesMG;
}

Settings::SettingListType Q2DViewerAnnotationsSettingsHelper::getSettingsDefaultValue()
{
    return {structToMap(Default, DefaultDefaultSettings), structToMap("MG", DefaultMGSettings)};
}

Q2DViewerAnnotationsSettingsHelper::Q2DViewerAnnotationsSettingsHelper()
    : m_modified(false)
{
    Settings settings;
    const Settings::SettingListType &annotationsSettings = settings.getList(CoreSettings::Q2DViewerAnnotations);

    for (const Settings::SettingsListItemType &item : annotationsSettings)
    {
        QString modality = item[Modality].toString();
        Q2DViewerAnnotationsSettings modalitySettings;
        modalitySettings.topLeft = item[TopLeft].toString();
        modalitySettings.topRight = item[TopRight].toString();
        modalitySettings.bottomLeft = item[BottomLeft].toString();
        modalitySettings.bottomRight = item[BottomRight].toString();
        modalitySettings.topOrientation = item[TopOrientation].toBool();
        modalitySettings.bottomOrientation = item[BottomOrientation].toBool();
        modalitySettings.leftOrientation = item[LeftOrientation].toBool();
        modalitySettings.rightOrientation = item[RightOrientation].toBool();
        modalitySettings.invertSidesMG = item[InvertSidesMG].toBool();
        m_settingsPerModality[modality] = modalitySettings;
    }

    // Create missing entries if needed
    if (!m_settingsPerModality.contains(Default))
    {
        settings.addListItem(CoreSettings::Q2DViewerAnnotations, structToMap(Default, DefaultDefaultSettings));
        m_settingsPerModality[Default] = DefaultDefaultSettings;
    }
    if (!m_settingsPerModality.contains("MG"))
    {
        settings.addListItem(CoreSettings::Q2DViewerAnnotations, structToMap("MG", DefaultMGSettings));
        m_settingsPerModality["MG"] = DefaultMGSettings;
    }
}

Q2DViewerAnnotationsSettingsHelper::~Q2DViewerAnnotationsSettingsHelper()
{
    if (m_modified)
    {
        Settings::SettingListType list;
        QStringList keys = m_settingsPerModality.keys();
        keys.move(keys.indexOf(Default), 0);    // move Default to the beginning

        for (const QString &key : qAsConst(keys))
        {
            list.append(structToMap(key, m_settingsPerModality[key]));
        }

        Settings settings;
        settings.setList(CoreSettings::Q2DViewerAnnotations, list);
    }
}

Q2DViewerAnnotationsSettings Q2DViewerAnnotationsSettingsHelper::getDefaultSettings(const QString &modality) const
{
    if (modality == "MG")
    {
        return DefaultMGSettings;
    }
    else
    {
        return DefaultDefaultSettings;
    }
}

Q2DViewerAnnotationsSettings Q2DViewerAnnotationsSettingsHelper::getDefaultSettings() const
{
    return m_settingsPerModality.value(Default);
}

void Q2DViewerAnnotationsSettingsHelper::setDefaultSettings(const Q2DViewerAnnotationsSettings &annotationsSettings)
{
    if (m_settingsPerModality[Default] != annotationsSettings)
    {
        m_settingsPerModality[Default] = annotationsSettings;
        m_modified = true;
    }
}

Q2DViewerAnnotationsSettings Q2DViewerAnnotationsSettingsHelper::getSettings(const QString &modality) const
{
    return m_settingsPerModality.value(modality, getDefaultSettings());
}

void Q2DViewerAnnotationsSettingsHelper::setSettings(const QString &modality, const Q2DViewerAnnotationsSettings &annotationsSettings)
{
    if (m_settingsPerModality[modality] != annotationsSettings)
    {
        m_settingsPerModality[modality] = annotationsSettings;
        m_modified = true;
    }
}

void Q2DViewerAnnotationsSettingsHelper::deleteSettings(const QString &modality)
{
    if (m_settingsPerModality.contains(modality))
    {
        m_settingsPerModality.remove(modality);
        m_modified = true;
    }
}

QStringList Q2DViewerAnnotationsSettingsHelper::getModalitiesWithSpecificSettings() const
{
    QStringList keys = m_settingsPerModality.keys();
    keys.removeOne(Default);
    return keys;
}

} // namespace udg
