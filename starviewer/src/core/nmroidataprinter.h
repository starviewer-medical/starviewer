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

#ifndef UDG_NMROIDATAPRINTER_H
#define UDG_NMROIDATAPRINTER_H

#include "abstractroidataprinter.h"

namespace udg {

/**
    ROI Data printer for primary NM images.
    The data to be printed will be the area and the statistical data (max, mean and standard deviation)
*/
class NMROIDataPrinter : public AbstractROIDataPrinter
{
public:
    NMROIDataPrinter(const QMap<int, ROIData> &roiDataMap, double areaInMm2, const QString &areaString, Q2DViewer *viewer);
    ~NMROIDataPrinter();

protected:
    virtual void gatherData();
    virtual QString getFormattedDataString() const;

private:
    /// Formatted strings with the values of the maximum data
    QString m_maxString;
    QString m_sumString;
    QString m_countsPerMm2String;
    double m_areaInMm2;
};

} // namespace udg

#endif // UDG_NMROIDATAPRINTER_H
