#include "measurementmanager.h"

#include "drawerline.h"
#include "drawerpolygon.h"
#include "image.h"
#include "series.h"
#include "distancemeasurecomputer.h"
#include "areameasurecomputer.h"

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

QString MeasurementManager::getMeasurementForDisplay(MeasureComputer *measureComputer, Image *image, double dataSpacing[3], 
    MeasurementDisplayVerbosityType verbosity)
{
    QString measurementString;

    // Common computing in all cases, the preferred measure
    double measure = measureComputer->computeMeasure(image, dataSpacing);
    measurementString = formatMeasurementForDisplay(measure, image, verbosity, measureComputer->getMeasureDimensions());
    
    if (hasToAddDetectorMeasurementInformation(image, verbosity))
    {
        // In Verbose* cases we also compute Detector measurement if needed
        bool verboseExplicit = verbosity == VerboseExplicit ? true : false;

        QString detectorMeasurement = getMeasurementForDisplayExplicit(measureComputer, image, dataSpacing, Detector, verboseExplicit);

        if (!detectorMeasurement.isEmpty())
        {
            measurementString += "\n" + detectorMeasurement;
        }
    }

    return measurementString;
}

QString MeasurementManager::getMeasurementForDisplay(DrawerLine *line, Image *image, double dataSpacing[3], MeasurementDisplayVerbosityType verbosity)
{
    return getMeasurementForDisplay(new DistanceMeasureComputer(line), image, dataSpacing, verbosity);
}

QString MeasurementManager::getMeasurementForDisplay(DrawerPolygon *polygon, Image *image, double dataSpacing[3], MeasurementDisplayVerbosityType verbosity)
{
    return getMeasurementForDisplay(new AreaMeasureComputer(polygon), image, dataSpacing, verbosity);
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
        if (modality == "MG" && image->getEstimatedRadiographicMagnificationFactor() != 0.0 && image->getEstimatedRadiographicMagnificationFactor() != 1.0)
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

QString MeasurementManager::getMeasurementTypeInformationString(MeasurementType type)
{
    QString information;
    
    switch (type)
    {
        case Detector:
            information = QObject::tr("detector");
            break;

        case Magnified:
            information = QObject::tr("magnified");
            break;

        case Calibrated:
            information = QObject::tr("calibrated");
            break;

        case NoDefinedUnits:
            break;
        
        case UnknownMeaning:
            information = QObject::tr("unknown");
            break;
    }

    return information;
}

QString MeasurementManager::getMeasurementForDisplayExplicit(MeasureComputer *measureComputer, Image *image, double dataSpacing[3], 
    MeasurementType explicitMeasurementType, bool verbose)
{
    MeasurementType appliedType;
    double measure = computeMeasureExplicit(measureComputer, image, dataSpacing, explicitMeasurementType, appliedType);
    if (appliedType != explicitMeasurementType)
    {
        return QString();
    }

    return formatMeasurementForDisplayExplicit(measure, image, explicitMeasurementType, verbose, measureComputer->getMeasureDimensions());
}

double MeasurementManager::computeMeasureExplicit(MeasureComputer *measureComputer, Image *image, double dataSpacing[3], MeasurementType explicitMeasurementType, 
    MeasurementType &appliedMeasurementType)
{
    MeasurementType defaultMeasurementType = getDefaultMeasurementType(image);
    if (defaultMeasurementType == explicitMeasurementType)
    {
        appliedMeasurementType = defaultMeasurementType;
        return measureComputer->computeMeasure(image, dataSpacing);
    }
    
    PixelSpacing2D spacing;
    double factor = 1.0;
    switch (explicitMeasurementType)
    {
        case Detector:
            // We need imager pixel spacing
            spacing = image->getImagerPixelSpacing();
            if (!spacing.isValid())
            {
                appliedMeasurementType = defaultMeasurementType;
            }
            break;
            
        case Magnified:
            // We need imager pixel spacing and estimated radiographic factor
            spacing = image->getImagerPixelSpacing();
            factor = image->getEstimatedRadiographicMagnificationFactor();
            if (!spacing.isValid() || factor == 1.0 || factor == 0.0)
            {
                appliedMeasurementType = defaultMeasurementType;
            }
            
            spacing.setX(spacing.x() / factor);
            spacing.setY(spacing.y() / factor);
            break;
            
        case Calibrated:
        case Physical:
            // We need pixel spacing
            spacing = image->getPixelSpacing();
            if (!spacing.isValid())
            {
                appliedMeasurementType = defaultMeasurementType;
            }
            break;
            
        case NoDefinedUnits:
            // No attribute is needed
            break;
            
        case UnknownMeaning:
            // Use pixel spacing or discard this case...
            break;
    }

    if (appliedMeasurementType == defaultMeasurementType)
    {
        return measureComputer->computeMeasure(image, dataSpacing);
    }
    else
    {
        appliedMeasurementType = explicitMeasurementType;
        
        return measureComputer->computeMeasureExplicit(dataSpacing, spacing);
    }
}

QString MeasurementManager::formatMeasurementForDisplay(double measurementValue, Image *image, MeasurementDisplayVerbosityType verbosity, int dimensions)
{
    MeasurementType defaultMeasurementType = getDefaultMeasurementType(image);
    
    bool verbose = hasToBeExplicitAboutMeasurementType(defaultMeasurementType, verbosity);
    
    return formatMeasurementForDisplayExplicit(measurementValue, image, defaultMeasurementType, verbose, dimensions);
}

QString MeasurementManager::formatMeasurementForDisplayExplicit(double measurementValue, Image *image, MeasurementType explicitMeasurementType, bool verbose, 
    int dimensions)
{
    // Determine units and precision of the measurement
    int decimalPrecision = 2;
    if (MeasurementManager::getMeasurementUnits(image) == MeasurementManager::Pixels)
    {
        decimalPrecision = 0;
    }
    
    QString units = MeasurementManager::getMeasurementUnitsAsQString(image);
    if (dimensions == 2 || dimensions == 3)
    {
        // This would indicate if the units are squared or cubic. Other values will be just ignored
        units += QString::number(dimensions);
    }
    
    QString formattedMeasurement = QString("%1 %2").arg(measurementValue, 0, 'f', decimalPrecision).arg(units);
    if (verbose) 
    {
        QString additionalInformation = getMeasurementTypeInformationString(explicitMeasurementType);
        if (!additionalInformation.isEmpty())
        {
            formattedMeasurement += " (" + additionalInformation + ")";
        }
    }

    return formattedMeasurement;
}

bool MeasurementManager::hasToAddDetectorMeasurementInformation(Image *image, MeasurementDisplayVerbosityType verbosity)
{
    bool addDetectorInformation = false;
    
    switch (verbosity)
    {
        case Verbose:
        case VerboseExplicit:
            {
                // In Verbose* cases we also have to add Detector measurement information if needed
                MeasurementType measurementType = getDefaultMeasurementType(image);
                if (measurementType == Magnified || measurementType == Calibrated)
                {
                    addDetectorInformation = true;
                }
            }
            break;
    }

    return addDetectorInformation;
}

bool MeasurementManager::hasToBeExplicitAboutMeasurementType(MeasurementType measurementType, MeasurementDisplayVerbosityType verbosity)
{
    bool verbose = false;
    if (measurementType == Calibrated || measurementType == Magnified)
    {
        verbose = true;
    }
    
    switch (verbosity)
    {
        case MinimalExplicit:
        case VerboseExplicit:
            if (measurementType == Detector)
            {
                verbose = true;
            }
            break;
    }

    return verbose;
}

}; // End namespace udg
