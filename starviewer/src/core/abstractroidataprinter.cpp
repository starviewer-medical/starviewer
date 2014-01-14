#include "abstractroidataprinter.h"

#include "roidata.h"
#include "q2dviewer.h"
#include "volume.h"
#include "standarduptakevaluemeasurehandler.h"

namespace udg {

AbstractROIDataPrinter::AbstractROIDataPrinter(const QMap<int, ROIData> &roiDataMap, const QString &areaString)
{
    m_roiDataMap = roiDataMap;
    m_areaString = areaString;
}

AbstractROIDataPrinter::~AbstractROIDataPrinter()
{
}

QString AbstractROIDataPrinter::getStandardizedUptakeValueMeasureString(ROIData &roiData, int inputIndex, Q2DViewer *viewer) const
{
    QString suvMeasurement;
    
    if (roiData.getModality() == "PT")
    {
        Image *petImage = viewer->getCurrentDisplayedImageOnInput(inputIndex);
        if (!petImage)
        {
            petImage = viewer->getInput(inputIndex)->getImage(0);
        }

        StandardUptakeValueMeasureHandler suvHandler;
        suvHandler.setImage(petImage);
        if (suvHandler.canComputePreferredFormula())
        {
            double maximum = suvHandler.computePreferredFormula(roiData.getMaximum());
            double mean = suvHandler.computePreferredFormula(roiData.getMean());

            QString units = suvHandler.getComputedFormulaUnits();
            suvMeasurement = QObject::tr("SUV (%1)").arg(suvHandler.getComputedFormulaLabel());
            suvMeasurement += QObject::tr("\nMax: %1 %2").arg(maximum, 0, 'f', 2).arg(units);
            suvMeasurement += QObject::tr("\nMean: %1 %2").arg(mean, 0, 'f', 2).arg(units);
        }
        else
        {
            suvMeasurement = QObject::tr("SUV (%1) - N/A").arg(suvHandler.getPreferredFormulaLabel());
            suvMeasurement += QObject::tr("\nMax: --");
            suvMeasurement += QObject::tr("\nMean: --");
        }
    }

    return suvMeasurement;
}

} // End namespace udg
