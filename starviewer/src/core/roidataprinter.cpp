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