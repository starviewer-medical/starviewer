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

#include "seedtool.h"
#include "seedtooldata.h"
#include "q2dviewer.h"
#include "drawerpoint.h"
#include "drawer.h"
// Vtk's
#include <vtkCommand.h>

namespace udg {

SeedTool::SeedTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent)
{
    m_toolName = "SeedTool";
    m_hasSharedData = false;
    m_hasPersistentData = true;

    m_myData = new SeedToolData;

    m_2DViewer = Q2DViewer::castFromQViewer(viewer);

    m_state = None;
    m_drawn = false;
    m_myData->setVolume(m_2DViewer->getMainInput());
}

SeedTool::~SeedTool()
{
}

void SeedTool::handleEvent(unsigned long eventID)
{
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            setSeed();
            break;

        case vtkCommand::MouseMoveEvent:
            doSeeding();
            break;

        case vtkCommand::LeftButtonReleaseEvent:
            endSeeding();
            break;

        default:
            break;
    }
}

void SeedTool::setToolData(ToolData *data)
{
    // Fem aquesta comparació perquè a vegades ens passa la data que ja tenim a m_myData
    if (m_myData != data)
    {
        // Creem de nou les dades
        m_toolData = data;
        m_myData = qobject_cast<SeedToolData*>(data);
        // Si tenim dades vol dir que ja hem pintat abans la seed si el volume ha canviat
        if (m_2DViewer->getMainInput() != m_myData->getVolume())
        {
            // Canvi de input
            m_drawn = false;
            m_myData->setVolume(m_2DViewer->getMainInput());
            // Si tenim dades vol dir que el viewer ha eliminat el punt pel que el posem a 0 perquè es torni a crear
            m_myData->setPoint(NULL);
        }
        else
        {
            // Canvi de tool
            m_drawn = true;
            m_2DViewer->getDrawer()->erasePrimitive(m_myData->getPoint());
            m_myData->setPoint(NULL);
            m_myData->setSeedPosition(m_myData->getSeedPosition());
            m_2DViewer->getDrawer()->draw(m_myData->getPoint(), m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
        }
    }
}

void SeedTool::setSeed()
{
    Q_ASSERT(m_2DViewer);

    m_state = Seeding;
    updateSeedPosition();
}

void SeedTool::doSeeding()
{
    Q_ASSERT(m_2DViewer);

    if (m_state == Seeding)
    {
        updateSeedPosition();
    }
}

void SeedTool::endSeeding()
{
    m_state = None;
}

void SeedTool::updateSeedPosition()
{
    Q_ASSERT(m_2DViewer);

    Vector3 seedPosition;
    if (m_2DViewer->getCurrentCursorImageCoordinate(seedPosition.data()))
    {
        //DEBUG_LOG(QString("Seed Pos: [%1,%2,%3]").arg(seedPosition[0]).arg(seedPosition[1]).arg(seedPosition[2]));

        m_myData->setSeedPosition(seedPosition);
        // TODO Apanyo perquè funcioni de moment, però s'ha d'arreglar
        // S'hauria d'emetre únicament "seedChanged()" i prou
        m_2DViewer->setSeedPosition(seedPosition);
        emit seedChanged(seedPosition.x, seedPosition.y, seedPosition.z);

        if (!m_drawn)
        {
            m_2DViewer->getDrawer()->draw(m_myData->getPoint(), m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
            m_drawn = true;
        }
        else
        {
            m_myData->getPoint()->update();
            m_2DViewer->render();
        }

        //m_2DViewer->getDrawer()->draw(m_myData->getPoint(), m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
    }
}

void SeedTool::setSeed(const Vector3 &seedPosition, int slice)
{
    Q_ASSERT(m_2DViewer);

    m_myData->setSeedPosition(seedPosition);
    m_2DViewer->setSeedPosition(seedPosition);
    emit seedChanged(seedPosition.x, seedPosition.y, seedPosition.z);
    //DEBUG_LOG(QString("Seed Pos: [%1,%2,%3], slice = %4").arg(seedPosition[0]).arg(seedPosition[1]).arg(seedPosition[2]).arg(slice));
    m_2DViewer->getDrawer()->draw(m_myData->getPoint(), m_2DViewer->getView(), slice);
}

ToolData *SeedTool::getToolData() const
{
    return m_myData;
}

}
