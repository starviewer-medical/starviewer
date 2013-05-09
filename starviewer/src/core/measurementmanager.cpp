#include "measurementmanager.h"

#include "image.h"

namespace udg {

MeasurementManager::MeasurementUnitsType MeasurementManager::getMeasurementUnits(Image *image)
{
    if (!image)
    {
        return MeasurementManager::NotAvailable;
    }

    const double *pixelSpacing = image->getPixelSpacing();
    if (pixelSpacing[0] == 0.0 && pixelSpacing[1] == 0.0)
    {
        return MeasurementManager::Pixels;
    }
    else
    {
        return MeasurementManager::Millimetres;
    }
}

QString MeasurementManager::getMeasurementUnitsAsQString(Image *image)
{
    QString units;
    
    switch (MeasurementManager::getMeasurementUnits(image))
    {
        case NotAvailable:
            units = QObject::tr("N/A");
            break;

        case Pixels:
            units = QObject::tr("px");
            break;

        case Millimetres:
            units = QObject::tr("mm");
            break;
    }

    return units;
}

}; // End namespace udg
