/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dicomvalueattribute.h"

#include <QVariant>

namespace udg {

DICOMValueAttribute::DICOMValueAttribute()
 : DICOMAttribute()
{
}

DICOMValueAttribute::~DICOMValueAttribute()
{

}

bool isValueAttribute()
{
    return true;
}

bool isSequenceAttribute()
{
    return false;
}

void DICOMValueAttribute::setValue( int value )
{
    m_value = value;
}

void DICOMValueAttribute::setValue( unsigned int value )
{
    m_value = value;
}

void DICOMValueAttribute::setValue( double value )
{
    m_value = value;
}

void DICOMValueAttribute::setValue( QString value )
{
    m_value = value;
}
    
int DICOMValueAttribute::getValueAsInt()
{
    return m_value.toInt();
}

unsigned int DICOMValueAttribute::getValueAsUnsignedInt()
{
    return m_value.toUInt();
}

double DICOMValueAttribute::getValueAsDouble()
{
    return m_value.toDouble();
}

QString DICOMValueAttribute::getValueAsQString()
{
    return m_value.toString();
}

}
