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

#include "drawercrosshair.h"

#include "q2dviewer.h"
#include "logging.h"
#include "drawerline.h"
#include "mathtools.h"
// Vtk
#include <vtkActor2D.h>
#include <vtkPropAssembly.h>

namespace udg {

DrawerCrossHair::DrawerCrossHair(QObject *parent)
: DrawerPrimitive(parent)
{
    m_vtkPropAssembly = NULL;
    m_lineUp = new DrawerLine();
    m_lineDown = new DrawerLine();
    m_lineLeft = new DrawerLine();
    m_lineRight = new DrawerLine();
    m_lineFront = new DrawerLine();
    m_lineBack = new DrawerLine();
}

DrawerCrossHair::~DrawerCrossHair()
{
    emit dying(this);

    if (m_vtkPropAssembly)
    {
        m_vtkPropAssembly->Delete();
    }
}

void DrawerCrossHair::setPosition(Vector3 position)
{
    m_position = std::move(position);

    // Assignem els punts a la línia 1
    m_lineUp->setFirstPoint(Vector3(m_position.x, m_position.y - 6, m_position.z));
    m_lineUp->setSecondPoint(Vector3(m_position.x, m_position.y - 1, m_position.z));

    // Assignem els punts a la línia 2
    m_lineDown->setFirstPoint(Vector3(m_position.x, m_position.y + 6, m_position.z));
    m_lineDown->setSecondPoint(Vector3(m_position.x, m_position.y + 1, m_position.z));

    // Assignem els punts a la línia 3
    m_lineLeft->setFirstPoint(Vector3(m_position.x - 6, m_position.y, m_position.z));
    m_lineLeft->setSecondPoint(Vector3(m_position.x - 1, m_position.y, m_position.z));

    // Assignem els punts a la línia 4
    m_lineRight->setFirstPoint(Vector3(m_position.x + 6, m_position.y, m_position.z));
    m_lineRight->setSecondPoint(Vector3(m_position.x + 1, m_position.y, m_position.z));

    // Assignem els punts a la línia 5
    m_lineBack->setFirstPoint(Vector3(m_position.x, m_position.y, m_position.z - 6));
    m_lineBack->setSecondPoint(Vector3(m_position.x, m_position.y, m_position.z - 1));

    // Assignem els punts a la línia 6
    m_lineFront->setFirstPoint(Vector3(m_position.x, m_position.y, m_position.z + 6));
    m_lineFront->setSecondPoint(Vector3(m_position.x, m_position.y, m_position.z + 1));

    emit changed();
}

vtkPropAssembly* DrawerCrossHair::getAsVtkPropAssembly()
{
    if (!m_vtkPropAssembly)
    {
        m_vtkPropAssembly = vtkPropAssembly::New();

        // Li donem els atributs
        updateVtkActorProperties();

        m_vtkPropAssembly->AddPart(m_lineUp->getAsVtkProp());
        m_vtkPropAssembly->AddPart(m_lineDown->getAsVtkProp());
        m_vtkPropAssembly->AddPart(m_lineLeft->getAsVtkProp());
        m_vtkPropAssembly->AddPart(m_lineRight->getAsVtkProp());
        m_vtkPropAssembly->AddPart(m_lineBack->getAsVtkProp());
        m_vtkPropAssembly->AddPart(m_lineFront->getAsVtkProp());
    }
    return m_vtkPropAssembly;
}

vtkProp* DrawerCrossHair::getAsVtkProp()
{
    return (vtkProp*)getAsVtkPropAssembly();
}

void DrawerCrossHair::update()
{
    m_lineUp->update();
    m_lineDown->update();
    m_lineLeft->update();
    m_lineRight->update();
    m_lineBack->update();
    m_lineFront->update();
}

void DrawerCrossHair::updateVtkProp()
{
    if (m_vtkPropAssembly)
    {
        m_lineUp->update();
        m_lineDown->update();
        m_lineLeft->update();
        m_lineRight->update();
        m_lineBack->update();
        m_lineFront->update();

        this->setModified(false);
    }
    else
    {
        DEBUG_LOG("No es pot actualitzar la línia, ja que encara no s'ha creat!");
    }
}

void DrawerCrossHair::updateVtkActorProperties()
{
    m_lineUp->setLineWidth(2.0);
    m_lineDown->setLineWidth(2.0);
    m_lineLeft->setLineWidth(2.0);
    m_lineRight->setLineWidth(2.0);
    m_lineBack->setLineWidth(2.0);
    m_lineFront->setLineWidth(2.0);
}

double DrawerCrossHair::getDistanceToPointInDisplay(const Vector3 &displayPoint, Vector3 &closestDisplayPoint,
                                                    std::function<Vector3(const Vector3&)> worldToDisplay)
{
    closestDisplayPoint = worldToDisplay(m_position);
    return (displayPoint - closestDisplayPoint).length();
}

std::array<double, 4> DrawerCrossHair::getDisplayBounds(std::function<Vector3(const Vector3&)> worldToDisplay)
{
    std::array<double, 4> linesBounds[] = { m_lineUp->getDisplayBounds(worldToDisplay),
                                            m_lineDown->getDisplayBounds(worldToDisplay),
                                            m_lineLeft->getDisplayBounds(worldToDisplay),
                                            m_lineRight->getDisplayBounds(worldToDisplay),
                                            m_lineFront->getDisplayBounds(worldToDisplay),
                                            m_lineBack->getDisplayBounds(worldToDisplay) };
    auto bounds = linesBounds[0];

    for (auto lineBounds : linesBounds)
    {
        bounds[0] = std::min(lineBounds[0], bounds[0]);
        bounds[1] = std::max(lineBounds[1], bounds[1]);
        bounds[2] = std::min(lineBounds[2], bounds[2]);
        bounds[3] = std::max(lineBounds[3], bounds[3]);
    }

    return bounds;
}

void DrawerCrossHair::setVisibility(bool visible)
{
    m_isVisible = visible;
    m_lineUp->setVisibility(visible);
    m_lineDown->setVisibility(visible);
    m_lineLeft->setVisibility(visible);
    m_lineRight->setVisibility(visible);
    m_lineBack->setVisibility(visible);
    m_lineFront->setVisibility(visible);
    emit changed();
}

}
