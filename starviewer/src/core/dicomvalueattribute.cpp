/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dicomvalueattribute.h"

#include "dicomtag.h"

#include <QVariant>
#include <QDate>

namespace udg {

DICOMValueAttribute::DICOMValueAttribute()
 : DICOMAttribute()
{
    setValueRepresentation( DICOMValueAttribute::Unknown );
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

QString DICOMValueAttribute::toString()
{
    return QString("%1: %2").arg( getTag()->getKeyAsQString() ).arg( getValueAsQString() );
}

void DICOMValueAttribute::setValue( int value )
{
    m_value = value;
    setValueRepresentation( DICOMValueAttribute::Int );
}

void DICOMValueAttribute::setValue( unsigned int value )
{
    m_value = value;
    setValueRepresentation( DICOMValueAttribute::Uint );
}

void DICOMValueAttribute::setValue( double value )
{
    m_value = value;
    setValueRepresentation( DICOMValueAttribute::Double );
}

void DICOMValueAttribute::setValue( QString value )
{
    m_value = value;
    setValueRepresentation( DICOMValueAttribute::String );
}

void DICOMValueAttribute::setValue( QByteArray value )
{
    m_value = value;
    setValueRepresentation( DICOMValueAttribute::ByteArray );
}


void DICOMValueAttribute::setValue( QDate value )
{
    m_value = value.toString( "yyyyMMdd" );;
    setValueRepresentation( DICOMValueAttribute::Date );
}

void DICOMValueAttribute::setValue( QTime value )
{
    m_value = value.toString( "HHmmss.zzz" );;
    setValueRepresentation( DICOMValueAttribute::Time );
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
    return static_cast<float>( m_value.toDouble() );
}

double DICOMValueAttribute::getValueAsDouble()
{
    return m_value.toDouble();
}

QString DICOMValueAttribute::getValueAsQString()
{
    return m_value.toString();
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

void DICOMValueAttribute::setValueRepresentation( ValueRepresentation value )
{
    m_valueRepresentation = value;
}

}
