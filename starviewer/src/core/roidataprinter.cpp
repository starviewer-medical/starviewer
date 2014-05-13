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

#include "roidataprinter.h"

#include "roidata.h"

namespace udg {

ROIDataPrinter::ROIDataPrinter(const QMap<int, ROIData> &roiDataMap, const QString &areaString, Q2DViewer *viewer)
 : AbstractROIDataPrinter(roiDataMap, areaString, viewer)
{
}

ROIDataPrinter::~ROIDataPrinter()
{
}

void ROIDataPrinter::gatherData()
{
    QMapIterator<int, ROIData> roiDataIterator(m_roiDataMap);
    while (roiDataIterator.hasNext())
    {
        roiDataIterator.next();
        ROIData roiData = roiDataIterator.value();
        if (!m_meanString.isEmpty())
        {
            m_meanString += "; ";
            m_standardDeviationString += "; ";
        }
        m_meanString += getFormattedValueString(roiData.getMean(), roiData.getUnits());
        m_standardDeviationString += getFormattedValueString(roiData.getStandardDeviation(), roiData.getUnits());

        QString suvMeasurement = getStandardizedUptakeValueMeasureString(roiData, getCurrentImage(m_2DViewer, roiDataIterator.key()));
        if (!m_suvString.isEmpty() && !suvMeasurement.isEmpty())
        {
            // In case there are more SUV values put them in a new paragraph preceeded by the input index
            m_suvString += QString("\n(%1)").arg(roiDataIterator.key());
        }
        m_suvString += suvMeasurement;
    }
}



} // End namespace udg
