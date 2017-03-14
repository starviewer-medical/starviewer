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

#include "slicingtool.h"
#include "logging.h"
#include "q2dviewer.h"
#include "volume.h"

// Vtk
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

SlicingTool::SlicingTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent)
{
    m_slicingMode = SliceMode;
    m_mouseMovement = false;
    m_numberOfImages = 1;
    m_inputHasPhases = false;
    m_forcePhaseMode = false;
    m_state = None;
    m_toolName = "SlicingTool";
    m_startPosition = QPoint(0, 0);
    m_currentPosition = QPoint(0, 0);
    m_2DViewer = Q2DViewer::castFromQViewer(viewer);
    // Ens assegurem que desde la creació tenim un viewer vàlid
    Q_ASSERT(m_2DViewer);

    // Cada cop que canvïi l'input cal fer algunes inicialitzacions
    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(inputChanged(Volume*)));
    inputChanged(m_2DViewer->getMainInput());
}

SlicingTool::~SlicingTool()
{
    m_viewer->unsetCursor();
}

void SlicingTool::startSlicing()
{
    m_state = Slicing;
    m_startPosition = m_2DViewer->getEventPosition();
    // Calculem el nombre d'imatges que manipulem
    computeImagesForScrollMode();
    m_screenSize = m_2DViewer->getRenderWindowSize();
}

void SlicingTool::doSlicing()
{
    if (m_state == Slicing)
    {
        Q_ASSERT(m_screenSize.isValid());
        m_viewer->setCursor(QCursor(QPixmap(":/images/cursors/scroll.svg")));
        m_currentPosition.setY(m_2DViewer->getEventPosition().y());

        // Increment normalitzat segons la mida de la finestra i el nombre de llesques
        double increase = (1.75 * (m_currentPosition.y() - m_startPosition.y()) / (double)m_screenSize.height()) * m_numberOfImages;
        m_startPosition.setY(m_currentPosition.y());

        // Canviem un nombre de llesques segons el desplaçament del mouse
        int value = qRound(increase);
        if (value == 0)
        {
            if (increase > 0)
            {
                value = 1;
            }
            else if (increase < 0)
            {
                value = -1;
            }
        }
        this->updateIncrement(value);
    }
}

void SlicingTool::endSlicing()
{
    m_viewer->unsetCursor();
    m_state = None;
}

void SlicingTool::inputChanged(Volume *input)
{
    chooseBestDefaultScrollMode(input);
    m_mouseMovement = false;
    m_state = None;
    m_inputHasPhases = false;
    if (input)
    {
        if (input->getNumberOfPhases() > 1)
        {
            m_inputHasPhases = true;
        }
    }
    else
    {
        DEBUG_LOG("L'input introduit és NULL!");
    }
}

void SlicingTool::switchSlicingMode()
{
    if (m_inputHasPhases)
    {
        if (m_slicingMode == SliceMode)
        {
            m_slicingMode = PhaseMode;
        }
        else
        {
            m_slicingMode = SliceMode;
        }
    }
    else
    {
    }
}

void SlicingTool::updateIncrement(int increment)
{
    // Si mantenim control apretat sempe mourem fases independentment de l'slicing mode
    if (m_forcePhaseMode)
    {
        m_2DViewer->setPhase(m_2DViewer->getCurrentPhase() + increment);
    }
    // Altrament continuem amb el comportament habitual
    else
    {
        switch (m_slicingMode)
        {
            case SliceMode:
                m_2DViewer->setSlice(m_2DViewer->getCurrentSlice() + increment);
                break;

            case PhaseMode:
                m_2DViewer->setPhase(m_2DViewer->getCurrentPhase() + increment);
                break;
        }
    }
}

void SlicingTool::computeImagesForScrollMode()
{
    if (m_forcePhaseMode)
    {
        m_numberOfImages = m_2DViewer->getNumberOfPhases();
    }
    else
    {
        if (m_slicingMode == SliceMode)
        {
            m_numberOfImages = m_2DViewer->getMaximumSlice();
        }
        else
        {
            m_numberOfImages = m_2DViewer->getNumberOfPhases();
        }
    }
}

void SlicingTool::chooseBestDefaultScrollMode(Volume *input)
{
    // Per defecte sempre serà aquest excepte quan només tenim 1 imatge i tenim fases
    m_slicingMode = SliceMode;
    if (input)
    {
        if (input->getNumberOfPhases() > 1 && input->getNumberOfSlicesPerPhase() <= 1)
        {
            m_slicingMode = PhaseMode;
        }
        else
        {
        }
    }
}

SlicingTool::SlicingMode SlicingTool::getSlicingMode()
{
    return m_slicingMode;
}

}
