#include "measurementtool.h"

#include "q2dviewer.h"
#include "volume.h"
#include "measurementmanager.h"
#include "measurecomputer.h"

namespace udg {

MeasurementTool::MeasurementTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent)
{
    m_2DViewer = castToQ2DViewer(viewer);
}

MeasurementTool::~MeasurementTool()
{
}

QString MeasurementTool::getMeasurementString()
{
    MeasureComputer *measureComputer = getMeasureComputer();
    QString measurementString = MeasurementManager::getMeasurementForDisplay(measureComputer, getImageForMeasurement(), m_2DViewer->getInput()->getSpacing(),
        MeasurementManager::getConfiguredDisplayVerbosity());
    delete measureComputer;

    return measurementString;
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
        image = m_2DViewer->getInput()->getImage(0);
    }

    return image;
}

} // End namespace udg
