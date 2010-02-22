/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dicomsequenceattribute.h"

#include "dicomsequenceitem.h"
#include "dicomtag.h"

#include <QString>

namespace udg {

DICOMSequenceAttribute::DICOMSequenceAttribute()
 : DICOMAttribute()
{

}

DICOMSequenceAttribute::~DICOMSequenceAttribute()
{
//    while ( ! m_itemList.isEmpty() )
//    {
//        delete m_itemList.takeLast();
//    }
}

bool DICOMSequenceAttribute::isValueAttribute()
{
    return false;
}

bool DICOMSequenceAttribute::isSequenceAttribute()
{
    return true;
}

void DICOMSequenceAttribute::addItem( DICOMSequenceItem * item )
{
    m_itemList.append( item );
}

QList<DICOMSequenceItem*> DICOMSequenceAttribute::getItems()
{
    return m_itemList;
}

QString DICOMSequenceAttribute::toString()
{
    QString result;

    result = getTag()->toString() + ": (SQ) ->";

    foreach( DICOMSequenceItem *item, m_itemList )
    {
        result += "\n" + item->toString();
    }

    //Per aconseguir la identació
    result.replace( QString("\n"), QString("\n  ") );

    return result;
}


}
