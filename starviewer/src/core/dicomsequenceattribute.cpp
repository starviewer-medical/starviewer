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
    qDeleteAll(m_itemList.begin(), m_itemList.end());
}

bool DICOMSequenceAttribute::isValueAttribute()
{
    return false;
}

bool DICOMSequenceAttribute::isSequenceAttribute()
{
    return true;
}

void DICOMSequenceAttribute::addItem(DICOMSequenceItem *item)
{
    m_itemList.append(item);
}

QList<DICOMSequenceItem*> DICOMSequenceAttribute::getItems()
{
    return m_itemList;
}

QString DICOMSequenceAttribute::toString()
{
    QString result;

    result = getTag()->getKeyAsQString() + ": (SQ) ->";

    foreach (DICOMSequenceItem *item, m_itemList)
    {
        result += "\n" + item->toString();
    }

    //Per aconseguir la identació
    result.replace(QString("\n"), QString("\n  "));

    return result;
}

}
