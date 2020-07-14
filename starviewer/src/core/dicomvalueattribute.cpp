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

#include "dicomvalueattribute.h"

#include "dicomtag.h"

#include <QVariant>
#include <QDate>
#include <QVector>

namespace udg {

DICOMValueAttribute::DICOMValueAttribute()
 : DICOMAttribute()
{
    setValueRepresentation(DICOMValueAttribute::Unknown);
}

DICOMValueAttribute::~DICOMValueAttribute()
{

}

bool DICOMValueAttribute::isValueAttribute()
{
    return true;
}

bool DICOMValueAttribute::isSequenceAttribute()
{
    return false;
}

QString DICOMValueAttribute::toString(bool verbose)
{
    QString result;
    
    if (verbose)
    {
        result = getTag()->getName() + " ";
    }
    result += QString("%1: %2").arg(getTag()->getKeyAsQString()).arg(getValueAsQString());
    
    return result;
}

void DICOMValueAttribute::setValue(int value)
{
    m_value = value;
    setValueRepresentation(DICOMValueAttribute::Int);
}

void DICOMValueAttribute::setValue(unsigned int value)
{
    m_value = value;
    setValueRepresentation(DICOMValueAttribute::Uint);
}

void DICOMValueAttribute::setValue(double value)
{
    m_value = value;
    setValueRepresentation(DICOMValueAttribute::Double);
}

void DICOMValueAttribute::setValue(const QString &value)
{
    m_value = value;
    setValueRepresentation(DICOMValueAttribute::String);
}

void DICOMValueAttribute::setValue(const QByteArray &value)
{
    m_value = value;
    setValueRepresentation(DICOMValueAttribute::ByteArray);
}

void DICOMValueAttribute::setValue(const QDate &value)
{
    m_value = value.toString("yyyyMMdd");
    setValueRepresentation(DICOMValueAttribute::Date);
}

void DICOMValueAttribute::setValue(const QTime &value)
{
    m_value = value.toString("HHmmss.zzz");
    setValueRepresentation(DICOMValueAttribute::Time);
}

int DICOMValueAttribute::getValueAsInt()
{
    return m_value.toInt();
}

unsigned int DICOMValueAttribute::getValueAsUnsignedInt()
{
    return m_value.toUInt();
}

float DICOMValueAttribute::getValueAsFloat()
{
    return static_cast<float>(m_value.toDouble());
}

double DICOMValueAttribute::getValueAsDouble()
{
    return m_value.toDouble();
}

QString DICOMValueAttribute::getValueAsQString()
{
    // HACK for wrong VR (#2146)
    if (m_valueRepresentation == Unknown)
    {
        QString hexString = m_value.toString();
        const auto &hexBytes = hexString.splitRef("\\");
        QByteArray byteArray;
        byteArray.reserve(hexBytes.size());
        foreach (const auto &hexByte, hexBytes)
        {
            byteArray.append(hexByte.toInt(nullptr, 16));
        }
        return QString::fromLatin1(byteArray);  // most likely Latin-1
    }
    else
    {
        return m_value.toString();
    }
}

QByteArray DICOMValueAttribute::getValueAsByteArray()
{
    return m_value.toByteArray();
}

QDate DICOMValueAttribute::getValueAsDate()
{
    return m_value.toDate();
}

QTime DICOMValueAttribute::getValueAsTime()
{
    return m_value.toTime();
}

DICOMValueAttribute::ValueRepresentation DICOMValueAttribute::getValueRepresentation()
{
    return m_valueRepresentation;
}

void DICOMValueAttribute::setValueRepresentation(ValueRepresentation value)
{
    m_valueRepresentation = value;
}

}
