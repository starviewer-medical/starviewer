#ifndef UDGROIDATAPRINTER_H
#define UDGROIDATAPRINTER_H

#include "abstractroidataprinter.h"

namespace udg {

class Q2DViewer;

/**
    General purpose ROIDataPrinter.
    It will build a string with as much as possible information on it.
 */
class ROIDataPrinter : public AbstractROIDataPrinter {
public:
    ROIDataPrinter(const QMap<int, ROIData> &roiDataMap, const QString &areaString, Q2DViewer *viewer);
    ~ROIDataPrinter();

protected:
    virtual void gatherData();

protected:
    /// The viewer where the ROI has been drawn
    Q2DViewer *m_2DViewer;
};

} // End namespace udg

#endif
