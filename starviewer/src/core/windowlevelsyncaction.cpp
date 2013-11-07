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
        viewer2D->setWindowLevelInVolume(viewer2D->indexOfVolume(m_volume), m_windowLevel);
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
