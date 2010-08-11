/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "drawerpoint.h"
#include "logging.h"
// vtk
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkSphereSource.h>

namespace udg {

DrawerPoint::DrawerPoint(QObject *parent) 
: DrawerPrimitive(parent), m_pointRadius(2.0), m_pointActor(NULL)
{
}

DrawerPoint::~DrawerPoint()
{
}

void DrawerPoint::setPosition(double point[3])
{
    m_position[0]=point[0];
    m_position[1]=point[1];
    m_position[2]=point[2];

    emit changed();
}

void DrawerPoint::setPosition(QVector<double> point)
{
    m_position[0]=point[0];
    m_position[1]=point[1];
    m_position[2]=point[2];

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
        m_pointMapper->SetInput(m_pointSphere->GetOutput());
        m_pointActor->SetMapper(m_pointMapper);
    }

    // Li donem els atributs
    m_pointSphere->SetCenter(m_position);

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
        m_pointSphere->SetCenter(m_position);
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
    //m_pointMapper->SetTransformCoordinate(this->getVtkCoordinateObject());

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

double DrawerPoint::getDistanceToPoint(double *point3D)
{
    return sqrt((point3D[0] - m_position[0]) * (point3D[0] - m_position[0]) + (point3D[1] - m_position[1]) * (point3D[1] - m_position[1]) + (point3D[2] - m_position[2]) * (point3D[2] - m_position[2]));
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
