#include "windowlevelpresetstooldata.h"
#include "customwindowlevelsrepository.h"
#include "logging.h"
#include <QStringList>

namespace udg {

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
    addPreset(WindowLevel(0, 0, tr("Custom")), CustomPreset);
    // TODO ara caldria afegir els presets que tinguem guardats en QSettins, o altres tipus d'arxius tipus XML o ".ini"
    loadCustomWindowLevelPresets();
    connect(CustomWindowLevelsRepository::getRepository(), SIGNAL(changed()), this, SLOT(updateCustomWindowLevels()));
}

WindowLevelPresetsToolData::~WindowLevelPresetsToolData()
{
}

void WindowLevelPresetsToolData::addPreset(const WindowLevel &windowLevel, int group)
{
    QString description = windowLevel.getName();
    if (!m_presets.contains(description))
    {
        WindowLevelStruct data = { windowLevel.getWidth(), windowLevel.getCenter(), group };
        m_presets.insert(description, data);
        if (group == WindowLevelPresetsToolData::FileDefined)
        {
            m_fileDefinedPresets << description;
        }
        emit presetAdded(description);
    }
}

void WindowLevelPresetsToolData::removePreset(const QString &description)
{
    if (m_presets.contains(description))
    {
        int group;
        if (getGroup(description, group))
        {
            if (group == FileDefined)
            {
                m_fileDefinedPresets.removeAt(m_fileDefinedPresets.indexOf(description));
            }
        }
        m_presets.remove(description);
        emit presetRemoved(description);
    }
}

void WindowLevelPresetsToolData::removePresetsFromGroup(int group)
{
    QMutableMapIterator<QString, WindowLevelStruct> iterator(m_presets);
    while (iterator.hasNext())
    {
        iterator.next();
        if (iterator.value().m_group == group)
        {
            emit presetRemoved(iterator.key());
            iterator.remove();
        }
    }

    if (group == FileDefined)
    {
        m_fileDefinedPresets.clear();
    }
}

bool WindowLevelPresetsToolData::getWindowLevelFromDescription(const QString &description, double &window, double &level)
{
    bool ok = true;
    if (m_presets.contains(description))
    {
        WindowLevelStruct data = m_presets.value(description);
        window = data.m_window;
        level = data.m_level;
    }
    else
    {
        ok = false;
        window = 0.0;
        level = 0.0;
    }
    return ok;
}

bool WindowLevelPresetsToolData::getGroup(const QString &description, int &group)
{
    bool ok = true;
    if (m_presets.contains(description))
    {
        WindowLevelStruct data = m_presets.value(description);
        group = data.m_group;
    }
    else
    {
        ok = false;
        group = Other;
    }
    return ok;
}

QStringList WindowLevelPresetsToolData::getDescriptionsFromGroup(int group)
{
    QStringList descriptionList;
    QMapIterator<QString, WindowLevelStruct> iterator(m_presets);
    while (iterator.hasNext())
    {
        iterator.next();
        if (iterator.value().m_group == group)
        {
            descriptionList << iterator.key();
        }
    }
    return descriptionList;
}

QString WindowLevelPresetsToolData::getCurrentPreset() const
{
    return m_currentPreset;
}

int WindowLevelPresetsToolData::getFileDefinedPresetIndex(const QString &preset) const
{
    return m_fileDefinedPresets.indexOf(preset);
}

void WindowLevelPresetsToolData::updateCurrentFileDefinedPreset(double window, double level)
{
    int group;
    if (getGroup(m_currentPreset, group))
    {
        if (group == FileDefined)
        {
            WindowLevelStruct data = { window, level, FileDefined };
            m_presets.insert(m_currentPreset, data);
            emit currentWindowLevel(window, level);
            // En aquest cas no fem emit presetChanged(preset) perquè sinó provocaria un bucle infinit
            // Només cal notificar el valor perquè el diàleg de custom WW/WL actualitzi el seu valor
        }
        else
        {
            DEBUG_LOG("El preset actual no és de tipus FileDefined. No s'actualitza cap valor de window/level");
        }
    }
    else
    {
        DEBUG_LOG("El preset actual no existeix al contenidor de presets. No s'actualitza cap valor de window/level");
    }
}

void WindowLevelPresetsToolData::setCustomWindowLevel(double window, double level)
{
    WindowLevelStruct data = { window, level, CustomPreset };
    m_presets.insert(tr("Custom"), data);
    emit currentWindowLevel(window, level);
    emit presetChanged(tr("Custom"));
    m_currentPreset = tr("Custom");
}

void WindowLevelPresetsToolData::activatePreset(const QString &preset)
{
    double window, level;
    if (this->getWindowLevelFromDescription(preset, window, level))
    {
        m_currentPreset = preset;
        emit currentWindowLevel(window, level);
        emit presetChanged(preset);
    }
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
