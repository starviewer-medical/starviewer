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

#include "drawerpoint.h"
#include "logging.h"
// Vtk
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkSphereSource.h>

namespace udg {

DrawerPoint::DrawerPoint(QObject *parent)
: DrawerPrimitive(parent), m_pointRadius(2.0), m_pointActor(0), m_pointSphere(0), m_pointMapper(0)
{
}

DrawerPoint::~DrawerPoint()
{
    if (m_pointActor)
    {
        m_pointActor->Delete();
    }
    if (m_pointSphere)
    {
        m_pointSphere->Delete();
    }
    if (m_pointMapper)
    {
        m_pointMapper->Delete();
    }
}

void DrawerPoint::setPosition(Vector3 point)
{
    m_position = std::move(point);
    emit changed();
}

void DrawerPoint::setRadius(double radius)
{
    m_pointRadius = radius;
}

vtkProp* DrawerPoint::getAsVtkProp()
{
    if (!m_pointActor)
    {
        // Creem el pipeline de l'm_vtkActor
        m_pointActor = vtkActor::New();
        m_pointSphere = vtkSphereSource::New();
        m_pointMapper = vtkPolyDataMapper::New();
        m_pointMapper->SetInputConnection(m_pointSphere->GetOutputPort());
        m_pointActor->SetMapper(m_pointMapper);
    }

    // Li donem els atributs
    m_pointSphere->SetCenter(m_position.data());

    updateVtkActorProperties();

    return m_pointActor;
}

void DrawerPoint::update()
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

void DrawerPoint::updateVtkProp()
{
    if (m_pointActor)
    {
        // Assignem les propietats del punt
        m_pointSphere->SetCenter(m_position.data());
        updateVtkActorProperties();
        this->setModified(false);
    }
    else
    {
        DEBUG_LOG("No es pot actualitzar el punt, ja que encara no s'ha creat!");
    }
}

void DrawerPoint::updateVtkActorProperties()
{
    vtkProperty *properties = m_pointActor->GetProperty();

    // Sistema de coordenades
    // TODO És un mapper 3D i per això no disposa d'aquest mètode Caldria veure si això pot suposar algun
    // problema amb la gestió dels sistemes de coordenades
    // m_pointMapper->SetTransformCoordinate(this->getVtkCoordinateObject());

    // Assignem gruix de l'esfera
    m_pointSphere->SetRadius(m_pointRadius);

    // Assignem opacitat al punt
    properties->SetOpacity(m_opacity);

    // Mirem la visibilitat de l'm_vtkActor
    m_pointActor->SetVisibility(this->isVisible());

    // Assignem color
    QColor color = this->getColor();
    properties->SetColor(color.redF(), color.greenF(), color.blueF());
}

double DrawerPoint::getDistanceToPoint(const Vector3 &point3D, Vector3 &closestPoint)
{
    closestPoint = m_position;
    return (point3D - m_position).length();
}

void DrawerPoint::getBounds(double bounds[6])
{
    for (int i = 0; i < 3; i++)
    {
        bounds[i * 2] = m_position[i] - m_pointRadius;
        bounds[i * 2 + 1] = m_position[i] + m_pointRadius;
    }
}

}
