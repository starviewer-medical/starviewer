/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dicomattribute.h"

#include "dicomtag.h"

namespace udg {

DICOMAttribute::DICOMAttribute( )
{
}

DICOMAttribute::~DICOMAttribute()
{

}

void DICOMAttribute::setTag( DICOMTag * tag )
{
    m_tag = tag;
}

DICOMTag * DICOMAttribute::getTag()
{
    return m_tag;
}

QString DICOMAttribute::getValueRepresentation()
{
    return m_valueRepresentation;
}

void DICOMAttribute::setValueRepresentation( QString value )
{
    m_valueRepresentation = value;
}

}
