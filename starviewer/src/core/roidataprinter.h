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
};

} // End namespace udg

#endif
