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

#include "drawerline.h"
#include "logging.h"
#include "mathtools.h"
// Vtk
#include <vtkLineSource.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkActor2D.h>
#include <vtkPropAssembly.h>

namespace udg {

DrawerLine::DrawerLine(QObject *parent)
 : DrawerPrimitive(parent), m_vtkLineSource(0), m_vtkActor(0), m_vtkBackgroundActor(0), m_vtkMapper(0), m_vtkPropAssembly(0)
{
}

DrawerLine::~DrawerLine()
{
    emit dying(this);

    if (m_vtkActor)
    {
        m_vtkActor->Delete();
    }

    if (m_vtkBackgroundActor)
    {
        m_vtkBackgroundActor->Delete();
    }

    if (m_vtkLineSource)
    {
        m_vtkLineSource->Delete();
    }

    if (m_vtkMapper)
    {
        m_vtkMapper->Delete();
    }
}

void DrawerLine::setFirstPoint(Vector3 point)
{
    m_firstPoint = std::move(point);
    emit changed();
}

void DrawerLine::setSecondPoint(Vector3 point)
{
    m_secondPoint = std::move(point);
    emit changed();
}

vtkProp* DrawerLine::getAsVtkProp()
{
    if (!m_vtkPropAssembly)
    {
        m_vtkPropAssembly = vtkPropAssembly::New();

        // Creem el pipeline de l'm_vtkActor
        m_vtkActor = vtkActor2D::New();
        m_vtkBackgroundActor = vtkActor2D::New();
        m_vtkLineSource = vtkLineSource::New();
        m_vtkMapper = vtkPolyDataMapper2D::New();

        // Assignem els punts a la línia
        m_vtkLineSource->SetPoint1(m_firstPoint.data());
        m_vtkLineSource->SetPoint2(m_secondPoint.data());

        m_vtkActor->SetMapper(m_vtkMapper);
        m_vtkBackgroundActor->SetMapper(m_vtkMapper);
        m_vtkMapper->SetInputConnection(m_vtkLineSource->GetOutputPort());

        // Li donem els atributs
        updateVtkActorProperties();

        m_vtkPropAssembly->AddPart(m_vtkBackgroundActor);
        m_vtkPropAssembly->AddPart(m_vtkActor);
    }
    return m_vtkPropAssembly;
}

const Vector3& DrawerLine::getFirstPoint() const
{
    return m_firstPoint;
}

const Vector3& DrawerLine::getSecondPoint() const
{
    return m_secondPoint;
}

void DrawerLine::update()
{
    switch (m_internalRepresentation)
    {
        case VTKRepresentation:
            updateVtkProp();
            break;

        case OpenGLRepresentation:
            break;
    }
}

void DrawerLine::updateVtkProp()
{
    if (m_vtkPropAssembly)
    {
        // Assignem els punts a la línia
        m_vtkLineSource->SetPoint1(m_firstPoint.data());
        m_vtkLineSource->SetPoint2(m_secondPoint.data());
        updateVtkActorProperties();
        this->setModified(false);
    }
    else
    {
        DEBUG_LOG("No es pot actualitzar la línia, ja que encara no s'ha creat!");
    }
}

void DrawerLine::updateVtkActorProperties()
{
    vtkProperty2D *properties = m_vtkActor->GetProperty();
    vtkProperty2D *propertiesBackground = m_vtkBackgroundActor->GetProperty();

    // Sistema de coordenades
    m_vtkMapper->SetTransformCoordinate(this->getVtkCoordinateObject());

    // Estil de la línia
    properties->SetLineStipplePattern(m_linePattern);
    propertiesBackground->SetLineStipplePattern(m_linePattern);

    // Assignem gruix de la línia
    properties->SetLineWidth(m_lineWidth);
    propertiesBackground->SetLineWidth(m_lineWidth + 2);

    // Assignem opacitat de la línia
    properties->SetOpacity(m_opacity);
    propertiesBackground->SetOpacity(m_opacity);

    // Mirem la visibilitat de l'm_vtkActor
    m_vtkActor->SetVisibility(this->isVisible());
    m_vtkBackgroundActor->SetVisibility(this->isVisible());

    // Assignem color
    QColor color = this->getColor();
    properties->SetColor(color.redF(), color.greenF(), color.blueF());
    propertiesBackground->SetColor(0.0, 0.0, 0.0);
}

double DrawerLine::getDistanceToPoint(const Vector3 &point3D, Vector3 &closestPoint)
{
    return MathTools::getPointToFiniteLineDistance(point3D, m_firstPoint, m_secondPoint, closestPoint);
}

void DrawerLine::getBounds(double bounds[6])
{
    for (int i = 0; i < 3; i++)
    {
        if (m_firstPoint[i] < m_secondPoint[i])
        {
            bounds[i * 2] = m_firstPoint[i];
            bounds[i * 2 + 1] = m_secondPoint[i];
        }
        else
        {
            bounds[i * 2] = m_secondPoint[i];
            bounds[i * 2 + 1] = m_firstPoint[i];
        }
    }
}

}
