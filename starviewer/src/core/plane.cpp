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

#include "plane.h"

namespace udg {

Plane::Plane(Vector3 normal, Vector3 point)
    : m_normal(std::move(normal)), m_point(std::move(point))
{
    Q_ASSERT(qFuzzyCompare(m_normal.length(), 1.0));
}

const Vector3& Plane::getNormal() const
{
    return m_normal;
}

void Plane::setNormal(Vector3 normal)
{
    Q_ASSERT(qFuzzyCompare(normal.length(), 1.0));
    m_normal = std::move(normal);
}

const Vector3& Plane::getPoint() const
{
    return m_point;
}

void Plane::setPoint(Vector3 point)
{
    m_point = std::move(point);
}

bool Plane::operator==(const Plane &plane) const
{
    return this->m_normal == plane.m_normal && this->m_point == plane.m_point;
}

bool Plane::operator!=(const Plane &plane) const
{
    return this->m_normal != plane.m_normal || this->m_point != plane.m_point;
}

double Plane::signedDistanceToPoint(const Vector3 &point) const
{
    double a = m_normal.x, b = m_normal.y, c = m_normal.z, d = -a * m_point.x - b * m_point.y - c * m_point.z;
    return a * point.x + b * point.y + c * point.z + d;
}

} // namespace udg
