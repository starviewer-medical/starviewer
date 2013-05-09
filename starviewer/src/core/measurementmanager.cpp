#include "measurementmanager.h"

#include "drawerline.h"
#include "image.h"
#include "mathtools.h"

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

double* MeasurementManager::amendCoordinate(double coordinate[3], double dataSpacing[3], Image *image)
{
    double *amendedCoordinate = new double[3];

    if (!image)
    {
        memcpy(amendedCoordinate, coordinate, sizeof(double) * 3);
        return amendedCoordinate;
    }

    const double *pixelSpacing = image->getPixelSpacing();
    double multiplierFactor[2];
    
    if (pixelSpacing[0] == 0.0 && pixelSpacing[1] == 0.0)
    {
        multiplierFactor[0] = multiplierFactor[1] = 1.0;
        
        amendedCoordinate[2] = coordinate[2] / dataSpacing[2];
    }
    else
    {
        multiplierFactor[0] = pixelSpacing[0];
        multiplierFactor[1] = pixelSpacing[1]; 

        amendedCoordinate[2] = coordinate[2];
    }
    
    for (int i = 0; i < 2; ++i)
    {
        amendedCoordinate[i] = coordinate[i] / dataSpacing[i] * multiplierFactor[i];
    }
    
    return amendedCoordinate;
}

double MeasurementManager::computeDistance(DrawerLine *line, Image *image, double dataSpacing[3])
{
    double *p1 = amendCoordinate(line->getFirstPoint(), dataSpacing, image);
    double *p2 = amendCoordinate(line->getSecondPoint(), dataSpacing, image);

    return MathTools::getDistance3D(p1, p2);
}

}; // End namespace udg
