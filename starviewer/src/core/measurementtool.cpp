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

#include "measurementtool.h"

#include "q2dviewer.h"
#include "volume.h"
#include "measurementmanager.h"
#include "measurecomputer.h"

namespace udg {

MeasurementTool::MeasurementTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent)
{
    m_2DViewer = Q2DViewer::castFromQViewer(viewer);
}

MeasurementTool::~MeasurementTool()
{
}

QString MeasurementTool::getMeasurementString()
{
    MeasureComputer *measureComputer = getMeasureComputer();
    QString measurementString = MeasurementManager::getMeasurementForDisplay(measureComputer, getImageForMeasurement(), m_2DViewer->getMainInput()->getSpacing(),
        MeasurementManager::getConfiguredDisplayVerbosity());
    delete measureComputer;

    return measurementString;
}

double MeasurementTool::getMeasurement() const
{
    QScopedPointer<MeasureComputer> measureComputer(getMeasureComputer());
    return measureComputer->computeMeasure(getImageForMeasurement(), m_2DViewer->getMainInput()->getSpacing());
}

MeasurementManager::MeasurementUnitsType MeasurementTool::getMeasurementUnits() const
{
    return MeasurementManager::getMeasurementUnits(getImageForMeasurement());
}

Image* MeasurementTool::getImageForMeasurement() const
{
    if (!m_2DViewer)
    {
        return 0;
    }
    
    Image *image = m_2DViewer->getCurrentDisplayedImage();
    if (!image)
    {
        // In case a reconstruction is applied, image will be null, that's why we take the first image in this caseto have the pixel spacing properties.
        // For these cases, the first image will be enough to properly compute the measurement
        image = m_2DViewer->getMainInput()->getImage(0);
    }

    return image;
}

} // End namespace udg
