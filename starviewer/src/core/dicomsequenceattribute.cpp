/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dicomsequenceattribute.h"

#include "dicomitem.h"

namespace udg {

DICOMSequenceAttribute::DICOMSequenceAttribute()
 : DICOMAttribute()
{

}

DICOMSequenceAttribute::~DICOMSequenceAttribute()
{

}

bool DICOMSequenceAttribute::isValueAttribute()
{
    return false;
}

bool DICOMSequenceAttribute::isSequenceAttribute()
{
    return true;
}

void DICOMSequenceAttribute::addItem( DICOMItem * item )
{
    m_itemList.append( item );
}

}
