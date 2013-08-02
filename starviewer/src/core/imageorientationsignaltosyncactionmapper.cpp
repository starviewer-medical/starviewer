#include "imageorientationsignaltosyncactionmapper.h"

#include "q2dviewer.h"
#include "patientorientation.h"

namespace udg {

ImageOrientationSignalToSyncActionMapper::ImageOrientationSignalToSyncActionMapper(QObject *parent)
 : SignalToSyncActionMapper(parent)
{
}

ImageOrientationSignalToSyncActionMapper::~ImageOrientationSignalToSyncActionMapper()
{
}

void ImageOrientationSignalToSyncActionMapper::mapSignal()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);
    if (viewer2D)
    {
        connect(viewer2D, SIGNAL(imageOrientationChanged(PatientOrientation)), SLOT(mapToSyncAction(PatientOrientation)));
    }
}

void ImageOrientationSignalToSyncActionMapper::unmapSignal()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);
    if (viewer2D)
    {
        disconnect(viewer2D, SIGNAL(imageOrientationChanged(PatientOrientation)), this, SLOT(mapToSyncAction(PatientOrientation)));
    }
}

void ImageOrientationSignalToSyncActionMapper::mapToSyncAction(const PatientOrientation &orientation)
{
    if (!m_mappedSyncAction)
    {
        m_mappedSyncAction = new ImageOrientationSyncAction();
    }
    static_cast<ImageOrientationSyncAction*>(m_mappedSyncAction)->setImageOrientation(orientation);
    
    emit actionMapped(m_mappedSyncAction);
}

} // End namespace udg
