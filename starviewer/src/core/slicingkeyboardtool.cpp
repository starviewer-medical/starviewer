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

#include "slicingkeyboardtool.h"
#include "q2dviewer.h"
#include "volume.h"
#include "study.h"
#include "series.h"
#include "patient.h"
#include "changesliceqviewercommand.h"
#include "renderqviewercommand.h"
// Qt
#include <QTime>
// Vtk
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

SlicingKeyboardTool::SlicingKeyboardTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent)
{
    m_toolName = "SlicingKeyboardTool";
    m_2DViewer = Q2DViewer::castFromQViewer(viewer);
}

SlicingKeyboardTool::~SlicingKeyboardTool()
{
}

void SlicingKeyboardTool::handleEvent(unsigned long eventID)
{
    switch (eventID)
    {
        case vtkCommand::KeyPressEvent:
        {
            QString keySymbol = m_2DViewer->getInteractor()->GetKeySym();
            if (keySymbol == "Home")
            {
                m_2DViewer->setSlice(0);
            }
            else if (keySymbol == "End")
            {
                m_2DViewer->setSlice(m_2DViewer->getMaximumSlice());
            }
            else if (keySymbol == "Up")
            {
                m_2DViewer->setSlice(m_2DViewer->getCurrentSlice() + 1);
            }
            else if (keySymbol == "Down")
            {
                m_2DViewer->setSlice(m_2DViewer->getCurrentSlice() - 1);
            }
            else if (keySymbol == "Left")
            {
                m_2DViewer->setPhase(m_2DViewer->getCurrentPhase() - 1);
            }
            else if (keySymbol == "Right")
            {
                m_2DViewer->setPhase(m_2DViewer->getCurrentPhase() + 1);
            }
            // TODO Vigilar que no es pot fer només començar perquè no hi ha cap viewer seleccionat si no s'aplica cap HP
            else if (keySymbol == "plus" || keySymbol == "minus")
            {
                if ((m_2DViewer->getCurrentSlice() == m_2DViewer->getMaximumSlice() && keySymbol == "plus") ||
                    (m_2DViewer->getCurrentSlice() == m_2DViewer->getMinimumSlice() && keySymbol == "minus"))
                {
                    QList<Volume*> volumesList;
                    int currentVolumeIndex = 0;

                    Volume *currentVolume = m_2DViewer->getMainInput();
                    if (currentVolume != NULL)
                    {
                        Study *currentStudy = currentVolume->getStudy();
                        if (currentStudy != NULL)
                        {
                            Patient *currentPatient = currentStudy->getParentPatient();

                            volumesList = currentPatient->getVolumesList();
                            currentVolumeIndex = volumesList.indexOf(currentVolume);
                            Q_ASSERT_X(currentVolumeIndex >= 0, "", "Volume must be found in the list");

                            int nextVolumeIndex = 0;
                            QViewerCommand *command;
                            if (keySymbol == "plus")
                            {
                                if (currentVolumeIndex >= volumesList.size() - 1)
                                {
                                    nextVolumeIndex = 0;
                                }
                                else
                                {
                                    nextVolumeIndex = currentVolumeIndex + 1;
                                }
                                command = new ChangeSliceQViewerCommand(m_2DViewer, ChangeSliceQViewerCommand::MinimumSlice);
                            }
                            else
                            {
                                if (currentVolumeIndex <= 0)
                                {
                                    nextVolumeIndex = volumesList.size() - 1;
                                }
                                else
                                {
                                    nextVolumeIndex = currentVolumeIndex - 1;
                                }
                                command = new ChangeSliceQViewerCommand(m_2DViewer, ChangeSliceQViewerCommand::MaximumSlice);
                            }
                            Volume *nextVolume = volumesList.at(nextVolumeIndex);
                            m_2DViewer->setInputAsynchronously(nextVolume, command);
                        }
                    }
                }
                else
                {
                    if (keySymbol == "plus")
                    {
                        m_2DViewer->setSlice(m_2DViewer->getCurrentSlice() + 1);
                    }
                    else if (keySymbol == "minus")
                    {
                        m_2DViewer->setSlice(m_2DViewer->getCurrentSlice() - 1);
                    }
                }
            }
        }
            break;

        default:
            break;
    }
}
}
