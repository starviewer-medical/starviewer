#ifndef UDGPETCTFUSIONROIDATAPRINTER_H
#define UDGPETCTFUSIONROIDATAPRINTER_H

#include "abstractroidataprinter.h"

namespace udg {

class Q2DViewer;

/**
    ROI Data printer for PET-CT fused images.
    The data to be printed will be the following: 
    The max and mean SUV values of the ROI, the area, and the statistical data from the CT (mean and standard deviation)
 */
class PETCTFusionROIDataPrinter : public AbstractROIDataPrinter {
public:
    PETCTFusionROIDataPrinter(const QMap<int, ROIData> &roiDataMap, const QString &areaString, Q2DViewer *viewer);
    ~PETCTFusionROIDataPrinter();

    virtual QString getString();

protected:
    virtual void gatherData();
};

} // End namespace udg

#endif
