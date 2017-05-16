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

#include "windowlevel.h"

#include <cmath>

#include <vtkWindowLevelLookupTable.h>

namespace udg {

WindowLevel::WindowLevel()
{
    m_width = 0.0;
    m_center = 0.0;
}

WindowLevel::WindowLevel(double width, double center, const QString &name)
{
    m_width = width;
    m_center = center;
    m_name = name;
}

WindowLevel::~WindowLevel()
{
}

void WindowLevel::setName(const QString &name)
{
    m_name = name;
}

void WindowLevel::setWidth(double width)
{
    m_width = width;
}

void WindowLevel::setCenter(double center)
{
    m_center = center;
}

const QString& WindowLevel::getName() const
{
    return m_name;
}

double WindowLevel::getWidth() const
{
    return m_width;
}

double WindowLevel::getCenter() const
{
    return m_center;
}

bool WindowLevel::isValid() const
{
    if (qAbs(m_width) < 1.0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool WindowLevel::valuesAreEqual(const WindowLevel &windowLevel) const
{
    if (m_width == windowLevel.getWidth() && m_center == windowLevel.getCenter())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool WindowLevel::operator==(const WindowLevel &windowLevelToCompare) const
{
    if (m_width == windowLevelToCompare.getWidth() && m_center == windowLevelToCompare.getCenter() && m_name == windowLevelToCompare.getName())
    {
        return true;
    }
    else
    {
        return false;
    }
}

vtkLookupTable* WindowLevel::toVtkLookupTable() const
{
    vtkWindowLevelLookupTable *lut = vtkWindowLevelLookupTable::New();
    lut->SetWindow(qAbs(m_width));
    lut->SetLevel(m_center);
    lut->SetInverseVideo(std::signbit(m_width));
    lut->Build();
    return lut;
}

}
