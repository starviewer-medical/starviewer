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
    AbstractROIDataPrinter(const QMap<int, ROIData> &roiDataMap, const QString &areaString);
    ~AbstractROIDataPrinter();

    /// Gets the string corresponding to the ROIData
    virtual QString getString() const = 0;

protected:
    /// The ROI data to make annotation from
    QMap<int, ROIData> m_roiDataMap;

    /// String with the corresponding area of the ROI
    QString m_areaString;

protected:
    /// Gets the Standardized Uptake Value measurement from the given ROI data, corresponding to the provided image
    /// It will be only computed on PT images
    QString getStandardizedUptakeValueMeasureString(ROIData &roiData, Image *petImage) const;

    /// Gets the value properly formatted as a string accompanied by the units
    QString getFormattedValueString(double value, const QString &units) const;

    /// Gets the current image from the given viewer on the given index. If image is null, because 
    /// some reconstruction is applied, the first image of the input is returned
    Image* getCurrentImage(Q2DViewer *viewer, int inputIndex) const;
};

} // End namespace udg

#endif
