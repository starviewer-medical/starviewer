#include "petctfusionroidataprinter.h"

#include "roidata.h"

namespace udg {

PETCTFusionROIDataPrinter::PETCTFusionROIDataPrinter(const QMap<int, ROIData> &roiDataMap, const QString &areaString, Q2DViewer *viewer)
 : AbstractROIDataPrinter(roiDataMap, areaString, viewer)
{
}

PETCTFusionROIDataPrinter::~PETCTFusionROIDataPrinter()
{
}

QString PETCTFusionROIDataPrinter::getString()
{
    gatherData();
    return getFormattedDataString();
}

void PETCTFusionROIDataPrinter::gatherData()
{
    QMapIterator<int, ROIData> roiDataIterator(m_roiDataMap);
    while (roiDataIterator.hasNext())
    {
        roiDataIterator.next();
        ROIData roiData = roiDataIterator.value();

        if (roiData.getModality() == "PT")
        {
            m_suvString = getStandardizedUptakeValueMeasureString(roiData, getCurrentImage(m_2DViewer, roiDataIterator.key()));
        }
        else if (roiData.getModality() == "CT")
        {
            m_meanString = getFormattedValueString(roiData.getMean(), roiData.getUnits());
            m_standardDeviationString = getFormattedValueString(roiData.getStandardDeviation(), roiData.getUnits());
        }
    }
}

} // End namespace udg
