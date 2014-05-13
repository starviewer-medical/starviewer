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

#include "synchronizetool.h"
#include "q2dviewer.h"
#include "synchronizetooldata.h"
#include "logging.h"

namespace udg {

SynchronizeTool::SynchronizeTool(QViewer *viewer, QObject *parent)
: Tool(viewer, parent)
{
    m_toolData = 0;
    m_toolName = "SynchronizeTool";
    m_hasSharedData = true;
    m_q2dviewer = Q2DViewer::castFromQViewer(viewer);
    m_lastSlice = m_q2dviewer->getCurrentSlice();
    m_roundLostSpacingBetweenSlices = 0.0;

    connect(m_q2dviewer, SIGNAL(volumeChanged(Volume*)), SLOT(reset()));
    connect(m_q2dviewer, SIGNAL(viewChanged(int)), SLOT(reset()));

    setToolData(new SynchronizeToolData());

    m_enabled = false;
}

SynchronizeTool::~SynchronizeTool()
{

}

void SynchronizeTool::setToolData(ToolData *data)
{
    if (m_toolData)
    {
        setEnabled(false);
    }

    this->m_toolData = dynamic_cast<SynchronizeToolData*>(data);
}

ToolData *SynchronizeTool::getToolData() const
{
    return this->m_toolData;
}

void SynchronizeTool::setEnabled(bool enabled)
{
    if (enabled)
    {
        if (!m_enabled)
        {
            connect(m_toolData, SIGNAL(sliceChanged()), SLOT(applySliceChanges()));
            connect(m_q2dviewer, SIGNAL(sliceChanged(int)), SLOT(setIncrement(int)));

            m_enabled = enabled;
        }

        reset();
    }
    else
    {
        disconnect(m_toolData, SIGNAL(sliceChanged()), this, SLOT(applySliceChanges()));
        disconnect(m_q2dviewer, SIGNAL(sliceChanged(int)), this, SLOT(setIncrement(int)));

        m_enabled = false;
    }
}

void SynchronizeTool::setIncrement(int slice)
{
    if (m_lastView == m_q2dviewer->getCurrentAnatomicalPlaneLabel())
    {
        double currentSpacingBetweenSlices = m_q2dviewer->getCurrentSpacingBetweenSlices();
        DEBUG_LOG(QString("setIncrement::currentSpacingBetweenSlices = %1").arg(currentSpacingBetweenSlices));
        // Si la imatge no té espai entre llesques (0.0), llavors li donem un valor nominal
        if (currentSpacingBetweenSlices == 0.0)
        {
            currentSpacingBetweenSlices = 1.0;
        }

        // Distancia incrementada
        double increment = (slice - m_lastSlice) * currentSpacingBetweenSlices;
        m_lastSlice = slice;
        disconnect(m_toolData, SIGNAL(sliceChanged()), this, SLOT(applySliceChanges()));
        this->m_toolData->setIncrement(increment, m_q2dviewer->getCurrentAnatomicalPlaneLabel());
        connect(m_toolData, SIGNAL(sliceChanged()), SLOT(applySliceChanges()));
    }
    else
    {
        // No es posa l'increment però s'actualitza la vista
        m_lastView = m_q2dviewer->getCurrentAnatomicalPlaneLabel();
    }
}

void SynchronizeTool::applySliceChanges()
{
    if (m_q2dviewer->getCurrentAnatomicalPlaneLabel() == m_toolData->getIncrementView())
    {
        double currentSpacingBetweenSlices = m_q2dviewer->getCurrentSpacingBetweenSlices();
        DEBUG_LOG(QString("applySliceChanges::currentSpacingBetweenSlices = %1").arg(currentSpacingBetweenSlices));
        // Si la imatge no té espai entre llesques (0.0), llavors li donem un valor nominal
        if (currentSpacingBetweenSlices == 0.0)
        {
            currentSpacingBetweenSlices = 1.0;
        }

        double sliceIncrement = (this->m_toolData->getIncrement() / currentSpacingBetweenSlices) + m_roundLostSpacingBetweenSlices;
        int slices = qRound(sliceIncrement);
        m_roundLostSpacingBetweenSlices = sliceIncrement - slices;
        disconnect(m_viewer, SIGNAL(sliceChanged(int)), this, SLOT(setIncrement(int)));

        int nextSlice = m_lastSlice + slices;
        if (nextSlice > m_q2dviewer->getMaximumSlice())
        {
            // Fixem a la última llesca per si hi ha l'slicinc cíclic activat
            nextSlice = m_q2dviewer->getMaximumSlice();
        }
        else if (nextSlice < 0)
        {
            // Fixem a la primera llesca per si hi ha l'slicinc cíclic activat
            nextSlice = 0;
        }

        m_q2dviewer->setSlice(nextSlice);

        m_lastSlice += slices;
        connect(m_viewer, SIGNAL(sliceChanged(int)), SLOT(setIncrement(int)));
    }
}

void SynchronizeTool::reset()
{
    if (m_q2dviewer->hasInput())
    {
        m_lastSlice = m_q2dviewer->getCurrentSlice();
        m_lastView = m_q2dviewer->getCurrentAnatomicalPlaneLabel();
        m_roundLostSpacingBetweenSlices = 0.0;
    }
}

void SynchronizeTool::handleEvent(unsigned long eventID)
{
    Q_UNUSED(eventID);
}

}
