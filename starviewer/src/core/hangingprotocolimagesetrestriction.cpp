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

namespace udg {

HangingProtocolImageSetRestriction::HangingProtocolImageSetRestriction()
    : m_usageFlag(Match), m_selectorValueNumber(0)
{
}

HangingProtocolImageSetRestriction::~HangingProtocolImageSetRestriction()
{
}

HangingProtocolImageSetRestriction::SelectorUsageFlag HangingProtocolImageSetRestriction::getUsageFlag() const
{
    return m_usageFlag;
}

void HangingProtocolImageSetRestriction::setUsageFlag(SelectorUsageFlag usageFlag)
{
    m_usageFlag = usageFlag;
}

const QString &HangingProtocolImageSetRestriction::getSelectorAttribute() const
{
    return m_selectorAttribute;
}

void HangingProtocolImageSetRestriction::setSelectorAttribute(const QString &selectorAttribute)
{
    m_selectorAttribute = selectorAttribute;
}

const QString& HangingProtocolImageSetRestriction::getValueRepresentation() const
{
    return m_valueRepresentation;
}

void HangingProtocolImageSetRestriction::setValueRepresentation(const QString &valueRepresentation)
{
    m_valueRepresentation = valueRepresentation;
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
        return series->getBodyPartExamined() == getValueRepresentation();
    }
    else if (getSelectorAttribute() == "ProtocolName")
    {
        return series->getProtocolName().contains(getValueRepresentation());
    }
    else if (getSelectorAttribute() == "ViewPosition")
    {
        return series->getViewPosition() == getValueRepresentation();
    }
    else if (getSelectorAttribute() == "SeriesDescription")
    {
        bool contains = series->getDescription().contains(getValueRepresentation(), Qt::CaseInsensitive);
        bool match = getUsageFlag() == Match;
        // True if contains and match or no contains and no match
        return contains == match;
    }
    else if (getSelectorAttribute() == "StudyDescription")
    {
        bool contains = series->getParentStudy()->getDescription().contains(getValueRepresentation(), Qt::CaseInsensitive);
        bool match = getUsageFlag() == Match;
        // True if contains and match or no contains and no match
        return contains == match;
    }
    else if (getSelectorAttribute() == "PatientName")
    {
        return series->getParentStudy()->getParentPatient()->getFullName() == getValueRepresentation();
    }
    else if (getSelectorAttribute() == "SeriesNumber")
    {
        return series->getSeriesNumber() == getValueRepresentation();
    }
    else if (getSelectorAttribute() == "MinimumNumberOfImages")
    {
        return series->getFirstVolume()->getImages().size() >= getValueRepresentation().toInt();
    }

    return true;
}

bool HangingProtocolImageSetRestriction::test(const Image *image) const
{
    if (getSelectorAttribute() == "ViewPosition")
    {
        bool contains = image->getViewPosition().contains(getValueRepresentation(), Qt::CaseInsensitive);
        bool match = getUsageFlag() == Match;
        // True if contains and match or no contains and no match
        return contains == match;
    }
    else if (getSelectorAttribute() == "ImageLaterality")
    {
        return image->getImageLaterality() == getValueRepresentation().at(0);
    }
    else if (getSelectorAttribute() == "Laterality")
    {
        // Atenció! Aquest atribut està definit a nivell de sèries
        return QString(image->getParentSeries()->getLaterality()) == getValueRepresentation();
    }
    else if (getSelectorAttribute() == "PatientOrientation")
    {
        return image->getPatientOrientation().getDICOMFormattedPatientOrientation().contains(getValueRepresentation());
    }
    // TODO Es podria canviar el nom, ja que és massa genèric. Seria més adequat ViewCodeMeaning per exemple
    else if (getSelectorAttribute() == "CodeMeaning")
    {
        bool contains = image->getViewCodeMeaning().contains(getValueRepresentation());
        bool match = getUsageFlag() == Match;
        // True if contains and match or no contains and no match
        return contains == match;
    }
    else if (getSelectorAttribute() == "ImageType")
    {
        bool contains = image->getImageType().contains(getValueRepresentation(), Qt::CaseInsensitive);
        bool match = getUsageFlag() == Match;
        // True if contains and match or no contains and no match
        return contains == match;
    }
    else if (getSelectorAttribute() == "MinimumNumberOfImages")
    {
        return image->getParentSeries()->getFirstVolume()->getImages().size() >= getValueRepresentation().toInt();
    }

    return true;
}

} // namespace udg
