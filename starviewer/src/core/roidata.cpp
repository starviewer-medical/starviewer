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

#include <QtCore/qmath.h>
#include <cfloat>

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
    m_voxels.clear();
    m_statisticsAreOutdated = false;
    m_mean = 0.0;
    m_standardDeviation = 0.0;
    m_maximum = 0.0;
    m_units = "";
    m_modality = "";
}

void ROIData::addVoxel(const Voxel &voxel)
{
    if (!voxel.isEmpty())
    {
        m_voxels << voxel;
        m_statisticsAreOutdated = true;
    }
}

double ROIData::getMean()
{
    computeStatistics();
    return m_mean;
}

double ROIData::getStandardDeviation()
{
    computeStatistics();
    return m_standardDeviation;
}

double ROIData::getMaximum()
{
    computeStatistics();
    return m_maximum;
}

double ROIData::getSum()
{
    computeStatistics();
    return m_sum;
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

void ROIData::computeStatistics()
{
    if (!m_statisticsAreOutdated)
    {
        return;
    }

    computeMean();
    computeStandardDeviation();
    computeMaximum();
    computeSum();

    m_statisticsAreOutdated = false;
}

void ROIData::computeMean()
{
    m_mean = 0.0;
    foreach (const Voxel &voxel, m_voxels)
    {
        m_mean += voxel.getComponent(0);
    }

    m_mean = m_mean / m_voxels.size();
}

void ROIData::computeStandardDeviation()
{
    m_standardDeviation = 0.0;
    QList<double> deviations;
    foreach (const Voxel &voxel, m_voxels)
    {
        double individualDeviation = voxel.getComponent(0) - m_mean;
        deviations << (individualDeviation * individualDeviation);
    }

    foreach (double deviation, deviations)
    {
        m_standardDeviation += deviation;
    }

    m_standardDeviation /= deviations.size();
    m_standardDeviation = qSqrt(m_standardDeviation);
}

void ROIData::computeMaximum()
{
    m_maximum = -DBL_MAX;

    foreach (const Voxel &voxel, m_voxels)
    {
        double value = voxel.getComponent(0);
        if (value > m_maximum)
        {
            m_maximum = value;
        }
    }
}

void ROIData::computeSum()
{
    m_sum = 0.0;
    foreach (const Voxel &voxel, m_voxels)
    {
        m_sum += voxel.getComponent(0);
    }
}

} // End namespace udg
