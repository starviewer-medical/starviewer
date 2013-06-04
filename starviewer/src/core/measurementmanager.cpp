#include "measurementmanager.h"

#include "drawerline.h"
#include "drawerpolygon.h"
#include "image.h"
#include "series.h"
#include "mathtools.h"

namespace udg {

MeasurementManager::MeasurementUnitsType MeasurementManager::getMeasurementUnits(Image *image)
{
    if (!image)
    {
        return MeasurementManager::NotAvailable;
    }

    switch (MeasurementManager::getDefaultMeasurementType(image))
    {
        case NoDefinedUnits:
            return MeasurementManager::Pixels;
            break;
        
        default:
            return MeasurementManager::Millimetres;
            break;
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
    if (!image)
    {
        double *amendedCoordinate = new double[3];
        memcpy(amendedCoordinate, coordinate, sizeof(double) * 3);
        return amendedCoordinate;
    }

    PixelSpacing2D pixelSpacing = image->getPreferredPixelSpacing();
    return amendCoordinateExplicit(coordinate, dataSpacing, pixelSpacing);
}

double MeasurementManager::computeDistance(DrawerLine *line, Image *image, double dataSpacing[3])
{
    if (!line)
    {
        return 0.0;
    }

    double *p1 = amendCoordinate(line->getFirstPoint(), dataSpacing, image);
    double *p2 = amendCoordinate(line->getSecondPoint(), dataSpacing, image);

    return MathTools::getDistance3D(p1, p2);
}

double MeasurementManager::computeArea(DrawerPolygon *polygon, Image *image, double dataSpacing[3])
{
    if (!polygon)
    {
        return 0.0;
    }
    
    // First we guess on which plane is lying the polygon
    int xIndex;
    int yIndex;
    polygon->get2DPlaneIndices(xIndex, yIndex);
    if (xIndex == -1 || yIndex == -1)
    {
        // For safety
        xIndex = 0;
        yIndex = 1;
    }
    // Now we can compute the 2D area
    double area = 0.0;
    int j = 0;
    int numberOfPoints = polygon->getNumberOfPoints();
    for (int i = 0; i < numberOfPoints; i++)
    {
        j++;
        if (j == numberOfPoints)
        {
            j = 0;
        }

        double *p1 = amendCoordinate((double*)polygon->getVertix(i), dataSpacing, image);
        double *p2 = amendCoordinate((double*)polygon->getVertix(j), dataSpacing, image);

        area += (p1[xIndex] + p2[xIndex]) * (p1[yIndex] - p2[yIndex]);
    }
    
    return std::abs(area) * 0.5;
}

MeasurementManager::MeasurementType MeasurementManager::getDefaultMeasurementType(Image *image)
{
    MeasurementType measurementType = NoDefinedUnits;
    if (!image)
    {
        return measurementType;
    }
    
    PixelSpacing2D pixelSpacing = image->getPixelSpacing();
    bool pixelSpacingIsPresent = pixelSpacing.isValid();
    PixelSpacing2D imagerPixelSpacing = image->getImagerPixelSpacing();
    bool imagerPixelSpacingIsPresent = imagerPixelSpacing.isValid();
    
    QString modality;
    if (image->getParentSeries())
    {
        modality = image->getParentSeries()->getModality();
    }

    if (pixelSpacingIsPresent && !imagerPixelSpacingIsPresent)
    {
        // Only pixel spacing is present
        
        // If only Pixel Spacing is present (e.g., in CR IOD), then Pixel Spacing should be used, but the user 
        // should be informed that what it means is unknown (reiterating that this is only for projection radiographs
        // - for 3D stuff like CT and MR, that is the only attribute that should be used and present).
        if (modality == "CR" || modality == "DX" || modality == "RF" || modality == "XA" || modality == "MG" || modality == "IO"
            || modality == "OP" || modality == "XC" || modality == "ES")
        {
            measurementType = UnknownMeaning;
        }
        else if (modality == "CT" || modality == "MR" || modality == "PT" || modality == "SC" || modality == "US" || modality == "NM")
        {
            // TODO Maybe SC should go in the prior group but we should then take into account Nominal Scanned Pixel Spacing attribute
            measurementType = Physical;
        }
    }
    else if (!pixelSpacingIsPresent && imagerPixelSpacingIsPresent)
    {
        // Only imager pixel spacing is present
        if (modality == "MG" && image->getEstimatedRadiographicMagnificationFactor() != 0.0)
        {
            // Imager Pixel Spacing * Estimated Radiographic Magnification Factor should be used
            measurementType = Magnified;
        }
        else
        {
            // Imager Pixel Spacing should be used
            measurementType = Detector;
        }
    }
    else if (pixelSpacingIsPresent && imagerPixelSpacingIsPresent)
    {
        // Both pixel spacing and imager pixel spacing are present
        if (pixelSpacing.isEqual(imagerPixelSpacing))
        {
            // If both values are equal, measurements are at the detector
            measurementType = Detector;
        }
        else
        {
            // If values are different, measurements are calibrated, pixel spacing used
            measurementType = Calibrated;
        }
    }
    else if (!pixelSpacingIsPresent && !imagerPixelSpacingIsPresent)
    {
        // Both pixel spacing and imager pixel spacing are *not* present
        measurementType = NoDefinedUnits;
    }
    
    return measurementType;
}

double* MeasurementManager::amendCoordinateExplicit(double coordinate[3], double coordinateSpacing[3], const PixelSpacing2D &amenderSpacing)
{
    double *amendedCoordinate = new double[3];

    double multiplierFactor[2];
    
    if (!amenderSpacing.isValid())
    {
        multiplierFactor[0] = multiplierFactor[1] = 1.0;
        
        amendedCoordinate[2] = coordinate[2] / coordinateSpacing[2];
    }
    else
    {
        multiplierFactor[0] = amenderSpacing.x();
        multiplierFactor[1] = amenderSpacing.y(); 

        amendedCoordinate[2] = coordinate[2];
    }
    
    for (int i = 0; i < 2; ++i)
    {
        amendedCoordinate[i] = coordinate[i] / coordinateSpacing[i] * multiplierFactor[i];
    }
    
    return amendedCoordinate;
}

}; // End namespace udg
