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

#include "coresettings.h"
#include "q2dviewer.h"
#include "logging.h"
#include "drawerline.h"
#include "mathtools.h"
// Vtk
#include <vtkCoordinate.h>
#include <vtkPropAssembly.h>

namespace udg {

DrawerCrossHair::DrawerCrossHair(QObject *parent)
: DrawerPrimitive(parent), m_vtkPropAssembly(nullptr)
{
    m_worldCoordinate = vtkCoordinate::New();
    m_worldCoordinate->SetCoordinateSystemToWorld();
    m_lineUp = new DrawerLine();
    m_lineUp->setCoordinateSystem(DisplayCoordinateSystem);
    m_lineUp->getVtkCoordinateObject()->SetReferenceCoordinate(m_worldCoordinate);
    m_lineDown = new DrawerLine();
    m_lineDown->setCoordinateSystem(DisplayCoordinateSystem);
    m_lineDown->getVtkCoordinateObject()->SetReferenceCoordinate(m_worldCoordinate);
    m_lineLeft = new DrawerLine();
    m_lineLeft->setCoordinateSystem(DisplayCoordinateSystem);
    m_lineLeft->getVtkCoordinateObject()->SetReferenceCoordinate(m_worldCoordinate);
    m_lineRight = new DrawerLine();
    m_lineRight->setCoordinateSystem(DisplayCoordinateSystem);
    m_lineRight->getVtkCoordinateObject()->SetReferenceCoordinate(m_worldCoordinate);
}

DrawerCrossHair::~DrawerCrossHair()
{
    m_worldCoordinate->Delete();

    if (m_vtkPropAssembly)
    {
        m_vtkPropAssembly->Delete();
    }
}

void DrawerCrossHair::setCentrePoint(double x, double y, double z)
{
    Settings settings;
    double point1 = settings.getValue(CoreSettings::CrosshairInnerDiameter).toInt() / 2;
    double point2 = settings.getValue(CoreSettings::CrosshairOuterDiameter).toInt() / 2;

    m_centrePoint[0] = x;
    m_centrePoint[1] = y;
    m_centrePoint[2] = z;

    m_worldCoordinate->SetValue(x, y, z);

    // Assignem els punts a la línia 1
    m_lineUp->setFirstPoint(0, point1, 0);
    m_lineUp->setSecondPoint(0, point2, 0);

    // Assignem els punts a la línia 2
    m_lineDown->setFirstPoint(0, -point1, 0);
    m_lineDown->setSecondPoint(0, -point2, 0);

    // Assignem els punts a la línia 3
    m_lineLeft->setFirstPoint(-point1, 0, 0);
    m_lineLeft->setSecondPoint(-point2, 0, 0);

    // Assignem els punts a la línia 4
    m_lineRight->setFirstPoint(point1, 0, 0);
    m_lineRight->setSecondPoint(point2, 0, 0);

    emit changed();
}

vtkProp* DrawerCrossHair::getAsVtkProp()
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
        m_vtkProp = m_vtkPropAssembly;
    }

    return DrawerPrimitive::getAsVtkProp();
}

void DrawerCrossHair::update()
{
    m_lineUp->update();
    m_lineDown->update();
    m_lineLeft->update();
    m_lineRight->update();
}

void DrawerCrossHair::updateVtkProp()
{
    if (m_vtkPropAssembly)
    {
        m_lineUp->update();
        m_lineDown->update();
        m_lineLeft->update();
        m_lineRight->update();

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
}

double DrawerCrossHair::getDistanceToPoint(double *point3D, double closestPoint[3])
{
    closestPoint[0] = m_centrePoint[0];
    closestPoint[1] = m_centrePoint[1];
    closestPoint[2] = m_centrePoint[2];
    return MathTools::getDistance3D(m_centrePoint, point3D);
}

void DrawerCrossHair::getBounds(double bounds[6])
{
    // TODO This is the best we can do for now, we don't have access to enough information here to convert between display and world coordinates.
    bounds[0] = m_centrePoint[0];
    bounds[1] = m_centrePoint[0];
    bounds[2] = m_centrePoint[1];
    bounds[3] = m_centrePoint[1];
    bounds[4] = m_centrePoint[2];
    bounds[5] = m_centrePoint[2];
}

void DrawerCrossHair::setVisibility(bool visible)
{
    m_isVisible = visible;
    m_lineUp->setVisibility(visible);
    m_lineDown->setVisibility(visible);
    m_lineLeft->setVisibility(visible);
    m_lineRight->setVisibility(visible);
    emit changed();
}

}
