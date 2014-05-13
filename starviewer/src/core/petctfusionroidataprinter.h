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
