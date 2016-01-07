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

#include "voilutpresetstooldata.h"

#include "customwindowlevelsrepository.h"

#include <QStringList>

namespace udg {

VoiLutPresetsToolData::VoiLutPresetsToolData(QObject *parent)
 : ToolData(parent)
{
    // Per defecte afegirem els window levels predeterminats estàndar.
    // TODO Més endavant aquests valors podrien estar continguts en algun determinat fitxer amb un format definit
    addPreset(WindowLevel(80, 40, tr("CT Head")), StandardPresets);
    addPreset(WindowLevel(2000, 500, tr("CT Bone")), StandardPresets);
    addPreset(WindowLevel(1500, -650, tr("CT Lung")), StandardPresets);
    addPreset(WindowLevel(400, 40, tr("CT Soft Tissues")), StandardPresets);
    addPreset(WindowLevel(400, 70, tr("CT Soft Tissues+Contrast")), StandardPresets);

    // 60-100
    addPreset(WindowLevel(300, 60, tr("CT Liver+Contrast")), StandardPresets);
    addPreset(WindowLevel(200, 40, tr("CT Liver")), StandardPresets);
    addPreset(WindowLevel(300, 50, tr("CT Neck+Contrast")), StandardPresets);

    // 100-200
    addPreset(WindowLevel(500, 100, tr("Angiography")), StandardPresets);

    // 100-1500:window!
    addPreset(WindowLevel(1000, 300, tr("Osteoporosis")), StandardPresets);
    addPreset(WindowLevel(800, -800, tr("Emphysema")), StandardPresets);
    addPreset(WindowLevel(4000, 700, tr("Petrous Bone")), StandardPresets);
    addPreset(WindowLevel(0, 0, getCustomPresetName()), CustomPreset);
    // TODO ara caldria afegir els presets que tinguem guardats en QSettins, o altres tipus d'arxius tipus XML o ".ini"
    loadCustomWindowLevelPresets();
    connect(CustomWindowLevelsRepository::getRepository(), SIGNAL(changed()), this, SLOT(updateCustomWindowLevels()));
}

VoiLutPresetsToolData::~VoiLutPresetsToolData()
{
}

void VoiLutPresetsToolData::addPreset(const VoiLut &preset, GroupsLabel group)
{
    if (!m_presetsByDescription.contains(preset.getExplanation()))
    {
        m_presetsByDescription.insert(preset.getExplanation(), preset);
        m_groupsByDescription.insert(preset.getExplanation(), group);
        emit presetAdded(preset);
    }
}

void VoiLutPresetsToolData::removePreset(const VoiLut &preset)
{
    if (m_presetsByDescription.contains(preset.getExplanation()))
    {
        emit presetRemoved(preset);
        m_presetsByDescription.remove(preset.getExplanation());
        m_groupsByDescription.remove(preset.getExplanation());
    }
}

void VoiLutPresetsToolData::removePresetsFromGroup(GroupsLabel group)
{
    QList<VoiLut> presetsFromGroup = getPresetsFromGroup(group);

    foreach (const VoiLut &preset, presetsFromGroup)
    {
        removePreset(preset);
    }
}

bool VoiLutPresetsToolData::containsPreset(const QString &description) const
{
    return m_presetsByDescription.contains(description);
}

VoiLut VoiLutPresetsToolData::getFromDescription(const QString &description) const
{
    return m_presetsByDescription[description];
}

VoiLutPresetsToolData::GroupsLabel VoiLutPresetsToolData::getGroup(const QString &description) const
{
    return m_groupsByDescription[description];
}

QStringList VoiLutPresetsToolData::getDescriptionsFromGroup(GroupsLabel group) const
{
    QStringList descriptionList;

    foreach (const QString &description, m_groupsByDescription.keys())
    {
        if (m_groupsByDescription[description] == group)
        {
            descriptionList.append(description);
        }
    }
    
    return descriptionList;
}

QList<VoiLut> VoiLutPresetsToolData::getPresetsFromGroup(GroupsLabel group) const
{
    QStringList descriptionsFromGroup = getDescriptionsFromGroup(group);
    QList<VoiLut> presetsFromGroup;

    foreach (const QString &description, descriptionsFromGroup)
    {
        presetsFromGroup.append(m_presetsByDescription[description]);
    }

    return presetsFromGroup;
}

const VoiLut& VoiLutPresetsToolData::getCurrentPreset() const
{
    return m_currentPreset;
}

const QString& VoiLutPresetsToolData::getCurrentPresetName() const
{
    return m_currentPreset.getExplanation();
}

void VoiLutPresetsToolData::updatePreset(const VoiLut &preset)
{
    if (m_presetsByDescription.contains(preset.getExplanation()))
    {
        m_presetsByDescription[preset.getExplanation()] = preset;

        if (m_currentPreset.getExplanation() == preset.getExplanation() && m_currentPreset != preset)
        {
            m_currentPreset = preset;
            emit currentPresetChanged(preset);
        }
    }
    else
    {
        DEBUG_LOG(QString("The given preset [%1] does not exist in the presets container. Nothing will be done.").arg(preset.getExplanation()));
    }
}

QString VoiLutPresetsToolData::getCustomPresetName()
{
    return tr("Custom");
}

void VoiLutPresetsToolData::setCustomVoiLut(VoiLut voiLut)
{
    voiLut.setExplanation(getCustomPresetName());
    setCurrentPreset(voiLut);
}

void VoiLutPresetsToolData::selectPreset(const QString &presetName)
{
    if (m_presetsByDescription.contains(presetName) && m_currentPreset.getExplanation() != presetName)
    {
        m_currentPreset = m_presetsByDescription[presetName];
        emit presetSelected(m_currentPreset);
    }
}

void VoiLutPresetsToolData::setCurrentPreset(const VoiLut &preset)
{
    if (m_presetsByDescription.contains(preset.getExplanation()))
    {
        updatePreset(preset);
    }
    else
    {
        addPreset(preset, Other);
    }

    selectPreset(preset.getExplanation());
}

void VoiLutPresetsToolData::loadCustomWindowLevelPresets()
{
    foreach (WindowLevel *customWindowLevel, CustomWindowLevelsRepository::getRepository()->getItems())
    {
        addPreset(*customWindowLevel, UserDefined);
    }
}

void VoiLutPresetsToolData::updateCustomWindowLevels()
{
    this->removePresetsFromGroup(UserDefined);
    this->loadCustomWindowLevelPresets();
}

}
