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

#include "changephaseqviewercommand.h"

#include "q2dviewer.h"

namespace udg {

ChangePhaseQViewerCommand::ChangePhaseQViewerCommand(Q2DViewer *viewer, PhasePosition phase, QObject *parent)
 : QViewerCommand(parent)
{
    m_viewer = viewer;
    m_phasePosition = phase;
    m_customPhaseNumber = 0;
}

ChangePhaseQViewerCommand::ChangePhaseQViewerCommand(Q2DViewer *viewer, int phase, QObject *parent)
 : QViewerCommand(parent)
{
    m_viewer = viewer;
    m_phasePosition = CustomPhase;
    m_customPhaseNumber = phase;
}

void ChangePhaseQViewerCommand::execute()
{
    switch (m_phasePosition)
    {
        case MaximumPhase:
            m_viewer->setPhase(m_viewer->getNumberOfPhases()-1);
            break;
        case MinimumPhase:
            m_viewer->setPhase(0);
            break;
        case MiddlePhase:
            {
                int middlePhase = (m_viewer->getNumberOfPhases() - 1) / 2;
                m_viewer->setPhase(middlePhase);
            }
            break;
        case CustomPhase:
            m_viewer->setPhase(qBound(0, m_customPhaseNumber, m_viewer->getNumberOfPhases() - 1));
            break;
    }
    m_viewer->render();
}

}
