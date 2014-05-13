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

#include "zoomfactorsignaltosyncactionmapper.h"

#include "qviewer.h"
#include "zoomfactorsyncaction.h"

namespace udg {

ZoomFactorSignalToSyncActionMapper::ZoomFactorSignalToSyncActionMapper(QObject *parent)
 : SignalToSyncActionMapper(parent)
{
}

ZoomFactorSignalToSyncActionMapper::~ZoomFactorSignalToSyncActionMapper()
{
}

void ZoomFactorSignalToSyncActionMapper::mapProperty()
{
    if (m_viewer)
    {
        mapToSyncAction(m_viewer->getCurrentZoomFactor()  / m_viewer->getRenderWindowSize().height());
    }
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
