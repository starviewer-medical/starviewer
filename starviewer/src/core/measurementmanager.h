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

#ifndef UDGMEASUREMENTMANAGER_H
#define UDGMEASUREMENTMANAGER_H

#include <QString>

namespace udg {

class Image;
class MeasureComputer;
/**
    This is a helper class to manage measurements safely.
    This class should be used when we want to give the right measurement for diagnostics.
    It handles the complexity that could be internally between DICOM and view spacings and coordinates.
*/
class MeasurementManager {
public:

    enum MeasurementType { Detector, Magnified, Calibrated, Physical, NoDefinedUnits, UnknownMeaning };

    enum MeasurementUnitsType { Pixels, Millimetres, NotAvailable };

    /// Enumarated type to choice the verbosity of the information to display for a given measurement
    /// Minimal: Only the preferred default measure is computed and its type is only explicity pointed out when it's different from the expected for its modality
    /// MinimalExplicit:Same as Minimal, but if measure is on Detector is also explicity pointed out
    /// Verbose: Same as Minimal plus Detector measure if available (not explicitly pointed out)
    /// VerboseExplicit: Same as Verbose and Detector measure is explicitly pointed out if available
    enum MeasurementDisplayVerbosityType { Minimal, MinimalExplicit, Verbose, VerboseExplicit };
    
    /// Gets the MeasurementDisplayVerbosityType configured in settings
    static MeasurementDisplayVerbosityType getConfiguredDisplayVerbosity();

    /// Returns the given MeasurementDisplayVerbosityType as a QString
    static QString getMeasurementDisplayVerbosityTypeAsQString(MeasurementDisplayVerbosityType verbosity);
    
    /// Returns the corresponding measurement units for the given image
    static MeasurementUnitsType getMeasurementUnits(Image *image);
    static QString getMeasurementUnitsAsQString(Image *image);

    /// Computes the corresponding measurement and returns it formatted for display
    /// @param verbosity Will determine how much extra information will be added in order to be more descriptive about de measurement
    static QString getMeasurementForDisplay(MeasureComputer *measureComputer, Image *image, double dataSpacing[3], 
        MeasurementDisplayVerbosityType verbosity = Minimal);
    
    /// Returns the default meaurement type regarding image properties, such as pixel spacing tags and modality
    static MeasurementType getDefaultMeasurementType(Image *image);

private:
    /// Return the associated information string for the provided MeasurementType
    static QString getMeasurementTypeInformationString(MeasurementType type);

    /// Computes measurement as getMeasurementForDisplay() method does but we can specify explicitly which kind of measurement we want to do
    /// If the specified measurement type requires attributes which are not present or valid, the result would be an empty string
    static QString getMeasurementForDisplayExplicit(MeasureComputer *measureComputer, Image *image, double dataSpacing[3], 
        MeasurementType explicitMeasurementType, bool verbose);
    
    /// Computes the measure specifying explicitly which kind of measurement we want to do
    /// If the specified measurement type requires attributes which are not present or valid, the result would be the same as the default measurement type
    /// for the given image. In any case, appliedMeasurementType will return the measurement type finally applied
    static double computeMeasureExplicit(MeasureComputer *measureComputer, Image *image, double dataSpacing[3], MeasurementType explicitMeasurementType, 
        MeasurementType &appliedMeasurementType);
    
    /// Formats the measurement made on the given image in a string suitable for display. Dimensions should be used to indicate if 
    /// the measure is an area (2) or a volume (3) and append the proper sufix to the units
    static QString formatMeasurementForDisplay(double measurementValue, Image *image, MeasurementDisplayVerbosityType verbosity, int dimensions);
    
    /// Same as formatMeasurementForDisplay() but indicating explicitly which is the measurement type
    static QString formatMeasurementForDisplayExplicit(double measurementValue, Image *image, MeasurementType explicitMeasurementType, bool verbose, int dimensions);

    /// Returns true if for the given image and verbosity, "detector" should be explicitly displayed, false otherwise
    static bool hasToAddDetectorMeasurementInformation(Image *image, MeasurementDisplayVerbosityType verbosity);

    /// Given the measurement type and verbosity tells if this type should be explicitly displayed
    static bool hasToBeExplicitAboutMeasurementType(MeasurementType measurementType, MeasurementDisplayVerbosityType verbosity);
};

} // End namespace udg

#endif
