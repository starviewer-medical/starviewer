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

#ifndef UDG_HANGINGPROTOCOLIMAGESETRESTRICTION_H
#define UDG_HANGINGPROTOCOLIMAGESETRESTRICTION_H

#include <QString>

namespace udg {

class Image;
class Series;

/**
 * @brief The HangingProtocolImageSetRestriction class represents a criterion that an image or series must satisfy to be selected for an image set.
 *
 * It is loosely based on an item of the DICOM Image Set Selector Sequence (0072,0022).
 */
class HangingProtocolImageSetRestriction
{
public:
    HangingProtocolImageSetRestriction();
    HangingProtocolImageSetRestriction(int identifier, const QString &selectorAttribute, const QString &selectorValue, int selectorValueNumber);
    ~HangingProtocolImageSetRestriction();

    int getIdentifier() const;
    void setIdentifier(int identifier);

    const QString& getSelectorAttribute() const;
    void setSelectorAttribute(const QString &selectorAttribute);

    const QString& getSelectorValue() const;
    void setSelectorValue(const QString &selectorValue);

    int getSelectorValueNumber() const;
    void setSelectorValueNumber(int selectorValueNumber);

    /// Returns true if the given series satisfies this restriction, and false otherwise. It also tests the first image for image attributes.
    bool test(const Series *series) const;
    /// Returns true if the given image satisfies this restriction, and false otherwise. It also tests the parent series for series attributes.
    bool test(Image *image) const;

    /// Returns true if all member are equal and false otherwise.
    bool operator==(const HangingProtocolImageSetRestriction &that) const;

private:
    /// Represents a test result: Pass means that the restriction is recognized and satisfied; Fail means that the restriction is recognized and not satisfied;
    /// Undecided means that the restriction is not recognized.
    enum class TestResult { Pass, Fail, Undecided };
    /// Returns true if the given series satisfies this restriction, and false otherwise.
    TestResult testSeries(const Series *series) const;
    /// Returns true if the given image satisfies this restriction, and false otherwise.
    TestResult testImage(Image *image) const;
    /// Tests the given image for a custom selector attribute of the form "(xxxx,yyyy)". If the selector attribute does not match this syntax, returns true.
    /// Otherwise it returns true if the attribute is found and matches the selector value, and false otherwise.
    TestResult testCustomAttribute(Image *image) const;

private:
    /// Identifier of this restriction. Must be unique in a hanging protocol.
    int m_identifier;
    /// Attribute whose value must match the selector value.
    /// This represents the DICOM Selector Attribute (0072,0026).
    QString m_selectorAttribute;
    /// The value that must match the selector attribute value.
    /// This represents any of the DICOM Selector * Value (0072,00**).
    QString m_selectorValue;
    /// Which value of a multi-valued attribute must be matched.
    /// This represents the DICOM Selector Value Number (0072,0028).
    int m_selectorValueNumber;

};

} // namespace udg

#endif // UDG_HANGINGPROTOCOLIMAGESETRESTRICTION_H
