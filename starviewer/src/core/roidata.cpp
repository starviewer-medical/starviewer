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

#include "roidata.h"

#include "voxel.h"

#include <cmath>

namespace udg {

ROIData::ROIData()
{
    clear();
}

ROIData::~ROIData()
{
}

void ROIData::clear()
{
    m_count = 0;
    m_k = 0;
    m_ex = 0;
    m_ex2 = 0;
    m_maximum = 0.0;
    m_units = "";
    m_modality = "";
}

void ROIData::addVoxel(const Voxel &voxel)
{
    if (!voxel.isEmpty())
    {
        double x = voxel.getComponent(0);
        if (m_count == 0)
        {
            m_k = x;
            m_maximum = -std::numeric_limits<double>::infinity();
        }
        m_count++;
        m_ex += x - m_k;
        m_ex2 += (x - m_k) * (x - m_k);
        m_maximum = std::max(x, m_maximum);
    }
}

double ROIData::getMean() const
{
    if (m_count > 0)
    {
        return m_k + m_ex / m_count;
    }
    else
    {
        return 0;
    }
}

double ROIData::getStandardDeviation() const
{
    if (m_count > 0)
    {
        return sqrt((m_ex2 - (m_ex * m_ex) / m_count) / (m_count));
    }
    else
    {
        return 0;
    }
}

double ROIData::getMaximum() const
{
    return m_maximum;
}

void ROIData::setUnits(const QString &units)
{
    m_units = units;
}

QString ROIData::getUnits() const
{
    return m_units;
}

void ROIData::setModality(const QString &modality)
{
    m_modality = modality;
}

QString ROIData::getModality() const
{
    return m_modality;
}

} // End namespace udg
