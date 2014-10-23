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

#include "nmctfusionroidataprinter.h"
#include "roidata.h"
#include <QObject>
#include <QMapIterator>

namespace udg {

NMCTFusionROIDataPrinter::NMCTFusionROIDataPrinter(const QMap<int, ROIData> &roiDataMap, const QString &areaString, Q2DViewer *viewer)
 : AbstractROIDataPrinter(roiDataMap, areaString, viewer)
{
}

NMCTFusionROIDataPrinter::~NMCTFusionROIDataPrinter()
{
}

void NMCTFusionROIDataPrinter::gatherData()
{
    QMapIterator<int, ROIData> roiDataIterator(m_roiDataMap);
    while (roiDataIterator.hasNext())
    {
        roiDataIterator.next();
        ROIData roiData = roiDataIterator.value();
        if (!m_meanString.isEmpty())
        {
            m_maxString += "; ";
            m_meanString += "; ";
            m_standardDeviationString += "; ";
        }
        m_maxString += getFormattedValueString(roiData.getMaximum(), roiData.getUnits());
        m_meanString += getFormattedValueString(roiData.getMean(), roiData.getUnits());
        m_standardDeviationString += getFormattedValueString(roiData.getStandardDeviation(), roiData.getUnits());
    }
}

QString NMCTFusionROIDataPrinter::getFormattedDataString() const
{
    QString dataString;

    dataString = QObject::tr("Area: %1").arg(m_areaString);
    if (!m_meanString.isEmpty())
    {
        dataString += "\n";
        dataString += QObject::tr("Max: %1").arg(m_maxString);
        dataString += "\n";
        dataString += QObject::tr("Mean: %1").arg(m_meanString);
        dataString += "\n";
        dataString += QObject::tr("Std.Dev.: %1").arg(m_standardDeviationString);
    }

    return dataString;
}

} // namespace udg
