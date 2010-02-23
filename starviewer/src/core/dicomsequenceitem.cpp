/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dicomsequenceitem.h"

#include "dicomattribute.h"
#include "dicomvalueattribute.h"
#include "dicomsequenceattribute.h"
#include "dicomtag.h"

#include <QString>
#include <QList>

namespace udg {

DICOMSequenceItem::DICOMSequenceItem( )
{
}

DICOMSequenceItem::~DICOMSequenceItem()
{
//    while ( ! m_attributeList.isEmpty() )
//    {
//        DICOMAttribute * attribute = m_attributeList.takeLast();
//        if ( attribute->isValueAttribute() )
//        {
//            DICOMValueAttribute * value = dynamic_cast< DICOMValueAttribute * > ( attribute );
//            delete value;
//        }
//        else if ( attribute->isSequenceAttribute() )
//        {
//            DICOMSequenceAttribute * sequence = dynamic_cast< DICOMSequenceAttribute * > ( attribute );
//            delete sequence;
//        }
//    }
}

void DICOMSequenceItem::addAttribute( DICOMAttribute * attribute )
{
    m_attributeList.insert( attribute->getTag()->toString(), attribute );
}

QList<DICOMAttribute*> DICOMSequenceItem::getAttributes()
{
    return m_attributeList.values();
}

DICOMAttribute * DICOMSequenceItem::getAttribute(DICOMTag tag)
{
    return m_attributeList.value( tag.toString() );
}

DICOMValueAttribute * DICOMSequenceItem::getValueAttribute(DICOMTag tag)
{
    DICOMAttribute * attribute = this->getAttribute( tag );

    if ( attribute )
    {
        if ( attribute->isValueAttribute() )
        {
            return dynamic_cast<DICOMValueAttribute *>( attribute );
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

DICOMSequenceAttribute * DICOMSequenceItem::getSequenceAttribute(DICOMTag tag)
{
    DICOMAttribute * attribute = this->getAttribute( tag );

    if ( attribute )
    {
        if ( attribute->isSequenceAttribute() )
        {
            return dynamic_cast<DICOMSequenceAttribute *>( attribute );
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

QString DICOMSequenceItem::toString()
{
    QString result;

    result = "item:";
    foreach( DICOMAttribute * attribute, this->getAttributes() )
    {
        result += "\n" + attribute->toString();
    }

    // Per aconseguir l'identació
    result.replace( QString("\n"), QString("\n  ") );

    return result;
}

}
