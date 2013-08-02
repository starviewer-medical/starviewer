#include "signaltosyncactionmapper.h"

namespace udg {

SignalToSyncActionMapper::SignalToSyncActionMapper(QObject *parent)
 : QObject(parent)
{
    m_viewer = 0;
    m_mappedSyncAction = 0;
}

SignalToSyncActionMapper::~SignalToSyncActionMapper()
{
}

void SignalToSyncActionMapper::setViewer(QViewer *viewer)
{
    unmapSignal();
    m_viewer = viewer;
    mapSignal();
}

} // End namespace udg
