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

#include <QString>

namespace udg {

class Voxel;

/**
 * @brief The ROIData class computes statistics from voxel data in a ROI.
 *
 * Currently it only takes into account the first component of the voxel,
 * i.e. if the voxel is an RGB color voxel, it only will take into account the red channel.
 *
 * The mean and the standard deviation are computed using the shifted data algorithm
 * (see https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Computing_shifted_data).
 */
class ROIData {
public:
    ROIData();
    ~ROIData();

    /// Clears all the contained data and reinitializes the corresponding values
    void clear();
    
    /// Adds a voxel unless Voxel::isEmpty() is true
    void addVoxel(const Voxel &voxel);

    /// Returns the mean of the added voxels.
    double getMean() const;
    /// Returns the standard deviation of the added voxels.
    double getStandardDeviation() const;
    /// Returns the maximum value of the added voxels.
    double getMaximum() const;

    /// Sets/gets the units of the voxels of this ROI
    void setUnits(const QString &units);
    QString getUnits() const;

    /// Sets/gets the modality of the voxels of this ROI
    void setModality(const QString &modality);
    QString getModality() const;

private:
    /// Number of voxels that have been added.
    int m_count;
    /// Shift to substract from the added voxels (equal to the first value added).
    double m_k;
    /// Sum of shifted data.
    double m_ex;
    /// Sum of squares of shifted data.
    double m_ex2;
    /// Maximum value.
    double m_maximum;
    
    /// Additional optional information of the ROI regarding the units of the voxels and their modality
    QString m_units;
    QString m_modality;
};

} // End namespace udg

#endif
