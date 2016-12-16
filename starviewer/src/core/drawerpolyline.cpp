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

#include "drawerpolyline.h"
#include "logging.h"
#include "mathtools.h"
// Vtk
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkActor2D.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkPropAssembly.h>

namespace udg {

DrawerPolyline::DrawerPolyline(QObject *parent)
 : DrawerPrimitive(parent), m_vtkPolydata(0), m_vtkPoints(0), m_vtkCellArray(0), m_vtkActor(0), m_vtkBackgroundActor(0), m_vtkMapper(0), m_vtkPropAssembly(0)
{
    m_vtkActor = NULL;
}

DrawerPolyline::~DrawerPolyline()
{
    emit dying(this);

    if (m_vtkPolydata)
    {
        m_vtkPolydata->Delete();
    }

    if (m_vtkPoints)
    {
        m_vtkPoints->Delete();
    }

    if (m_vtkCellArray)
    {
        m_vtkCellArray->Delete();
    }

    if (m_vtkActor)
    {
        m_vtkActor->Delete();
    }
    if (m_vtkBackgroundActor)
    {
        m_vtkBackgroundActor->Delete();
    }
    if (m_vtkMapper)
    {
        m_vtkMapper->Delete();
    }
}

void DrawerPolyline::addPoint(const Vector3 &point)
{
    m_pointsList.append(point);
    emit changed();
}

void DrawerPolyline::setPoint(int i, const Vector3 &point)
{
    if (i >= m_pointsList.count() || i < 0)
    {
        addPoint(point);
    }
    else
    {
        m_pointsList.removeAt(i);
        m_pointsList.insert(i, point);
        emit changed();
    }
}

Vector3 DrawerPolyline::getPoint(int position) const
{
    if (position >= m_pointsList.count())
    {
        return Vector3();
    }
    else
    {
        return m_pointsList[position];
    }
}

void DrawerPolyline::removePoint(int i)
{
    m_pointsList.removeAt(i);
    emit changed();
}

void DrawerPolyline::deleteAllPoints()
{
    m_pointsList.clear();
}

vtkProp* DrawerPolyline::getAsVtkProp()
{
    if (!m_vtkPropAssembly)
    {
        m_vtkPropAssembly = vtkPropAssembly::New();

        buildVtkPoints();
        // Creem el pipeline de l'm_vtkActor
        m_vtkActor = vtkActor2D::New();
        m_vtkBackgroundActor = vtkActor2D::New();
        m_vtkMapper = vtkPolyDataMapper2D::New();

        m_vtkActor->SetMapper(m_vtkMapper);
        m_vtkBackgroundActor->SetMapper(m_vtkMapper);
        m_vtkMapper->SetInputData(m_vtkPolydata);
        // Li donem els atributs
        updateVtkActorProperties();

        m_vtkPropAssembly->AddPart(m_vtkBackgroundActor);
        m_vtkPropAssembly->AddPart(m_vtkActor);
    }
    return m_vtkPropAssembly;
}

void DrawerPolyline::update()
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

void DrawerPolyline::updateVtkProp()
{
    if (m_vtkPropAssembly)
    {
        m_vtkPolydata->Reset();
        buildVtkPoints();
        updateVtkActorProperties();
        this->setModified(false);
    }
    else
    {
        DEBUG_LOG("No es pot actualitzar la polilínia, ja que encara no s'ha creat!");
    }
}

void DrawerPolyline::buildVtkPoints()
{
    if (!m_vtkPolydata)
    {
        m_vtkPolydata = vtkPolyData::New();
        m_vtkPoints = vtkPoints::New();
        m_vtkCellArray = vtkCellArray::New();
    }

    // Especifiquem el nombre de vèrtexs que té la polilinia
    int numberOfVertices = m_pointsList.count();
    m_vtkCellArray->InsertNextCell(numberOfVertices);
    m_vtkPoints->SetNumberOfPoints(numberOfVertices);

    // Donem els punts
    int i = 0;
    foreach (const auto &vertex, m_pointsList)
    {
        m_vtkPoints->InsertPoint(i, vertex.x, vertex.y, vertex.z);
        m_vtkCellArray->InsertCellPoint(i);
        i++;
    }

    // Assignem els punts al polydata
    m_vtkPolydata->SetPoints(m_vtkPoints);

    m_vtkPolydata->SetLines(m_vtkCellArray);
}

void DrawerPolyline::updateVtkActorProperties()
{
    // Sistema de coordenades
    m_vtkMapper->SetTransformCoordinate(this->getVtkCoordinateObject());
    // Estil de la línia
    m_vtkActor->GetProperty()->SetLineStipplePattern(m_linePattern);
    m_vtkBackgroundActor->GetProperty()->SetLineStipplePattern(m_linePattern);
    // Assignem gruix de la línia
    m_vtkActor->GetProperty()->SetLineWidth(m_lineWidth);
    m_vtkBackgroundActor->GetProperty()->SetLineWidth(m_lineWidth + 2);
    // Assignem opacitat de la línia
    m_vtkActor->GetProperty()->SetOpacity(m_opacity);
    m_vtkBackgroundActor->GetProperty()->SetOpacity(m_opacity);
    // Mirem la visibilitat de l'm_vtkActor
    m_vtkActor->SetVisibility(this->isVisible());
    m_vtkBackgroundActor->SetVisibility(this->isVisible());
    // Assignem color
    QColor color = this->getColor();
    m_vtkActor->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
    m_vtkBackgroundActor->GetProperty()->SetColor(0.0, 0.0, 0.0);
}

int DrawerPolyline::getNumberOfPoints()
{
    return m_pointsList.count();
}

double DrawerPolyline::getDistanceToPoint(const Vector3 &point3D, Vector3 &closestPoint)
{
    int closestEdge;
    return MathTools::getPointToClosestEdgeDistance(point3D, m_pointsList, false, closestPoint, closestEdge);
}

void DrawerPolyline::getBounds(double bounds[6])
{
    if (m_vtkPolydata)
    {
        m_vtkPolydata->GetBounds(bounds);
    }
    else
    {
        memset(bounds, 0.0, sizeof(double) * 6);
    }
}

QList<Vector3> DrawerPolyline::getPointsList() const
{
    return m_pointsList;
}

}
