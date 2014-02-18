#include "pansignaltosyncactionmapper.h"

#include "q2dviewer.h"

namespace udg {

PanSignalToSyncActionMapper::PanSignalToSyncActionMapper(QObject *parent)
 : SignalToSyncActionMapper(parent)
{
}

PanSignalToSyncActionMapper::~PanSignalToSyncActionMapper()
{
}

void PanSignalToSyncActionMapper::mapProperty()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);
    if (viewer2D)
    {
        double centerPoint[3];

        if (viewer2D->getCurrentFocalPoint(centerPoint))
        {
            mapToSyncAction(centerPoint);
        }
    }
}


void PanSignalToSyncActionMapper::mapSignal()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);
    if (viewer2D)
    {
        connect(viewer2D, SIGNAL(panChanged(double*)), SLOT(mapToSyncAction(double*)));
    }
}

void PanSignalToSyncActionMapper::unmapSignal()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);
    if (viewer2D)
    {
        disconnect(viewer2D, SIGNAL(panChanged(double*)), this, SLOT(mapToSyncAction(double*)));
    }
}

void PanSignalToSyncActionMapper::mapToSyncAction(double *factor)
{
    if (!m_mappedSyncAction)
    {
        m_mappedSyncAction = new PanSyncAction();
    }

    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);
    double dicomWorldPosition[4];
    viewer2D->getDicomWorldCoordinates(factor, dicomWorldPosition);

    static_cast<PanSyncAction*>(m_mappedSyncAction)->setMotionVector(dicomWorldPosition);
    
    emit actionMapped(m_mappedSyncAction);
}

} // End namespace udg
