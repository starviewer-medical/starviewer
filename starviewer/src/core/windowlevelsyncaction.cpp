#include "windowlevelsyncaction.h"

#include "qviewer.h"
#include "q2dviewer.h"
#include "windowlevelpresetstooldata.h"
#include "inputsynccriterion.h"

namespace udg {

WindowLevelSyncAction::WindowLevelSyncAction()
 : SyncAction()
{
}

WindowLevelSyncAction::~WindowLevelSyncAction()
{
}

void WindowLevelSyncAction::setWindowLevel(const WindowLevel &windowLevel)
{
    m_windowLevel = windowLevel;
}

void WindowLevelSyncAction::setVolume(Volume *volume)
{
    m_volume = volume;
}

void WindowLevelSyncAction::run(QViewer *viewer)
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(viewer);

    if (viewer2D)
    {
        int index = viewer2D->indexOfVolume(m_volume);
        int group;
        WindowLevelPresetsToolData *currentWindowLevelPresetsToolData = viewer2D->getWindowLevelDataForVolume(index);

        // If the stored window level is in a group and isn't custom, then select it; otherwise, just set it
        if (currentWindowLevelPresetsToolData->getGroup(m_windowLevel, group) && group != WindowLevelPresetsToolData::CustomPreset)
        {
            WindowLevel wl;
            currentWindowLevelPresetsToolData->getFromDescription(m_windowLevel.getName(), wl);
            viewer2D->setWindowLevelInVolume(index, wl);
        }
        else
        {
            viewer2D->setWindowLevelInVolume(index, m_windowLevel);
        }
    }
    else
    {
        viewer->getWindowLevelData()->setCurrentPreset(m_windowLevel);
    }
}

void WindowLevelSyncAction::setupMetaData()
{
    m_metaData = SyncActionMetaData("WindowLevelSyncAction", QObject::tr("window level"), "windowLevel");
}

void WindowLevelSyncAction::setupDefaultSyncCriteria()
{
    m_defaultSyncCriteria << new InputSyncCriterion();
}

} // End namespace udg
