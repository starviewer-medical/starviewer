#include "zoomfactorsignaltosyncactionmapper.h"

#include "qviewer.h"

namespace udg {

ZoomFactorSignalToSyncActionMapper::ZoomFactorSignalToSyncActionMapper(QObject *parent)
 : SignalToSyncActionMapper(parent)
{
}

ZoomFactorSignalToSyncActionMapper::~ZoomFactorSignalToSyncActionMapper()
{
}

void ZoomFactorSignalToSyncActionMapper::mapSignal()
{
    if (m_viewer)
    {
        connect(m_viewer, SIGNAL(zoomFactorChanged(double)), SLOT(mapToSyncAction(double)));
    }
}

void ZoomFactorSignalToSyncActionMapper::unmapSignal()
{
    if (m_viewer)
    {
        disconnect(m_viewer, SIGNAL(zoomFactorChanged(double)), this, SLOT(mapToSyncAction(double)));
    }
}

void ZoomFactorSignalToSyncActionMapper::mapToSyncAction(double factor)
{
    if (!m_mappedSyncAction)
    {
        m_mappedSyncAction = new ZoomFactorSyncAction();
    }
    static_cast<ZoomFactorSyncAction*>(m_mappedSyncAction)->setZoomFactor(factor);
    
    emit actionMapped(m_mappedSyncAction);
}

} // End namespace udg
