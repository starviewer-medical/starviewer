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

#include "dicomvalueattribute.h"
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
    // TODO std::optional<bool> could be used instead of the enum when upgrading to C++17 to make code more readable
    TestResult result = testSeries(series);

    if (result == TestResult::Pass)
    {
        return true;
    }
    else if (result == TestResult::Fail)
    {
        return false;
    }
    else
    {
        result = testImage(series->getImageByIndex(0));

        if (result == TestResult::Pass)
        {
            return true;
        }
        else if (result == TestResult::Fail)
        {
            return false;
        }
        else
        {
            result = testCustomAttribute(series->getImageByIndex(0));
            return result != TestResult::Fail;
        }
    }
}

bool HangingProtocolImageSetRestriction::test(Image *image) const
{
    // TODO std::optional<bool> could be used instead of the enum when upgrading to C++17 to make code more readable
    TestResult result = testImage(image);

    if (result == TestResult::Pass)
    {
        return true;
    }
    else if (result == TestResult::Fail)
    {
        return false;
    }
    else
    {
        result = testSeries(image->getParentSeries());

        if (result == TestResult::Pass)
        {
            return true;
        }
        else if (result == TestResult::Fail)
        {
            return false;
        }
        else
        {
            result = testCustomAttribute(image);
            return result != TestResult::Fail;
        }
    }
}

bool HangingProtocolImageSetRestriction::operator==(const HangingProtocolImageSetRestriction &that) const
{
    return this->m_identifier == that.m_identifier
        && this->m_selectorAttribute == that.m_selectorAttribute
        && this->m_selectorValue == that.m_selectorValue
        && this->m_selectorValueNumber == that.m_selectorValueNumber;
}

HangingProtocolImageSetRestriction::TestResult HangingProtocolImageSetRestriction::testSeries(const Series *series) const
{
    if (getSelectorAttribute() == "PatientName")
    {
        return series->getParentStudy()->getParentPatient()->getFullName() == getSelectorValue() ? TestResult::Pass : TestResult::Fail;
    }
    else if (getSelectorAttribute() == "StudyDescription")
    {
        return series->getParentStudy()->getDescription().contains(QRegularExpression(getSelectorValue(), QRegularExpression::CaseInsensitiveOption)) ?
                    TestResult::Pass : TestResult::Fail;
    }
    else if (getSelectorAttribute() == "Modality")
    {
        return series->getModality() == getSelectorValue() ? TestResult::Pass : TestResult::Fail;
    }
    else if (getSelectorAttribute() == "SeriesDescription")
    {
        return series->getDescription().contains(QRegularExpression(getSelectorValue(), QRegularExpression::CaseInsensitiveOption)) ?
                    TestResult::Pass : TestResult::Fail;
    }
    else if (getSelectorAttribute() == "BodyPartExamined")
    {
        return series->getBodyPartExamined() == getSelectorValue() ? TestResult::Pass : TestResult::Fail;
    }
    else if (getSelectorAttribute() == "ProtocolName")
    {
        return series->getProtocolName().contains(QRegularExpression(getSelectorValue())) ? TestResult::Pass : TestResult::Fail;
    }
    else if (getSelectorAttribute() == "PatientPosition")
    {
        return series->getPatientPosition() == getSelectorValue() ? TestResult::Pass : TestResult::Fail;
    }
    else if (getSelectorAttribute() == "SeriesNumber")
    {
        return series->getSeriesNumber() == getSelectorValue() ? TestResult::Pass : TestResult::Fail;
    }
    else if (getSelectorAttribute() == "Laterality")
    {
        return QString(series->getLaterality()) == getSelectorValue() ? TestResult::Pass : TestResult::Fail;
    }
    else if (getSelectorAttribute() == "ViewPosition")
    {
        return series->getViewPosition() == getSelectorValue() ? TestResult::Pass : TestResult::Fail;
    }
    else if (getSelectorAttribute() == "Manufacturer")
    {
        return series->getManufacturer().contains(QRegularExpression(getSelectorValue(), QRegularExpression::CaseInsensitiveOption)) ?
                    TestResult::Pass : TestResult::Fail;
    }
    else if (getSelectorAttribute() == "MinimumNumberOfImages")
    {
        return series->getFirstVolume()->getImages().size() >= getSelectorValue().toInt() ? TestResult::Pass : TestResult::Fail;
    }
    else
    {
        return TestResult::Undecided;
    }
}

HangingProtocolImageSetRestriction::TestResult HangingProtocolImageSetRestriction::testImage(Image *image) const
{
    if (getSelectorAttribute() == "ImageType")
    {
        return image->getImageType().contains(QRegularExpression(getSelectorValue(), QRegularExpression::CaseInsensitiveOption)) ?
                    TestResult::Pass : TestResult::Fail;
    }
    else if (getSelectorAttribute() == "CodeMeaning")
    {
        return image->getViewCodeMeaning().contains(QRegularExpression(getSelectorValue())) ? TestResult::Pass : TestResult::Fail;
    }
    else if (getSelectorAttribute() == "ViewPosition")
    {
        return image->getViewPosition().contains(QRegularExpression(getSelectorValue(), QRegularExpression::CaseInsensitiveOption)) ?
                    TestResult::Pass : TestResult::Fail;
    }
    else if (getSelectorAttribute() == "PatientOrientation")
    {
        return image->getPatientOrientation().getDICOMFormattedPatientOrientation().contains(QRegularExpression(getSelectorValue())) ?
                    TestResult::Pass : TestResult::Fail;
    }
    else if (getSelectorAttribute() == "ImageLaterality")
    {
        return image->getImageLaterality() == getSelectorValue().at(0) ? TestResult::Pass : TestResult::Fail;
    }
    else if (getSelectorAttribute() == "PhotometricInterpretation")
    {
        return image->getPhotometricInterpretation() == getSelectorValue() ? TestResult::Pass : TestResult::Fail;
    }
    else
    {
        return TestResult::Undecided;
    }
}

HangingProtocolImageSetRestriction::TestResult HangingProtocolImageSetRestriction::testCustomAttribute(Image *image) const
{
    static const QRegularExpression TagRegex("^\\(([0-9a-f]{4}),([0-9a-f]{4})\\)$", QRegularExpression::CaseInsensitiveOption);
    auto match = TagRegex.match(getSelectorAttribute());

    if (match.hasMatch())
    {
        unsigned short group = match.captured(1).toUShort(nullptr, 16);
        unsigned short element = match.captured(2).toUShort(nullptr, 16);
        DICOMTag tag(group, element);
        const DICOMTagReader &dicomTagReader = image->getDicomTagReader();

        if (dicomTagReader.hasAttribute(tag))
        {
            // Getting the attribute object and then getting the string from it we can get more useful values from private tags with unknown VR than if we got
            // the string directly from dicomTagReader, thanks to the hack for #2146
            std::unique_ptr<DICOMValueAttribute> attribute(dicomTagReader.getValueAttribute(tag));
            return attribute->getValueAsQString().contains(QRegularExpression(getSelectorValue())) ? TestResult::Pass : TestResult::Fail;
        }
        else
        {
            return TestResult::Fail;
        }
    }
    else
    {
        return TestResult::Undecided;
    }
}

} // namespace udg
