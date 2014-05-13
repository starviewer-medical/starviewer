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

#include "photometricinterpretation.h"

namespace udg {

PhotometricInterpretation::PhotometricInterpretation()
{
    init();
}

PhotometricInterpretation::PhotometricInterpretation(PhotometricType value)
{
    init();
    setValue(value);
}

PhotometricInterpretation::PhotometricInterpretation(const QString &value)
{
    init();
    setValue(value);
}

PhotometricInterpretation::~PhotometricInterpretation()
{
}

void PhotometricInterpretation::init()
{
    m_value = None;
    m_typeStringMap.insert(Monochrome1, "MONOCHROME1");
    m_typeStringMap.insert(Monochrome2, "MONOCHROME2");
    m_typeStringMap.insert(RGB, "RGB");
    m_typeStringMap.insert(Palette_Color, "PALETTE COLOR");
    m_typeStringMap.insert(YBR_Full,  "YBR_FULL");
    m_typeStringMap.insert(YBR_Full_422, "YBR_FULL_422");
    m_typeStringMap.insert(YBR_Partial_422, "YBR_PARTIAL_422");
    m_typeStringMap.insert(YBR_Partial_420, "YBR_PARTIAL_420");
    m_typeStringMap.insert(YBR_ICT, "YBR_ICT");
    m_typeStringMap.insert(YBR_RCT, "YBR_RCT");
    m_typeStringMap.insert(None, "");
}

PhotometricInterpretation::PhotometricType PhotometricInterpretation::getFromString(const QString &value) const
{
    PhotometricType mappedValue = None;
    
    QMapIterator<PhotometricType, QString> iterator(m_typeStringMap);
    while (iterator.hasNext())
    {
        iterator.next();

        if (iterator.value() == value)
        {
            mappedValue = iterator.key();
            iterator.toBack();
        }
    }

    return mappedValue;
}

void PhotometricInterpretation::setValue(PhotometricType value)
{
    m_value = value;
}

void PhotometricInterpretation::setValue(const QString &value)
{
    m_value = getFromString(value);
}

PhotometricInterpretation::PhotometricType PhotometricInterpretation::getValue() const
{
    return m_value;
}

bool PhotometricInterpretation::isColor() const
{
    switch (m_value)
    {
        case RGB:
        case Palette_Color:
        case YBR_Full:
        case YBR_Full_422:
        case YBR_Partial_422:
        case YBR_Partial_420:
        case  YBR_ICT:
        case YBR_RCT:
            return true;
            break;
        
        default:
            return false;
            break;
    }
}

QString PhotometricInterpretation::getAsQString() const
{
    return m_typeStringMap.value(m_value);
}

bool PhotometricInterpretation::operator==(const PhotometricInterpretation &value) const
{
    return m_value == value.m_value;
}

bool PhotometricInterpretation::operator==(const PhotometricType &value) const
{
    return m_value == value;
}

bool PhotometricInterpretation::operator!=(const PhotometricInterpretation &value) const
{
    return m_value != value.m_value;
}

bool PhotometricInterpretation::operator!=(const PhotometricType &value) const
{
    return m_value != value;
}

} // End namespace udg
