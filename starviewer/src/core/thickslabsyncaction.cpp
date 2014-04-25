#include "thickslabsyncaction.h"

#include "anatomicalplanesynccriterion.h"
#include "inputsynccriterion.h"
#include "q2dviewer.h"

namespace udg {

ThickSlabSyncAction::ThickSlabSyncAction()
 : SyncAction(), m_slabProjectionMode(0), m_slabThickness(0), m_volume(0)
{
}

ThickSlabSyncAction::~ThickSlabSyncAction()
{
}

void ThickSlabSyncAction::setSlabProjectionMode(int slabProjectionMode)
{
    m_slabProjectionMode = slabProjectionMode;
}

void ThickSlabSyncAction::setSlabThickness(int numberOfSlices)
{
    m_slabThickness = numberOfSlices;
}

void ThickSlabSyncAction::setVolume(Volume *volume)
{
    m_volume = volume;
}

void ThickSlabSyncAction::run(QViewer *viewer)
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(viewer);

    // Limit action to 2D viewers and to secondary inputs
    if (viewer2D && viewer2D->getMainInput() != m_volume)
    {
        viewer2D->setSlabProjectionModeInVolume(viewer2D->indexOfVolume(m_volume), m_slabProjectionMode);
        viewer2D->setSlabThicknessInVolume(viewer2D->indexOfVolume(m_volume), m_slabThickness);
    }
}

void ThickSlabSyncAction::setupMetaData()
{
    m_metaData = SyncActionMetaData("ThickSlabSyncAction", QObject::tr("Thick slab"), "thickSlab");
}

void ThickSlabSyncAction::setupDefaultSyncCriteria()
{
    m_defaultSyncCriteria << new AnatomicalPlaneSyncCriterion() << new InputSyncCriterion();
}

}
