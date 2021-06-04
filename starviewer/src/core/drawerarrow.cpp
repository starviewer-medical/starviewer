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

#include "drawerarrow.h"

#include "mathtools.h"

#include <vtkActor2D.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkPropAssembly.h>
#include <vtkProperty2D.h>

namespace udg {

DrawerArrow::DrawerArrow(QObject *parent)
    : DrawerPrimitive(parent), m_scale(1)
{
    m_lineMapper->SetInputConnection(m_lineSource->GetOutputPort());
    m_tipTopMapper->SetInputConnection(m_tipTopSource->GetOutputPort());
    m_tipBottomMapper->SetInputConnection(m_tipBottomSource->GetOutputPort());

    m_lineActor->SetMapper(m_lineMapper);
    m_backgroundLineActor->SetMapper(m_lineMapper);
    m_tipTopActor->SetMapper(m_tipTopMapper);
    m_backgroundTipTopActor->SetMapper(m_tipTopMapper);
    m_tipBottomActor->SetMapper(m_tipBottomMapper);
    m_backgroundTipBottomActor->SetMapper(m_tipBottomMapper);

    m_propAssembly->AddPart(m_backgroundTipBottomActor);
    m_propAssembly->AddPart(m_backgroundTipTopActor);
    m_propAssembly->AddPart(m_backgroundLineActor);
    m_propAssembly->AddPart(m_tipBottomActor);
    m_propAssembly->AddPart(m_tipTopActor);
    m_propAssembly->AddPart(m_lineActor);
}

DrawerArrow::~DrawerArrow()
{
    emit dying(this);
}

const Vector3& DrawerArrow::getStartPoint() const
{
    return m_startPoint;
}

void DrawerArrow::setStartPoint(const Vector3 &point)
{
    m_startPoint = point;
    emit changed();
}

const Vector3& DrawerArrow::getEndPoint() const
{
    return m_endPoint;
}

void DrawerArrow::setEndPoint(const Vector3 &point)
{
    m_endPoint = point;
    emit changed();
}

void DrawerArrow::setScale(double scale)
{
    if (scale != 0.0)
    {
        m_scale = scale;
    }
    emit changed();
}

void DrawerArrow::setViewPlaneNormal(const Vector3 &normal)
{
    m_viewPlaneNormal = normal;
    emit changed();
}

vtkProp* DrawerArrow::getAsVtkProp()
{
    return m_propAssembly;
}

double DrawerArrow::getDistanceToPoint(double *point3D, double closestPoint[3])
{
    return MathTools::getPointToFiniteLineDistance(point3D, m_startPoint.toArray().data(), m_endPoint.toArray().data(), closestPoint);
}

void DrawerArrow::getBounds(double bounds[6])
{
    std::array<double, 3> start(m_startPoint), end(m_endPoint);

    for (uint i = 0; i < 3; i++)
    {
        if (start[i] < end[i])
        {
            bounds[i * 2] = start[i];
            bounds[i * 2 + 1] = end[i];
        }
        else
        {
            bounds[i * 2] = end[i];
            bounds[i * 2 + 1] = start[i];
        }
    }
}

void DrawerArrow::update()
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

void DrawerArrow::updateVtkProp()
{
    constexpr double MinimumFactor = 0.01;
    constexpr double MaximumFactor = 0.05;
    constexpr double TipRatio = 0.25;

    m_lineSource->SetPoint1(m_startPoint.x, m_startPoint.y, m_startPoint.z);
    m_lineSource->SetPoint2(m_endPoint.x, m_endPoint.y, m_endPoint.z);
    m_tipTopSource->SetPoint1(m_endPoint.x, m_endPoint.y, m_endPoint.z);
    m_tipBottomSource->SetPoint1(m_endPoint.x, m_endPoint.y, m_endPoint.z);

    // Calculate two points to draw the tip lines (each one from the end to one of these points)
    // The tip will have the shape of an equilateral triangle, i.e. an angle of 60°
    // To achieve this the distance of each point to the line must be tipLength / sqrt(3)
    Vector3 startToEnd = m_endPoint - m_startPoint;
    double arrowLength = startToEnd.length();
    double minimumLength = m_scale * MinimumFactor;
    double maximumLength = m_scale * MaximumFactor;
    double tipLength = qBound(minimumLength, arrowLength * TipRatio, maximumLength);
    Vector3 tipPart = startToEnd / arrowLength * tipLength;
    Vector3 arrowNormal = Vector3::cross(m_viewPlaneNormal, startToEnd);
    arrowNormal.normalize() *= tipLength / std::sqrt(3);
    Vector3 tipTop = m_endPoint - tipPart + arrowNormal;
    Vector3 tipBottom = m_endPoint - tipPart - arrowNormal;

    m_tipTopSource->SetPoint2(tipTop.x, tipTop.y, tipTop.z);
    m_tipBottomSource->SetPoint2(tipBottom.x, tipBottom.y, tipBottom.z);

    updateVtkActorProperties();

    this->setModified(false);
}

void DrawerArrow::updateVtkActorProperties()
{
    std::vector<vtkPolyDataMapper2D*> mappers{m_lineMapper, m_tipTopMapper, m_tipBottomMapper};

    for (vtkPolyDataMapper2D *mapper : mappers)
    {
        mapper->SetTransformCoordinate(this->getVtkCoordinateObject());
    }

    std::vector<vtkProperty2D*> properties{m_lineActor->GetProperty(), m_tipTopActor->GetProperty(), m_tipBottomActor->GetProperty()};
    std::vector<vtkProperty2D*> backgroundProperties{m_backgroundLineActor->GetProperty(), m_backgroundTipTopActor->GetProperty(),
                m_backgroundTipBottomActor->GetProperty()};
    std::vector<vtkProperty2D*> allProperties{properties};
    allProperties.insert(allProperties.end(), backgroundProperties.begin(), backgroundProperties.end());

    for (vtkProperty2D *property : allProperties)
    {
        property->SetLineStipplePattern(m_linePattern);
        property->SetOpacity(m_opacity);
    }

    QColor color = this->getColor();

    for (vtkProperty2D *property : properties)
    {
        property->SetLineWidth(m_lineWidth);
        property->SetColor(color.redF(), color.greenF(), color.blueF());
    }

    for (vtkProperty2D *backgroundProperty : backgroundProperties)
    {
        backgroundProperty->SetLineWidth(m_lineWidth + 2);
        backgroundProperty->SetColor(0, 0, 0);
    }

    m_propAssembly->SetVisibility(this->isVisible());
}

} // namespace udg
