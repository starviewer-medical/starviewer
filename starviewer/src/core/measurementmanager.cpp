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

#include "measurementmanager.h"

#include "image.h"
#include "series.h"
#include "defaultmeasurementtypeselector.h"
#include "measurecomputer.h"
#include "coresettings.h"

namespace udg {

const QString MinimalVerbosityString("Minimal");
const QString MinimalExplicitVerbosityString("MinimalExplicit");
const QString VerboseVerbosityString("Verbose");
const QString VerboseExplicitVerbosityString("VerboseExplicit");

MeasurementManager::MeasurementDisplayVerbosityType MeasurementManager::getConfiguredDisplayVerbosity()
{
    Settings settings;

    QString configuredVerbosity = settings.getValue(CoreSettings::MeasurementDisplayVerbosity).toString();

    // It will be Minimal by default
    MeasurementDisplayVerbosityType verbosity = Minimal;
    
    if (configuredVerbosity == MinimalVerbosityString)
    {
        verbosity = Minimal;
    }
    else if (configuredVerbosity == MinimalExplicitVerbosityString)
    {
        verbosity = MinimalExplicit;
    }
    else if (configuredVerbosity == VerboseVerbosityString)
    {
        verbosity = Verbose;
    }
    else if (configuredVerbosity == VerboseExplicitVerbosityString)
    {
        verbosity = VerboseExplicit;
    }

    return verbosity;
}

QString MeasurementManager::getMeasurementDisplayVerbosityTypeAsQString(MeasurementDisplayVerbosityType verbosity)
{
    switch (verbosity)
    {
        case MeasurementManager::Minimal:
            return MinimalVerbosityString;
            break;

        case MeasurementManager::MinimalExplicit:
            return MinimalExplicitVerbosityString;
            break;

        case MeasurementManager::Verbose:
            return VerboseVerbosityString;
            break;

        case MeasurementManager::VerboseExplicit:
            return VerboseExplicitVerbosityString;
            break;

        default:
            return QString();
            break;
    }
}

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

MeasurementManager::MeasurementType MeasurementManager::getDefaultMeasurementType(Image *image)
{
    DefaultMeasurementTypeSelector measurementTypeSelector;
    return measurementTypeSelector.getDefaultMeasurementType(image);
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

        default:
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
    
    appliedMeasurementType = explicitMeasurementType;
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
        default:
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
        default:
            break;
    }

    return verbose;
}

}; // End namespace udg
