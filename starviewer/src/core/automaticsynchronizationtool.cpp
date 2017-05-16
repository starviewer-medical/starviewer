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

#include "automaticsynchronizationtool.h"
#include "automaticsynchronizationtooldata.h"
#include "q2dviewer.h"
#include "logging.h"
#include "image.h"
#include "series.h"
#include "volume.h"
#include "imageplane.h"
#include "slicelocator.h"

#include <vtkPlane.h>

namespace udg {

AutomaticSynchronizationTool::AutomaticSynchronizationTool(QViewer *viewer, QObject *parent)
: Tool(viewer, parent)
{
    m_toolData = 0;
    m_toolName = "AutomaticSynchronizationTool";
    m_hasSharedData = true;

    m_2DViewer = Q2DViewer::castFromQViewer(viewer);

    m_lastSlice = m_2DViewer->getCurrentSlice();
    m_roundLostSpacingBetweenSlices = 0.0;

    m_sliceLocator = new SliceLocator;

    connect(m_2DViewer, SIGNAL(selected()), SLOT(changePositionIfActive()));
    connect(m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(changePositionIfActive()));
    
    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(reset()));
    connect(m_2DViewer, SIGNAL(viewChanged(int)), SLOT(reset()));

    setToolData(new AutomaticSynchronizationToolData());
}

AutomaticSynchronizationTool::~AutomaticSynchronizationTool()
{
    delete m_sliceLocator;
}

void AutomaticSynchronizationTool::reset()
{
    m_lastSlice = m_2DViewer->getCurrentSlice();
    m_roundLostSpacingBetweenSlices = 0.0;
    initialize();
}

void AutomaticSynchronizationTool::setToolData(ToolData *data)
{
    this->m_toolData = dynamic_cast<AutomaticSynchronizationToolData*>(data);
    connect(m_toolData, SIGNAL(changed()), SLOT(updatePosition()));

    initialize();
}

void AutomaticSynchronizationTool::initialize()
{
    if (m_2DViewer->hasInput())
    {
        QString frameOfReferenceUID = m_2DViewer->getMainInput()->getImage(0)->getParentSeries()->getFrameOfReferenceUID();
        int groupOfActualViewer = m_toolData->getGroupForUID(frameOfReferenceUID);

        if (groupOfActualViewer == -1)
        {
            if (frameOfReferenceUID != "")
            {
                m_toolData->setGroupForUID(frameOfReferenceUID, m_toolData->getNumberOfGroups());
            }
            else
            {
                DEBUG_LOG(QString("Viewer sense frameOfReference, no es tindrà en compte a la sincronitzacio automatica."));
            }
        }

        updateSliceLocator();
        changePositionIfActive();
        updatePosition();
    }
}

void AutomaticSynchronizationTool::updateSliceLocator()
{
    if (m_2DViewer)
    {
        m_sliceLocator->setVolume(m_2DViewer->getMainInput());
        m_sliceLocator->setPlane(m_2DViewer->getView());
    }
}

ToolData* AutomaticSynchronizationTool::getToolData() const
{
    return this->m_toolData;
}

void AutomaticSynchronizationTool::handleEvent(unsigned long eventID)
{
    Q_UNUSED(eventID);
}

void AutomaticSynchronizationTool::changePositionIfActive()
{
    if (m_2DViewer->hasInput() && m_2DViewer->isActive())
    {
        setPositionToToolData();
    }
}

void AutomaticSynchronizationTool::setPositionToToolData()
{
    QString frameOfReference = m_2DViewer->getMainInput()->getImage(0)->getParentSeries()->getFrameOfReferenceUID();
    
    Vector3 center = m_2DViewer->getCurrentImagePlane()->getCenter();

    int slice = m_2DViewer->getCurrentSlice();
    double currentSpacingBetweenSlices = m_2DViewer->getCurrentSpacingBetweenSlices();

    // Si la imatge no té espai entre llesques (0.0), llavors li donem un valor nominal
    if (currentSpacingBetweenSlices == 0.0)
    {
        currentSpacingBetweenSlices = 1.0;
    }

    // Distancia incrementada
    double increment = (slice - m_lastSlice) * currentSpacingBetweenSlices;
    m_lastSlice = slice;

    m_toolData->setPosition(frameOfReference, m_2DViewer->getCurrentAnatomicalPlaneLabel(), center.toArray().data(), increment);
}

void AutomaticSynchronizationTool::updatePosition()
{
    if (m_2DViewer->hasInput() && !m_2DViewer->isActive())
    {
        QString frameOfReference = m_2DViewer->getMainInput()->getImage(0)->getParentSeries()->getFrameOfReferenceUID();

        int activeGroup = m_toolData->getGroupForUID(m_toolData->getSelectedUID());
        int groupOfActualViewer = m_toolData->getGroupForUID(frameOfReference);

        if (groupOfActualViewer != -1)
        {
            if (groupOfActualViewer == activeGroup && m_2DViewer->getCurrentAnatomicalPlaneLabel() == m_toolData->getSelectedView()) //Actualitzem la llesca
            {
                if (m_toolData->getSelectedUID() == frameOfReference)
                {
                    // Actualitzem per posició
                    double *position = m_toolData->getPosition(frameOfReference, m_2DViewer->getCurrentAnatomicalPlaneLabel());
                    
                    int nearestSlice = m_sliceLocator->getNearestSlice(position);
                    if (nearestSlice != -1)
                    {
                        m_2DViewer->setSlice(nearestSlice);
                    }
                }
                else
                {
                    // TODO Check if this code can be actually reached, it seems not. If this code is never reached, it should be removed then
                    double currentSpacingBetweenSlices = m_2DViewer->getCurrentSpacingBetweenSlices();
                    if (currentSpacingBetweenSlices == 0.0)
                    {
                        // Si la imatge no té espai entre llesques (0.0), llavors li donem un valor nominal
                        // TODO En teoria l'spacing mai hauria de poder ser 0.0, tot i així es manté per seguretat
                        currentSpacingBetweenSlices = 1.0;
                    }

                    // Actualitzem per increment
                    double sliceIncrement = (this->m_toolData->getDisplacement() / currentSpacingBetweenSlices) + m_roundLostSpacingBetweenSlices;
                    int slices = qRound(sliceIncrement);
                    m_roundLostSpacingBetweenSlices = sliceIncrement - slices;
                    m_2DViewer->setSlice(m_lastSlice + slices);
                    m_lastSlice += slices;
                }
            }
        }
    }
}

}
