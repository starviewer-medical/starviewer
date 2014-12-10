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
#include "logging.h"
#include "windowlevel.h"

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

void VoiLutPresetsToolData::addPreset(const WindowLevel &preset, int group)
{
    QString presetName = preset.getName();
    QMapIterator<int, VoiLut> iterator(m_presetsByGroup);
    bool found = false;
    while (iterator.hasNext() && !found)
    {
        iterator.next();
        if (iterator.value().getExplanation() == presetName)
        {
            found = true;
        }
    }
    
    if (!found)
    {
        m_presetsByGroup.insert(group, preset);
    }
    emit presetAdded(preset);
}

void VoiLutPresetsToolData::removePreset(const WindowLevel &preset)
{
    QMutableMapIterator<int, VoiLut> iterator(m_presetsByGroup);
    while (iterator.hasNext())
    {
        iterator.next();
        if (iterator.value().getExplanation() == preset.getName())
        {
            emit presetRemoved(preset);
            iterator.remove();
        }
    }
}

void VoiLutPresetsToolData::removePresetsFromGroup(int group)
{
    foreach (const VoiLut &preset, m_presetsByGroup.values(group))
    {
        emit presetRemoved(preset.getWindowLevel());
    }
    m_presetsByGroup.remove(group);
}

bool VoiLutPresetsToolData::getFromDescription(const QString &description, WindowLevel &preset)
{
    bool found = false;
    QMapIterator<int, VoiLut> iterator(m_presetsByGroup);
    while (iterator.hasNext() && !found)
    {
        iterator.next();
        if (iterator.value().getExplanation() == description)
        {
            preset = iterator.value().getWindowLevel();
            found = true;
        }
    }

    return found;
}

bool VoiLutPresetsToolData::getGroup(const WindowLevel &preset, int &group)
{
    bool found = false;
    foreach (int key, m_presetsByGroup.keys())
    {
        QStringList presetsNames = getDescriptionsFromGroup(key);
        if (presetsNames.contains(preset.getName()))
        {
            found = true;
            group = key;
            break;
        }
    }
    
    if (!found)
    {
        group = Other;
    }

    return found;
}

QStringList VoiLutPresetsToolData::getDescriptionsFromGroup(int group)
{
    QStringList descriptionList;
    QList<WindowLevel> windowLevelList = getPresetsFromGroup(group);
    foreach (const WindowLevel &preset, windowLevelList)
    {
        descriptionList << preset.getName();
    }
    descriptionList.sort();
    
    return descriptionList;
}

namespace {

bool windowLevelLessThanSortOperator(const WindowLevel &wl1, const WindowLevel &wl2)
{
    return wl1.getName() < wl2.getName();
}

}

QList<WindowLevel> VoiLutPresetsToolData::getPresetsFromGroup(int group)
{
    QList<VoiLut> voiLutList = m_presetsByGroup.values(group);
    QList<WindowLevel> windowLevelList;

    foreach (const VoiLut &voiLut, voiLutList)
    {
        windowLevelList.append(voiLut.getWindowLevel());
    }

    // The list has to be sorted to be coherent with the overloaded version wich returns a QStringList
    qSort(windowLevelList.begin(), windowLevelList.end(), windowLevelLessThanSortOperator);
    
    return windowLevelList;
}

QString VoiLutPresetsToolData::getCurrentPresetName() const
{
    return m_currentPreset.getExplanation();
}

WindowLevel VoiLutPresetsToolData::getCurrentPreset() const
{
    return m_currentPreset.getWindowLevel();
}

bool VoiLutPresetsToolData::updatePreset(const WindowLevel &preset)
{
    bool found = false;
    QMutableMapIterator<int, VoiLut> iterator(m_presetsByGroup);
    while (iterator.hasNext() && !found)
    {
        iterator.next();
        if (iterator.value().getExplanation() == preset.getName())
        {
            iterator.value() = preset;
            found = true;

            if (m_currentPreset.getExplanation() == preset.getName())
            {
                m_currentPreset = preset;
            }
        }
    }

    if (!found)
    {
        DEBUG_LOG(QString("The given preset [%1: %2, %3] does not exist in the presets container. Nothing will be done.")
            .arg(preset.getName()).arg(preset.getWidth()).arg(preset.getCenter()));
    }
    
    return found;
}

QString VoiLutPresetsToolData::getCustomPresetName()
{
    return tr("Custom");
}

void VoiLutPresetsToolData::setCustomWindowLevel(double window, double level)
{
    WindowLevel customPreset(window, level, getCustomPresetName());
    updatePreset(customPreset);
    if (m_currentPreset.getExplanation() != customPreset.getName())
    {
        selectCurrentPreset(customPreset.getName());
    }
    else
    {
        emit currentPresetChanged(customPreset);
    }
}

void VoiLutPresetsToolData::selectCurrentPreset(const QString &presetName)
{
    WindowLevel preset;
    if (getFromDescription(presetName, preset))
    {
        m_currentPreset = preset;
        emit presetSelected(preset);
    }
}

void VoiLutPresetsToolData::setCurrentPreset(const WindowLevel &preset)
{
    WindowLevel internalPreset;
    if (getFromDescription(preset.getName(), internalPreset))
    {
        int group;
        if (getGroup(preset, group))
        {
            if (group == CustomPreset)
            {
                updatePreset(preset);
            }
        }
    }
    else
    {
        addPreset(preset, Other);
    }

    selectCurrentPreset(preset.getName());
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
