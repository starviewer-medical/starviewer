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

#include "changesliceqviewercommand.h"

#include "q2dviewer.h"

namespace udg {

ChangeSliceQViewerCommand::ChangeSliceQViewerCommand(Q2DViewer *viewer, SlicePosition slice, QObject *parent)
 : QViewerCommand(parent)
{
    m_viewer = viewer;
    m_slicePosition = slice;
    m_customSliceNumber = 0;
}

ChangeSliceQViewerCommand::ChangeSliceQViewerCommand(Q2DViewer *viewer, int slice, QObject *parent)
 : QViewerCommand(parent)
{
    m_viewer = viewer;
    m_slicePosition = CustomSlice;
    m_customSliceNumber = slice;
}

void ChangeSliceQViewerCommand::execute()
{
    switch (m_slicePosition)
    {
        case MaximumSlice:
            m_viewer->setSlice(m_viewer->getMaximumSlice());
            break;
        case MinimumSlice:
            m_viewer->setSlice(m_viewer->getMinimumSlice());
            break;
        case MiddleSlice:
            {
                int middleSlice = (m_viewer->getMaximumSlice() - m_viewer->getMinimumSlice()) / 2;
                m_viewer->setSlice(middleSlice);
            }
            break;
        case CustomSlice:
            m_viewer->setSlice(qBound(m_viewer->getMinimumSlice(), m_customSliceNumber, m_viewer->getMaximumSlice()));
            break;
    }
    m_viewer->render();
}

}
