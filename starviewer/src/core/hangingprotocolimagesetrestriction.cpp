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
        if (series->getBodyPartExamined() != getValueRepresentation())
        {
            return false;
        }
    }
    else if (getSelectorAttribute() == "ProtocolName")
    {
        if (!series->getProtocolName().contains(getValueRepresentation()))
        {
            return false;
        }
    }
    else if (getSelectorAttribute() == "ViewPosition")
    {
        if (series->getViewPosition() != getValueRepresentation())
        {
            return false;
        }
    }
    else if (getSelectorAttribute() == "SeriesDescription")
    {
        bool contains = series->getDescription().contains(getValueRepresentation(), Qt::CaseInsensitive);
        bool match = (getUsageFlag() == HangingProtocolImageSetRestriction::NoMatch);
        return contains ^ match;
    }
    else if (getSelectorAttribute() == "StudyDescription")
    {
        bool contains = series->getParentStudy()->getDescription().contains(getValueRepresentation(), Qt::CaseInsensitive);
        bool match = (getUsageFlag() == HangingProtocolImageSetRestriction::NoMatch);
        return contains ^ match;
    }
    else if (getSelectorAttribute() == "PatientName")
    {
        if (series->getParentStudy()->getParentPatient()->getFullName() != getValueRepresentation())
        {
            return false;
        }
    }
    else if (getSelectorAttribute() == "SeriesNumber")
    {
        if (series->getSeriesNumber() != getValueRepresentation())
        {
            return false;
        }
    }
    else if (getSelectorAttribute() == "MinimumNumberOfImages")
    {
        if (series->getFirstVolume()->getImages().size() < getValueRepresentation().toInt())
        {
            return false;
        }
    }

    return true;
}

bool HangingProtocolImageSetRestriction::test(const Image *image) const
{
    if (getSelectorAttribute() == "ViewPosition")
    {
        bool contains = image->getViewPosition().contains(getValueRepresentation(), Qt::CaseInsensitive);
        bool match = (getUsageFlag() == HangingProtocolImageSetRestriction::NoMatch);
        return contains ^ match;
    }
    else if (getSelectorAttribute() == "ImageLaterality")
    {
        if (QString(image->getImageLaterality()) != getValueRepresentation().at(0))
        {
            return false;
        }
    }
    else if (getSelectorAttribute() == "Laterality")
    {
        // Atenció! Aquest atribut està definit a nivell de sèries
        if (QString(image->getParentSeries()->getLaterality()) != getValueRepresentation())
        {
            return false;
        }
    }
    else if (getSelectorAttribute() == "PatientOrientation")
    {
        if (!image->getPatientOrientation().getDICOMFormattedPatientOrientation().contains(getValueRepresentation()))
        {
            return false;
        }
    }
     // TODO Es podria canviar el nom, ja que és massa genèric. Seria més adequat ViewCodeMeaning per exemple
    else if (getSelectorAttribute() == "CodeMeaning")
    {
        bool match = (getUsageFlag() == HangingProtocolImageSetRestriction::Match);
        bool valid = true;

        if (!(image->getViewCodeMeaning().contains(getValueRepresentation())))
        {
            valid = false;
        }

        if (!match)
        {
            // Just el cas contrari
            valid = !valid;
        }

        return valid;
    }
    else if (getSelectorAttribute() == "ImageType")
    {
        bool isLocalyzer = image->getImageType().contains(getValueRepresentation(), Qt::CaseInsensitive);
        bool match = (getUsageFlag() == HangingProtocolImageSetRestriction::NoMatch);
        return isLocalyzer ^ match;
    }
    else if (getSelectorAttribute() == "MinimumNumberOfImages")
    {
        Series *serie = image->getParentSeries();
        if (serie->getFirstVolume()->getImages().size() < getValueRepresentation().toInt())
        {
            return false;
        }
    }

    return true;
}

} // namespace udg
