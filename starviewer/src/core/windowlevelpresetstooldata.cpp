#include "windowlevelpresetstooldata.h"
#include "customwindowlevelsrepository.h"
#include "logging.h"
#include <QStringList>

namespace udg {

const QString WindowLevelPresetsToolData::CustomPresetName = tr("Custom");

WindowLevelPresetsToolData::WindowLevelPresetsToolData(QObject *parent)
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
    addPreset(WindowLevel(0, 0, CustomPresetName), CustomPreset);
    // TODO ara caldria afegir els presets que tinguem guardats en QSettins, o altres tipus d'arxius tipus XML o ".ini"
    loadCustomWindowLevelPresets();
    connect(CustomWindowLevelsRepository::getRepository(), SIGNAL(changed()), this, SLOT(updateCustomWindowLevels()));
}

WindowLevelPresetsToolData::~WindowLevelPresetsToolData()
{
}

void WindowLevelPresetsToolData::addPreset(const WindowLevel &preset, int group)
{
    QString presetName = preset.getName();
    QMapIterator<int, WindowLevel> iterator(m_presetsByGroup);
    bool found = false;
    while (iterator.hasNext() && !found)
    {
        iterator.next();
        if (iterator.value().getName() == presetName)
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

void WindowLevelPresetsToolData::removePreset(const WindowLevel &preset)
{
    QMutableMapIterator<int, WindowLevel> iterator(m_presetsByGroup);
    while (iterator.hasNext())
    {
        iterator.next();
        if (iterator.value().getName() == preset.getName())
        {
            emit presetRemoved(preset);
            iterator.remove();
        }
    }
}

void WindowLevelPresetsToolData::removePresetsFromGroup(int group)
{
    foreach (const WindowLevel &preset, m_presetsByGroup.values(group))
    {
        emit presetRemoved(preset);
    }
    m_presetsByGroup.remove(group);
}

bool WindowLevelPresetsToolData::getFromDescription(const QString &description, WindowLevel &preset)
{
    bool found = false;
    QMapIterator<int, WindowLevel> iterator(m_presetsByGroup);
    while (iterator.hasNext() && !found)
    {
        iterator.next();
        if (iterator.value().getName() == description)
        {
            preset = iterator.value();
            found = true;
        }
    }

    return found;
}

bool WindowLevelPresetsToolData::getGroup(const WindowLevel &preset, int &group)
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

QStringList WindowLevelPresetsToolData::getDescriptionsFromGroup(int group)
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

QList<WindowLevel> WindowLevelPresetsToolData::getPresetsFromGroup(int group)
{
    QList<WindowLevel> windowLevelList = m_presetsByGroup.values(group);
    return windowLevelList;
}

QString WindowLevelPresetsToolData::getCurrentPresetName() const
{
    return m_currentPreset.getName();
}

WindowLevel WindowLevelPresetsToolData::getCurrentPreset() const
{
    return m_currentPreset;
}

bool WindowLevelPresetsToolData::updatePreset(const WindowLevel &preset)
{
    bool found = false;
    QMutableMapIterator<int, WindowLevel> iterator(m_presetsByGroup);
    while (iterator.hasNext() && !found)
    {
        iterator.next();
        if (iterator.value().getName() == preset.getName())
        {
            iterator.value() = preset;
            found = true;
        }
    }

    if (!found)
    {
        DEBUG_LOG(QString("The given preset [%1: %2, %3] does not exist in the presets container. Nothing will be done.")
            .arg(preset.getName()).arg(preset.getWidth()).arg(preset.getCenter()));
    }
    
    return found;
}

void WindowLevelPresetsToolData::setCustomWindowLevel(double window, double level)
{
    WindowLevel customPreset(window, level, CustomPresetName);
    m_presetsByGroup.replace(CustomPreset, customPreset);
    if (m_currentPreset.getName() != customPreset.getName())
    {
        selectCurrentPreset(customPreset.getName());
    }
    else
    {
        emit presetChanged(customPreset);
    }
}

void WindowLevelPresetsToolData::selectCurrentPreset(const QString &presetName)
{
    WindowLevel preset;
    if (getFromDescription(presetName, preset))
    {
        m_currentPreset = preset;
        emit presetSelected(preset);
    }
}

void WindowLevelPresetsToolData::setCurrentPreset(const WindowLevel &preset)
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

void WindowLevelPresetsToolData::loadCustomWindowLevelPresets()
{
    foreach (WindowLevel *customWindowLevel, CustomWindowLevelsRepository::getRepository()->getItems())
    {
        addPreset(*customWindowLevel, UserDefined);
    }
}

void WindowLevelPresetsToolData::updateCustomWindowLevels()
{
    this->removePresetsFromGroup(UserDefined);
    this->loadCustomWindowLevelPresets();
}

}
