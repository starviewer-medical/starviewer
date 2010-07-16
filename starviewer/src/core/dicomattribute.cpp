/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dicomattribute.h"
#include "dicomtag.h"


namespace udg {

DICOMAttribute::DICOMAttribute()
{
    m_tag = new DICOMTag();
}

DICOMAttribute::~DICOMAttribute()
{
    delete m_tag;
}

void DICOMAttribute::setTag(DICOMTag tag)
{
    m_tag->setGroup(tag.getGroup());
    m_tag->setElement(tag.getElement());
}

DICOMTag * DICOMAttribute::getTag()
{
    return m_tag;
}


}
