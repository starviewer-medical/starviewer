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

#include "defaultmeasurementtypeselector.h"

namespace udg {

DefaultMeasurementTypeSelector::DefaultMeasurementTypeSelector()
 : PixelSpacingSchemePropertySelector()
{
    m_defaultMeasurementType = MeasurementManager::NoDefinedUnits;
}

DefaultMeasurementTypeSelector::~DefaultMeasurementTypeSelector()
{
}

MeasurementManager::MeasurementType DefaultMeasurementTypeSelector::getDefaultMeasurementType(const Image *image)
{
    execute(image);
    return m_defaultMeasurementType;
}

void DefaultMeasurementTypeSelector::runPixelSpacingSelector()
{
    // Only pixel spacing is present

    // If only Pixel Spacing is present (e.g., in CR IOD), then Pixel Spacing should be used, but the user 
    // should be informed that what it means is unknown (reiterating that this is only for projection radiographs
    // - for 3D stuff like CT and MR, that is the only attribute that should be used and present).
    if (m_modality == "CR" || m_modality == "DX" || m_modality == "RF" || m_modality == "XA" || m_modality == "MG" || m_modality == "IO"
        || m_modality == "OP" || m_modality == "XC" || m_modality == "ES")
    {
        m_defaultMeasurementType = MeasurementManager::UnknownMeaning;
    }
    else if (m_modality == "CT" || m_modality == "MR" || m_modality == "PT" || m_modality == "SC" || m_modality == "US" || m_modality == "NM")
    {
        // TODO Maybe SC should go in the prior group but we should then take into account Nominal Scanned Pixel Spacing attribute?
        m_defaultMeasurementType = MeasurementManager::Physical;
    }
}

void DefaultMeasurementTypeSelector::runImagerPixelSpacingWithMagnificationFactorSelector()
{
    // Imager Pixel Spacing + Estimated Radiographic Magnification Factor (MG modality) measurements should be magnified
    m_defaultMeasurementType = MeasurementManager::Magnified;
}

void DefaultMeasurementTypeSelector::runImagerPixelSpacingSelector()
{
    // Imager Pixel Spacing only, means measurements are at detector
    m_defaultMeasurementType = MeasurementManager::Detector;
}

void DefaultMeasurementTypeSelector::runEqualPixelSpacingAndImagerPixelSpacingSelector()
{
    // If both values are equal, measurements are at detector
    m_defaultMeasurementType = MeasurementManager::Detector;
}

void DefaultMeasurementTypeSelector::runDifferentPixelSpacingAndImagerPixelSpacingSelector()
{
    // If values are different, measurements are calibrated
    m_defaultMeasurementType = MeasurementManager::Calibrated;
}

void DefaultMeasurementTypeSelector::runNoSpacingPresentSelector()
{
    // Meither pixel spacing nor imager pixel spacing are present
    m_defaultMeasurementType = MeasurementManager::NoDefinedUnits;
}

} // End namespace udg
