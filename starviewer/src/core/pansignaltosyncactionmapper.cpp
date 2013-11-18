#include "pansignaltosyncactionmapper.h"

#include "qviewer.h"

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
    if (m_viewer)
    {
        double currentPanFactor[3];
        m_viewer->getCurrentPanFactor(currentPanFactor);
        mapToSyncAction(currentPanFactor);
    }
}


void PanSignalToSyncActionMapper::mapSignal()
{
    if (m_viewer)
    {
        connect(m_viewer, SIGNAL(panChanged(double*)), SLOT(mapToSyncAction(double*)));
    }
}

void PanSignalToSyncActionMapper::unmapSignal()
{
    if (m_viewer)
    {
        disconnect(m_viewer, SIGNAL(panChanged(double*)), this, SLOT(mapToSyncAction(double*)));
    }
}

void PanSignalToSyncActionMapper::mapToSyncAction(double *factor)
{
    if (!m_mappedSyncAction)
    {
        m_mappedSyncAction = new PanSyncAction();
    }
    static_cast<PanSyncAction*>(m_mappedSyncAction)->setMotionVector(factor);
    
    emit actionMapped(m_mappedSyncAction);
}

} // End namespace udg
