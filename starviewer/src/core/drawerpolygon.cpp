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

#include "drawerpolygon.h"
#include "glutessellator.h"
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

DrawerPolygon::DrawerPolygon(QObject *parent)
 : DrawerPrimitive(parent), m_pointsChanged(false), m_vtkPolyData(0), m_vtkPoints(0), m_vtkCellArray(0), m_vtkActor(0), m_vtkBackgroundActor(0), m_vtkMapper(0),
   m_vtkPropAssembly(0)
{
}

DrawerPolygon::~DrawerPolygon()
{
    emit dying(this);

    if (m_vtkPropAssembly)
    {
        m_vtkPolyData->Delete();
        m_vtkPoints->Delete();
        m_vtkCellArray->Delete();
        m_vtkActor->Delete();
        m_vtkBackgroundActor->Delete();
        m_vtkMapper->Delete();
        m_vtkPropAssembly->Delete();
    }
}

void DrawerPolygon::addVertex(const Vector3 &point)
{
    m_pointsList << point;
    emit changed();
    m_pointsChanged = true;
}

void DrawerPolygon::setVertex(int i, const Vector3 &point)
{
    if (i >= m_pointsList.count() || i < 0)
    {
        addVertex(point);
    }
    else
    {
        m_pointsList[i] = point;
        emit changed();
        m_pointsChanged = true;
    }
}

void DrawerPolygon::removeVertices()
{
    m_pointsList.clear();
    emit changed();
    m_pointsChanged = true;
}

Vector3 DrawerPolygon::getVertex(int i) const
{
    if (i >= m_pointsList.count() || i < 0)
    {
        return Vector3();
    }
    else
    {
        return m_pointsList.at(i).data();
    }
}

QList<Line3D> DrawerPolygon::getSegments()
{
    QList<Line3D> polygonSegments;

    int numberOfSegments = getNumberOfPoints();
    for (int i = 0; i < numberOfSegments - 1; ++i)
    {
        Point3D firstPoint(getVertex(i).data());
        Point3D secondPoint(getVertex(i + 1).data());
        Line3D segment(firstPoint, secondPoint);
        polygonSegments << segment;
    }
    // Must add last segment corresponding to the last point to the first of the polygon
    Point3D firstPoint(getVertex(numberOfSegments - 1).data());
    Point3D secondPoint(getVertex(0).data());
    Line3D segment(firstPoint, secondPoint);
    polygonSegments << segment;

    return polygonSegments;
}

vtkProp* DrawerPolygon::getAsVtkProp()
{
    updateVtkProp();
    return m_vtkPropAssembly;
}

void DrawerPolygon::update()
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

void DrawerPolygon::updateVtkProp()
{
    // La pipeline s'ha de construir la primera vegada, tant si hi ha hagut modificacions com si no
    if (!m_vtkPropAssembly)
    {
        buildVtkPipeline();
        updateVtkActorProperties();
    }

    if (this->isModified())
    {
        updateVtkActorProperties();
        this->setModified(false);
    }

    // Si hi ha hagut modificacions dels punts reconstruïm els polígons de VTK
    if (m_pointsChanged)
    {
        buildVtkPoints();
        m_pointsChanged = false;
    }
}

void DrawerPolygon::buildVtkPipeline()
{
    m_vtkCellArray = vtkCellArray::New();
    m_vtkPoints = vtkPoints::New();
    m_vtkPolyData = vtkPolyData::New();
    m_vtkMapper = vtkPolyDataMapper2D::New();
    m_vtkMapper->SetInputData(m_vtkPolyData);
    m_vtkActor = vtkActor2D::New();
    m_vtkActor->SetMapper(m_vtkMapper);
    m_vtkBackgroundActor = vtkActor2D::New();
    m_vtkBackgroundActor->SetMapper(m_vtkMapper);
    m_vtkPropAssembly = vtkPropAssembly::New();
    m_vtkPropAssembly->AddPart(m_vtkBackgroundActor);
    m_vtkPropAssembly->AddPart(m_vtkActor);
}

