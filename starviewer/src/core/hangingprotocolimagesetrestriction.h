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

/**
 * @brief The HangingProtocolImageSetRestriction class represents a criterion that an image or series must satisfy to be selected for an image set.
 *
 * It is loosely based on an item of the DICOM Image Set Selector Sequence (0072,0022).
 */
class HangingProtocolImageSetRestriction
{
public:
    /// Possible values for the usage flag.
    /// Match means that the attribute must exist and its value must coincide with the value representation.
    /// NoMatch means that the attribute must exist and its value must not coincide with the value representation.
    enum SelectorUsageFlag { Match, NoMatch };

    HangingProtocolImageSetRestriction();
    ~HangingProtocolImageSetRestriction();

    SelectorUsageFlag getUsageFlag() const;
    void setUsageFlag(SelectorUsageFlag usageFlag);

    const QString& getSelectorAttribute() const;
    void setSelectorAttribute(const QString &selectorAttribute);

    const QString& getValueRepresentation() const;
    void setValueRepresentation(const QString &valueRepresentation);

    int getSelectorValueNumber() const;
    void setSelectorValueNumber(int selectorValueNumber);

private:
    /// Flag indicating whether the attribute value must match or not match the value representation.
    /// \warning This is used differently than the DICOM Image Set Selector Usage Flag (0072,0024).
    SelectorUsageFlag m_usageFlag;
    /// Attribute whose value must match or not match the value representation.
    /// This represents the DICOM Selector Attribute (0072,0026).
    QString m_selectorAttribute;
    /// The value that must or must not match the selector attribute value.
    /// \warning This is used differently than the DICOM Selector Attribute VR (0072,0050).
    QString m_valueRepresentation;
    /// Which value of a multi-valued attribute must be matched or not matched.
    /// This represents the DICOM Selector Value Number (0072,0028).
    int m_selectorValueNumber;

};

} // namespace udg

#endif // UDG_HANGINGPROTOCOLIMAGESETRESTRICTION_H
