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
    if (getSelectorAttribute() == "BodyPartExamined")
    {
        return series->getBodyPartExamined() == getSelectorValue();
    }
    else if (getSelectorAttribute() == "ProtocolName")
    {
        return series->getProtocolName().contains(QRegularExpression(getSelectorValue()));
    }
    else if (getSelectorAttribute() == "ViewPosition")
    {
        return series->getViewPosition() == getSelectorValue();
    }
    else if (getSelectorAttribute() == "SeriesDescription")
    {
        return series->getDescription().contains(QRegularExpression(getSelectorValue(), QRegularExpression::CaseInsensitiveOption));
    }
    else if (getSelectorAttribute() == "StudyDescription")
    {
        return series->getParentStudy()->getDescription().contains(QRegularExpression(getSelectorValue(), QRegularExpression::CaseInsensitiveOption));
    }
    else if (getSelectorAttribute() == "PatientName")
    {
        return series->getParentStudy()->getParentPatient()->getFullName() == getSelectorValue();
    }
    else if (getSelectorAttribute() == "SeriesNumber")
    {
        return series->getSeriesNumber() == getSelectorValue();
    }
    else if (getSelectorAttribute() == "MinimumNumberOfImages")
    {
        return series->getFirstVolume()->getImages().size() >= getSelectorValue().toInt();
    }

    return true;
}

bool HangingProtocolImageSetRestriction::test(const Image *image) const
{
    if (getSelectorAttribute() == "ViewPosition")
    {
        return image->getViewPosition().contains(QRegularExpression(getSelectorValue(), QRegularExpression::CaseInsensitiveOption));
    }
    else if (getSelectorAttribute() == "ImageLaterality")
    {
        return image->getImageLaterality() == getSelectorValue().at(0);
    }
    else if (getSelectorAttribute() == "Laterality")
    {
        // Atenció! Aquest atribut està definit a nivell de sèries
        return QString(image->getParentSeries()->getLaterality()) == getSelectorValue();
    }
    else if (getSelectorAttribute() == "PatientOrientation")
    {
        return image->getPatientOrientation().getDICOMFormattedPatientOrientation().contains(QRegularExpression(getSelectorValue()));
    }
    // TODO Es podria canviar el nom, ja que és massa genèric. Seria més adequat ViewCodeMeaning per exemple
    else if (getSelectorAttribute() == "CodeMeaning")
    {
        return image->getViewCodeMeaning().contains(QRegularExpression(getSelectorValue()));
    }
    else if (getSelectorAttribute() == "ImageType")
    {
        return image->getImageType().contains(QRegularExpression(getSelectorValue(), QRegularExpression::CaseInsensitiveOption));
    }
    else if (getSelectorAttribute() == "MinimumNumberOfImages")
    {
        return image->getParentSeries()->getFirstVolume()->getImages().size() >= getSelectorValue().toInt();
    }
    else if (getSelectorAttribute() == "SeriesDescription")
    {
        return image->getParentSeries()->getDescription().contains(QRegularExpression(getSelectorValue(), QRegularExpression::CaseInsensitiveOption));
    }

    return true;
}

} // namespace udg
