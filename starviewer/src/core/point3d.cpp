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

#include "point3d.h"

#include <limits>

#include "mathtools.h"

#include <QString>

namespace udg {

Point3D::Point3D()
{
    for (int i = 0; i < 3; ++i)
    {
        m_point[i] = 0.0;
    }
}

Point3D::Point3D(double point[3])
{
    for (int i = 0; i < 3; ++i)
    {
        m_point[i] = point[i];
    }
}

Point3D::Point3D(double x, double y, double z)
{
    m_point[0] = x;
    m_point[1] = y;
    m_point[2] = z;
}

Point3D::~Point3D()
{
}

void Point3D::set(int i, double value)
{
    if (MathTools::isInsideRange(i, 0, 2))
    {
        m_point[i] = value;
    }
}

double Point3D::at(int i) const
{
    if (MathTools::isInsideRange(i, 0, 2))
    {
        return m_point[i];
    }
    else
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
}

double& Point3D::operator[](int index)
{
    return m_point[index];
}

const double& Point3D::operator[](int index) const
{
    return m_point[index];
}

bool Point3D::operator==(const Point3D &point)
{
    bool areEqual = true;
    for (int i = 0; i < 3; ++i)
    {
        areEqual = areEqual && qFuzzyCompare(m_point[i], point.m_point[i]);
    }

    return areEqual;
}

const QString Point3D::getAsQString() const
{
    QString string;

    string = QString("(%1, %2, %3)").arg(m_point[0]).arg(m_point[1]).arg(m_point[2]);
    
    return string;
}

double* Point3D::getAsDoubleArray()
{
    return m_point;
}

} // End namespace udg
