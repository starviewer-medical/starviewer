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

#ifndef UDGROIDATA_H
#define UDGROIDATA_H

#include "voxel.h"

#include <QString>

namespace udg {

/**
    Class to save voxel data contained in a ROI and compute statistics from it.
    Currently it only takes into account the first component of the voxel,
    i.e. if the voxel is an RGB color voxel, it only will take into account the red channel
 */
class ROIData {
public:
    ROIData();
    ~ROIData();

    /// Clears all the contained data and reinitializes the corresponding values
    void clear();
    
    /// Adds a voxel unless Voxel::isEmpty() is true
    void addVoxel(const Voxel &voxel);

    /// Gets the mean/standard deviation/maximum corresponding to the current voxels
    double getMean();
    double getStandardDeviation();
    double getMaximum();
    double getSum();

    /// Sets/gets the units of the voxels of this ROI
    void setUnits(const QString &units);
    QString getUnits() const;

    /// Sets/gets the modality of the voxels of this ROI
    void setModality(const QString &modality);
    QString getModality() const;

private:
    /// Computes all statistics data if needed
    void computeStatistics();

    /// Computes voxels mean
    void computeMean();

    /// Computes voxels standard deviation. Requires mean to be computed before, as it uses the m_mean value
    void computeStandardDeviation();

    /// Computes voxels maximum value
    void computeMaximum();

    /// Computes the sum of the voxel values.
    void computeSum();

private:
    /// The container of the ROI voxels
    QVector<Voxel> m_voxels;
    
    /// Statistic data members
    double m_mean;
    double m_standardDeviation;
    double m_maximum;
    double m_sum;

    /// Used to control whether the statistics data has to be computed or not when requested
    bool m_statisticsAreOutdated;
    
    /// Additional optional information of the ROI regarding the units of the voxels and their modality
    QString m_units;
    QString m_modality;
};

} // End namespace udg

#endif
