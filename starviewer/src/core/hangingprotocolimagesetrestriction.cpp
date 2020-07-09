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

#include "hangingprotocolimagesetrestriction.h"

#include "image.h"
#include "patient.h"
#include "series.h"
#include "study.h"
#include "volume.h"

#include <QRegularExpression>

namespace udg {

HangingProtocolImageSetRestriction::HangingProtocolImageSetRestriction()
    : m_identifier(0), m_selectorValueNumber(0)
{
}

HangingProtocolImageSetRestriction::HangingProtocolImageSetRestriction(int identifier, const QString &selectorAttribute, const QString &selectorValue,
                                                                       int selectorValueNumber)
    : m_identifier(identifier), m_selectorAttribute(selectorAttribute), m_selectorValue(selectorValue), m_selectorValueNumber(selectorValueNumber)
{
}

HangingProtocolImageSetRestriction::~HangingProtocolImageSetRestriction()
{
}

int HangingProtocolImageSetRestriction::getIdentifier() const
{
    return m_identifier;
}

void HangingProtocolImageSetRestriction::setIdentifier(int identifier)
{
    m_identifier = identifier;
}

const QString &HangingProtocolImageSetRestriction::getSelectorAttribute() const
{
    return m_selectorAttribute;
}

void HangingProtocolImageSetRestriction::setSelectorAttribute(const QString &selectorAttribute)
{
    m_selectorAttribute = selectorAttribute;
}

const QString& HangingProtocolImageSetRestriction::getSelectorValue() const
{
    return m_selectorValue;
}

void HangingProtocolImageSetRestriction::setSelectorValue(const QString &selectorValue)
{
    m_selectorValue = selectorValue;
}

int HangingProtocolImageSetRestriction::getSelectorValueNumber() const
{
    return m_selectorValueNumber;
}

void HangingProtocolImageSetRestriction::setSelectorValueNumber(int selectorValueNumber)
{
    m_selectorValueNumber = selectorValueNumber;
}

bool HangingProtocolImageSetRestriction::test(const Series *series) const
{
    // Logic: if test for either series or image is false we must return false, but if selector attribute is ViewPosition it must be tested only for series
    // because it has different semantic for image.
    return testSeries(series) && (getSelectorAttribute() == "ViewPosition" || testImage(series->getImageByIndex(0)));
}

bool HangingProtocolImageSetRestriction::test(const Image *image) const
{
    // Logic: if test for either image or series is false we must return false, but if selector attribute is ViewPosition it must be tested only for image
    // because it has different semantic for series.
    return testImage(image) && (getSelectorAttribute() == "ViewPosition" || testSeries(image->getParentSeries()));
}

bool HangingProtocolImageSetRestriction::operator==(const HangingProtocolImageSetRestriction &that) const
{
    return this->m_identifier == that.m_identifier
        && this->m_selectorAttribute == that.m_selectorAttribute
        && this->m_selectorValue == that.m_selectorValue
        && this->m_selectorValueNumber == that.m_selectorValueNumber;
}

bool HangingProtocolImageSetRestriction::testSeries(const Series *series) const
{
    if (getSelectorAttribute() == "PatientName")
    {
        return series->getParentStudy()->getParentPatient()->getFullName() == getSelectorValue();
    }
    else if (getSelectorAttribute() == "StudyDescription")
    {
        return series->getParentStudy()->getDescription().contains(QRegularExpression(getSelectorValue(), QRegularExpression::CaseInsensitiveOption));
    }
    else if (getSelectorAttribute() == "Modality")
    {
        return series->getModality() == getSelectorValue();
    }
    else if (getSelectorAttribute() == "SeriesDescription")
    {
        return series->getDescription().contains(QRegularExpression(getSelectorValue(), QRegularExpression::CaseInsensitiveOption));
    }
    else if (getSelectorAttribute() == "BodyPartExamined")
    {
        return series->getBodyPartExamined() == getSelectorValue();
    }
    else if (getSelectorAttribute() == "ProtocolName")
    {
        return series->getProtocolName().contains(QRegularExpression(getSelectorValue()));
    }
    else if (getSelectorAttribute() == "PatientPosition")
    {
        return series->getPatientPosition() == getSelectorValue();
    }
    else if (getSelectorAttribute() == "SeriesNumber")
    {
        return series->getSeriesNumber() == getSelectorValue();
    }
    else if (getSelectorAttribute() == "Laterality")
    {
        return QString(series->getLaterality()) == getSelectorValue();
    }
    else if (getSelectorAttribute() == "ViewPosition")
    {
        return series->getViewPosition() == getSelectorValue();
    }
    else if (getSelectorAttribute() == "Manufacturer")
    {
        return series->getManufacturer().contains(QRegularExpression(getSelectorValue(), QRegularExpression::CaseInsensitiveOption));
    }
    else if (getSelectorAttribute() == "MinimumNumberOfImages")
    {
        return series->getFirstVolume()->getImages().size() >= getSelectorValue().toInt();
    }
    else
    {
        return true;
    }
}

bool HangingProtocolImageSetRestriction::testImage(const Image *image) const
{
    if (getSelectorAttribute() == "ImageType")
    {
        return image->getImageType().contains(QRegularExpression(getSelectorValue(), QRegularExpression::CaseInsensitiveOption));
    }
    else if (getSelectorAttribute() == "CodeMeaning")
    {
        return image->getViewCodeMeaning().contains(QRegularExpression(getSelectorValue()));
    }
    else if (getSelectorAttribute() == "ViewPosition")
    {
        return image->getViewPosition().contains(QRegularExpression(getSelectorValue(), QRegularExpression::CaseInsensitiveOption));
    }
    else if (getSelectorAttribute() == "PatientOrientation")
    {
        return image->getPatientOrientation().getDICOMFormattedPatientOrientation().contains(QRegularExpression(getSelectorValue()));
    }
    else if (getSelectorAttribute() == "ImageLaterality")
    {
        return image->getImageLaterality() == getSelectorValue().at(0);
    }
    else if (getSelectorAttribute() == "PhotometricInterpretation")
    {
        return image->getPhotometricInterpretation() == getSelectorValue();
    }
    else
    {
        return true;
    }
}

} // namespace udg
