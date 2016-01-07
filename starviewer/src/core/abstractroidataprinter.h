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

#ifndef UDGABSTRACTROIDATAPRINTER_H
#define UDGABSTRACTROIDATAPRINTER_H

#include <QMap>
#include <QString>

namespace udg {

class ROIData;
class Q2DViewer;
class Image;

/**
    Abstract interface for classes to analyze the ROIData and make the proper outputs depending on the features of the source data.
    
    This lets make different outputs from the same ROIData collections, i.e. the output results that may be interesting for an area
    made on a PET-CT may differ from the ones from a single MR series.
 */
class AbstractROIDataPrinter {
public:
    AbstractROIDataPrinter(const QMap<int, ROIData> &roiDataMap, const QString &areaString, Q2DViewer *viewer);
    virtual ~AbstractROIDataPrinter();

    /// Gets the string corresponding to the ROIData
    QString getString();

protected:
    /// Gathers the required data to build the final string
    /// Each subclass must implement this method
    virtual void gatherData() = 0;

    /// Formats the data gathered with gatherData() and returns it as a formatted string
    /// Each subclass can re-implement this method
    virtual QString getFormattedDataString() const;
    
    /// Gets the Standardized Uptake Value measurement from the given ROI data, corresponding to the provided image
    /// It will be only computed on PT images
    QString getStandardizedUptakeValueMeasureString(ROIData &roiData, Image *petImage) const;

    /// Gets the value properly formatted as a string accompanied by the units
    QString getFormattedValueString(double value, const QString &units) const;

    /// Gets the current image from the given viewer on the given index. If image is null, because 
    /// some reconstruction is applied, the first image of the input is returned
    Image* getCurrentImage(Q2DViewer *viewer, int inputIndex) const;

protected:
    /// The viewer where the ROI has been drawn
    Q2DViewer *m_2DViewer;
    
    /// The ROI data to make annotation from
    QMap<int, ROIData> m_roiDataMap;

    /// String with the corresponding area of the ROI
    QString m_areaString;

    /// Formatted strings with the values of each quantitative data
    QString m_meanString;
    QString m_standardDeviationString;
    QString m_suvString;
};

} // End namespace udg

#endif
