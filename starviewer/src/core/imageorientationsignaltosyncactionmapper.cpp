/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "imageorientationsignaltosyncactionmapper.h"

#include "q2dviewer.h"
#include "patientorientation.h"
#include "imageorientationsyncaction.h"

namespace udg {

ImageOrientationSignalToSyncActionMapper::ImageOrientationSignalToSyncActionMapper(QObject *parent)
 : SignalToSyncActionMapper(parent)
{
}

ImageOrientationSignalToSyncActionMapper::~ImageOrientationSignalToSyncActionMapper()
{
}

void ImageOrientationSignalToSyncActionMapper::mapProperty()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);
    if (viewer2D)
    {
        mapToSyncAction(viewer2D->getCurrentDisplayedImagePatientOrientation());
    }
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
