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

#include "voxel.h"

#include <QString>
#include <QObject>
#include <limits>

namespace udg {

Voxel::Voxel()
{
}

Voxel::~Voxel()
{
}

void Voxel::addComponent(double x)
{
    m_values << x;
}

double Voxel::getComponent(int i) const
{
    if (i >= m_values.size() || i < 0)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
    else
    {
        return m_values.at(i);
    }
}

int Voxel::getNumberOfComponents() const
{
    return m_values.size();
}

bool Voxel::isEmpty() const
{
    return m_values.isEmpty();
}

void Voxel::reset()
{
    m_values.clear();
}

QString Voxel::getAsQString() const
{
    QString valueString;
    
    switch (m_values.size())
    {
        case 0:
            valueString = QObject::tr("N/A");
            break;

        case 1:
            valueString = QString("%1").arg(m_values.at(0));
            break;

        default:
            valueString = QString("(%1").arg(m_values.at(0));
            for (int i = 1; i < m_values.size(); ++i)
            {
                valueString += QString(", %1").arg(m_values.at(i));
            }
            valueString += ")";
            break;
    }

    return valueString;
}

bool Voxel::operator==(const Voxel &voxel) const
{
    if (getNumberOfComponents() != voxel.getNumberOfComponents())
    {
        return false;
    }

    bool areEqual = true;
    for (int i = 0; i < getNumberOfComponents(); ++i)
    {
        areEqual = areEqual && m_values.at(i) == voxel.m_values.at(i);
    }

    return areEqual;
}

Voxel Voxel::operator+(const Voxel &value) const
{
    Voxel addedVoxel;

    int numberOfComponents = qMax(getNumberOfComponents(), value.getNumberOfComponents());

    for (int i = 0; i < numberOfComponents; ++i)
    {
        double value1 = getNumberOfComponents() > i ? m_values.at(i) : 0.0;
        double value2 = value.getNumberOfComponents() > i ? value.m_values.at(i) : 0.0;
        
        addedVoxel.addComponent(value1 + value2);
    }

    return addedVoxel;
}

Voxel& Voxel::operator+=(const Voxel &voxel)
{
    *this = *this + voxel;
    
    return *this;
}

Voxel Voxel::operator-(const Voxel &value) const
{
    Voxel substractedVoxel;

    int numberOfComponents = qMax(getNumberOfComponents(), value.getNumberOfComponents());

    for (int i = 0; i < numberOfComponents; ++i)
    {
        double value1 = getNumberOfComponents() > i ? m_values.at(i) : 0.0;
        double value2 = value.getNumberOfComponents() > i ? value.m_values.at(i) : 0.0;
        
        substractedVoxel.addComponent(value1 - value2);
    }

    return substractedVoxel;
}

Voxel& Voxel::operator-=(const Voxel &voxel)
{
    *this = *this - voxel;
    
    return *this;
}

Voxel Voxel::operator*(const Voxel &value) const
{
    Voxel multipliedVoxel;

    int numberOfComponents = qMax(getNumberOfComponents(), value.getNumberOfComponents());

    for (int i = 0; i < numberOfComponents; ++i)
    {
        double value1 = getNumberOfComponents() > i ? m_values.at(i) : 1.0;
        double value2 = value.getNumberOfComponents() > i ? value.m_values.at(i) : 1.0;
        
        multipliedVoxel.addComponent(value1 * value2);
    }

    return multipliedVoxel;
}

Voxel Voxel::operator/(double value) const
{
    Voxel dividedVoxel;
    for (int i = 0; i < getNumberOfComponents(); ++i)
    {
        dividedVoxel.addComponent(m_values.at(i) / value);
    }

    return dividedVoxel;
}

} // End namespace udg
