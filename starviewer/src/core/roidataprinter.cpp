#include "roidataprinter.h"

#include <QString>
#include <QMapIterator>
#include <QObject>

#include "roidata.h"

namespace udg {

ROIDataPrinter::ROIDataPrinter(const QMap<int, ROIData> &roiDataMap, const QString &areaString, Q2DViewer *viewer)
 : AbstractROIDataPrinter(roiDataMap, areaString)
{
    m_2DViewer = viewer;
}

ROIDataPrinter::~ROIDataPrinter()
{
}

QString ROIDataPrinter::getString() const
{
    QString meansString;
    QString standardDeviationsString;
    QString suvsString;
    QMapIterator<int, ROIData> roiDataIterator(m_roiDataMap);
    while (roiDataIterator.hasNext())
    {
        roiDataIterator.next();
        ROIData roiData = roiDataIterator.value();
        if (!meansString.isEmpty())
        {
            meansString += "; ";
            standardDeviationsString += "; ";
        }
        meansString += QString("%1").arg(roiData.getMean(), 0, 'f', 2);
        standardDeviationsString += QString("%1").arg(roiData.getStandardDeviation(), 0, 'f', 2);

        QString units = roiData.getUnits();
        if (!units.isEmpty())
        {
            QString unitsSuffix = " " + units;
            meansString += unitsSuffix;
            standardDeviationsString += unitsSuffix;
        }

        QString suvMeasurement = getStandardizedUptakeValueMeasureString(roiData, roiDataIterator.key(), m_2DViewer);
        if (!suvsString.isEmpty() && !suvMeasurement.isEmpty())
        {
            // In case there are more SUV values put them in a new paragraph preceeded by the input index
            suvsString += QString("\n(%1)").arg(roiDataIterator.key());
        }
        suvsString += suvMeasurement;
    }

    QString annotation = QObject::tr("Area: %1").arg(m_areaString);
    if (!meansString.isEmpty())
    {
        annotation += QObject::tr("\nMean: %1\nSt.Dev.: %2").arg(meansString).arg(standardDeviationsString);
    }

    // Final annotation string with SUV measurement (if any) and statistical data
    annotation = suvsString + "\n" + annotation;

    return annotation;
}

} // End namespace udg