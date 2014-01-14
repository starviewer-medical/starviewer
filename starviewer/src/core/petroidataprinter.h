#ifndef UDGPETROIDATAPRINTER_H
#define UDGPETROIDATAPRINTER_H

#include "abstractroidataprinter.h"

namespace udg {

class Q2DViewer;

/**
    ROIDataPrinter for PET images.
 */
class PETROIDataPrinter : public AbstractROIDataPrinter {
public:
    PETROIDataPrinter(const QMap<int, ROIData> &roiDataMap, const QString &areaString, Q2DViewer *viewer);
    ~PETROIDataPrinter();

    virtual QString getString();

protected:
    virtual void gatherData();
};

} // End namespace udg

#endif
