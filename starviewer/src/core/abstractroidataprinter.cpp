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

#include "abstractroidataprinter.h"

#include "roidata.h"
#include "q2dviewer.h"
#include "volume.h"
#include "standarduptakevaluemeasurehandler.h"

namespace udg {

AbstractROIDataPrinter::AbstractROIDataPrinter(const QMap<int, ROIData> &roiDataMap, const QString &areaString, Q2DViewer *viewer)
{
    m_roiDataMap = roiDataMap;
    m_areaString = areaString;
    m_2DViewer = viewer;
}

AbstractROIDataPrinter::~AbstractROIDataPrinter()
{
}

QString AbstractROIDataPrinter::getString()
{
    gatherData();
    return getFormattedDataString();
}

QString AbstractROIDataPrinter::getFormattedDataString() const
{
    QString dataString;

    dataString = m_suvString;
    dataString += "\n" + QObject::tr("Area: %1").arg(m_areaString);
    if (!m_meanString.isEmpty())
    {
        dataString += "\n";
        dataString += QObject::tr("Mean: %1").arg(m_meanString);
        dataString += "\n";
        dataString += QObject::tr("Std.Dev.: %1").arg(m_standardDeviationString);
    }

    return dataString;
}

QString AbstractROIDataPrinter::getStandardizedUptakeValueMeasureString(ROIData &roiData, Image *petImage) const
{
    QString suvMeasurement;
    
    if (roiData.getModality() == "PT")
    {
        StandardUptakeValueMeasureHandler suvHandler;
        suvHandler.setImage(petImage);
        if (suvHandler.canComputePreferredFormula())
        {
            double maximum = suvHandler.computePreferredFormula(roiData.getMaximum());
            double mean = suvHandler.computePreferredFormula(roiData.getMean());

            QString units = suvHandler.getComputedFormulaUnits();
            suvMeasurement = QObject::tr("SUV (%1)").arg(suvHandler.getComputedFormulaLabel());
            suvMeasurement += "\n";
            suvMeasurement += QObject::tr("Max: ") + getFormattedValueString(maximum, units);
            suvMeasurement += "\n";
            suvMeasurement += QObject::tr("Mean: ") + getFormattedValueString(mean, units);
        }
        else
        {
            suvMeasurement = QObject::tr("SUV (%1) - N/A").arg(suvHandler.getPreferredFormulaLabel());
        }
    }

    return suvMeasurement;
}

QString AbstractROIDataPrinter::getFormattedValueString(double value, const QString &units) const
{
    QString valueString;

    valueString = QString("%1").arg(value, 0, 'f', 2);
    if (!units.isEmpty())
    {
        valueString += " " + units;
    }
    return valueString;
}

Image* AbstractROIDataPrinter::getCurrentImage(Q2DViewer *viewer, int inputIndex) const
{
    if (!viewer)
    {
        return 0;
    }
    
    Image *image = viewer->getCurrentDisplayedImageOnInput(inputIndex);
    if (!image)
    {
        image = viewer->getInput(inputIndex)->getImage(0);
    }

    return image;
}

} // End namespace udg
