#ifndef UDGABSTRACTROIDATAPRINTER_H
#define UDGABSTRACTROIDATAPRINTER_H

#include <QMap>
#include <QString>

namespace udg {

class ROIData;
class Q2DViewer;

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
    /// Gets the Standardized Uptake Value measurement from the given ROI data, corresponding to the provided input number
    /// It will be only computed on PT images
    QString getStandardizedUptakeValueMeasureString(ROIData &roiData, int inputIndex, Q2DViewer *viewer) const;

};

} // End namespace udg

#endif
