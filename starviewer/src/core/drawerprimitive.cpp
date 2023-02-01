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

#include "drawerprimitive.h"
// Vtk
#include <vtkCoordinate.h>

namespace udg {

DrawerPrimitive::DrawerPrimitive(QObject *parent)
: QObject(parent), m_internalRepresentation(VTKRepresentation), m_isVisible(true), m_coordinateSystem(WorldCoordinateSystem), m_color(QColor(0xe4, 0xc1, 0x7c)),
  m_isFilled(false), m_linePattern(ContinuousLinePattern), m_lineWidth(2.0), m_opacity(1.0), m_modified(false), m_vtkProp(nullptr), m_referenceCount(0),
  m_coordinate(0)
{
    m_isErasable = true;
    connect(this, SIGNAL(changed()), SLOT(setModified()));
}

DrawerPrimitive::~DrawerPrimitive()
{
    emit dying(this);

    if (m_coordinate)
    {
        m_coordinate->Delete();
    }
}

void DrawerPrimitive::setVisibility(bool visible)
{
    m_isVisible = visible;
    emit changed();
}

void DrawerPrimitive::visibilityOn()
{
    setVisibility(true);
}

void DrawerPrimitive::visibilityOff()
{
    setVisibility(false);
}

bool DrawerPrimitive::isVisible() const
{
    return m_isVisible;
}

void DrawerPrimitive::setCoordinateSystem(int system)
{
    m_coordinateSystem = system;
    emit changed();
}

int DrawerPrimitive::getCoordinateSystem() const
{
    return m_coordinateSystem;
}

void DrawerPrimitive::setColor(QColor color)
{
    m_color = color;
    emit changed();
}

QColor DrawerPrimitive::getColor() const
{
    return m_color;
}

void DrawerPrimitive::setFilled(bool fill)
{
    m_isFilled = fill;
    emit changed();
}

bool DrawerPrimitive::isFilled() const
{
    return m_isFilled;
}

void DrawerPrimitive::setLinePattern(int pattern)
{
    m_linePattern = pattern;
    emit changed();
}

int DrawerPrimitive::getLinePattern() const
{
    return m_linePattern;
}

void DrawerPrimitive::setLineWidth(double width)
{
    m_lineWidth = width;
    emit changed();
}

double DrawerPrimitive::getLineWidth() const
{
    return m_lineWidth;
}

void DrawerPrimitive::setOpacity(double opacity)
{
    m_opacity = opacity;
    emit changed();
}

double DrawerPrimitive::getOpacity() const
{
    return m_opacity;
}

void DrawerPrimitive::setErasable(bool erasable)
{
    m_isErasable = erasable;
}

bool DrawerPrimitive::isErasable() const
{
    return m_isErasable;
}

vtkProp* DrawerPrimitive::getAsVtkProp()
{
    return m_vtkProp;
}

bool DrawerPrimitive::isModified() const
{
    return m_modified;
}

// HACK Mètodes de sucedani d'smart pointer
void DrawerPrimitive::increaseReferenceCount()
{
    m_referenceCount++;
}

void DrawerPrimitive::decreaseReferenceCount()
{
    if (m_referenceCount > 0)
    {
        m_referenceCount--;
    }
}

int DrawerPrimitive::getReferenceCount() const
{
    return m_referenceCount;
}

bool DrawerPrimitive::hasOwners() const
{
    return m_referenceCount > 0;
}

// FI Mètodes de sucedani d'smart pointer

void DrawerPrimitive::setModified(bool modified)
{
    m_modified = modified;
}

vtkCoordinate* DrawerPrimitive::getVtkCoordinateObject()
{
    if (!m_coordinate)
    {
        m_coordinate = vtkCoordinate::New();
    }
    switch (m_coordinateSystem)
    {
        case WorldCoordinateSystem:
            m_coordinate->SetCoordinateSystemToWorld();
            break;

        case DisplayCoordinateSystem:
            m_coordinate->SetCoordinateSystemToDisplay();
            break;
    }
    return m_coordinate;
}

}