void DrawerPolygon::buildVtkPoints()
{
    // Primer comprovem si el polígon és tancat. En cas que l'últim i el primer no coincideixin, l'afegim
    // TODO es podria comprovar si com a mínim té tres punts, sinó, no es pot considerar polígon
    bool extraVertex = false;
    if (!m_pointsList.isEmpty())
    {
        auto firstPoint = m_pointsList.first();
        auto lastPoint = m_pointsList.last();
        if (firstPoint != lastPoint)
        {
            extraVertex = true;
        }
    }

    // Especifiquem el nombre de vèrtexs que té el polígon
    int numberOfVertices = m_pointsList.count() + (extraVertex ? 1 : 0);
    m_vtkCellArray->Reset();

    if (this->isFilled())
    {
        GluTessellator tessellator;
        tessellator.tessellate(m_pointsList);
        
        QList<Vector3> vertices = tessellator.getVertices();
        m_vtkPoints->SetNumberOfPoints(vertices.size());

        for (int i = 0; i < vertices.size(); i++)
        {
            m_vtkPoints->SetPoint(i, vertices[i].data());
        }

        const QList<GluTessellator::Triangle> &triangles = tessellator.getTriangles();
        DEBUG_LOG(QString("%1 triangles").arg(triangles.size()));

        for (int i = 0; i < triangles.size(); i++)
        {
            m_vtkCellArray->InsertNextCell(3);
            for (int j = 0; j < 3; j++)
            {
                m_vtkCellArray->InsertCellPoint(triangles[i].indices[j]);
            }
        }

        m_vtkPolyData->Initialize();
        // Assignem els punts al polydata
        m_vtkPolyData->SetPoints(m_vtkPoints);
        m_vtkPolyData->SetPolys(m_vtkCellArray);
    }
    else
    {
        if (numberOfVertices > 0)
        {
            m_vtkCellArray->InsertNextCell(numberOfVertices);
        }
        m_vtkPoints->SetNumberOfPoints(numberOfVertices);

        // Donem els punts/vèrtexs
        int i = 0;
        foreach (const auto &vertex, m_pointsList)
        {
            m_vtkPoints->InsertPoint(i, vertex.data());
            m_vtkCellArray->InsertCellPoint(i);
            i++;
        }

        if (extraVertex)
        {
            // Tornem a afegir el primer punt
            m_vtkPoints->InsertPoint(numberOfVertices - 1, m_pointsList.at(0).data());
            m_vtkCellArray->InsertCellPoint(numberOfVertices - 1);
        }
        m_vtkPolyData->Initialize();
        // Assignem els punts al polydata
        m_vtkPolyData->SetPoints(m_vtkPoints);
        m_vtkPolyData->SetLines(m_vtkCellArray);
    }
}

void DrawerPolygon::updateVtkActorProperties()
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

int DrawerPolygon::getNumberOfPoints() const
{
    return m_pointsList.count();
}

double DrawerPolygon::getDistanceToPointInDisplay(const Vector3 &displayPoint, Vector3 &closestDisplayPoint,
                                                  std::function<Vector3(const Vector3&)> worldToDisplay)
{
    QList<Vector3> pointsListDisplay;

    foreach (auto point, m_pointsList)
    {
        pointsListDisplay.append(worldToDisplay(point));
    }

    int closestEdge;
    return MathTools::getPointToClosestEdgeDistance(displayPoint, pointsListDisplay, true, closestDisplayPoint, closestEdge);
}

std::array<double, 4> DrawerPolygon::getDisplayBounds(std::function<Vector3(const Vector3&)> worldToDisplay)
{
    double minX, maxX, minY, maxY;
    minX = minY = std::numeric_limits<double>::infinity();
    maxX = maxY = -std::numeric_limits<double>::infinity();

    foreach (auto point, m_pointsList)
    {
        auto pointDisplay = worldToDisplay(point);
        minX = std::min(pointDisplay.x, minX);
        maxX = std::max(pointDisplay.x, maxX);
        minY = std::min(pointDisplay.y, minY);
        maxY = std::max(pointDisplay.y, maxY);
    }

    return std::array<double, 4>{{minX, maxX, minY, maxY}};
}

void DrawerPolygon::get2DPlaneIndices(int &xIndex, int &yIndex) const
{
    // We guess on which plane is lying the polygon
    bool xEqual = true;
    bool yEqual = true;
    bool zEqual = true;
    int numberOfPoints = getNumberOfPoints();
    for (int i = 0; i < numberOfPoints - 1; ++i)
    {
        auto p1 = getVertex(i);
        auto p2 = getVertex(i + 1);

        if (!qFuzzyCompare(p1[0], p2[0]))
        {
            xEqual = false;
        }

        if (!qFuzzyCompare(p1[1], p2[1]))
        {
            yEqual = false;
        }

        if (!qFuzzyCompare(p1[2], p2[2]))
        {
            zEqual = false;
        }
    }

    if (!xEqual && !yEqual && zEqual)
    {
        xIndex = 0;
        yIndex = 1;
    }
    else if (!xEqual && yEqual && !zEqual)
    {
        xIndex = 0;
        yIndex = 2;
    }
    else if (xEqual && !yEqual && !zEqual)
    {
        xIndex = 1;
        yIndex = 2;
    }
    else
    {
        // The result for other combinations is undetermined
        xIndex = -1;
        yIndex = -1;
    }
}

vtkPolyData* DrawerPolygon::getVtkPolyData() const
{
    return m_vtkPolyData;
}

}
