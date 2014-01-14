#include "petroidataprinter.h"

#include "roidata.h"

namespace udg {

PETROIDataPrinter::PETROIDataPrinter(const QMap<int, ROIData> &roiDataMap, const QString &areaString, Q2DViewer *viewer)
 : AbstractROIDataPrinter(roiDataMap, areaString, viewer)
{
}

PETROIDataPrinter::~PETROIDataPrinter()
{
}

QString PETROIDataPrinter::getString()
{
    gatherData();
    return getFormattedDataString();
}

void PETROIDataPrinter::gatherData()
{
    QMapIterator<int, ROIData> roiDataIterator(m_roiDataMap);
    while (roiDataIterator.hasNext())
    {
        roiDataIterator.next();
        ROIData roiData = roiDataIterator.value();

        if (roiData.getModality() == "PT")
        {
            m_suvString = getStandardizedUptakeValueMeasureString(roiData, getCurrentImage(m_2DViewer, roiDataIterator.key()));

            m_meanString = getFormattedValueString(roiData.getMean(), roiData.getUnits());
            m_standardDeviationString = getFormattedValueString(roiData.getStandardDeviation(), roiData.getUnits());
        }
    }
}

} // End namespace udg
