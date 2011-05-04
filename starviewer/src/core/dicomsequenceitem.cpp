#include "dicomsequenceitem.h"

#include "dicomattribute.h"
#include "dicomvalueattribute.h"
#include "dicomsequenceattribute.h"
#include "dicomtag.h"

#include <QString>
#include <QList>

namespace udg {

DICOMSequenceItem::DICOMSequenceItem()
{
}

DICOMSequenceItem::~DICOMSequenceItem()
{
    qDeleteAll(m_attributeList.begin(), m_attributeList.end());
}

void DICOMSequenceItem::addAttribute(DICOMAttribute *attribute)
{
    m_attributeList.insert(attribute->getTag()->getKeyAsQString(), attribute);
}

QList<DICOMAttribute*> DICOMSequenceItem::getAttributes()
{
    return m_attributeList.values();
}

DICOMAttribute* DICOMSequenceItem::getAttribute(const DICOMTag &tag)
{
    return m_attributeList.value(tag.getKeyAsQString());
}

DICOMValueAttribute* DICOMSequenceItem::getValueAttribute(const DICOMTag &tag)
{
    DICOMAttribute *attribute = this->getAttribute(tag);

    if (attribute)
    {
        if (attribute->isValueAttribute())
        {
            return dynamic_cast<DICOMValueAttribute*>(attribute);
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

DICOMSequenceAttribute* DICOMSequenceItem::getSequenceAttribute(const DICOMTag &tag)
{
    DICOMAttribute *attribute = this->getAttribute(tag);

    if (attribute)
    {
        if (attribute->isSequenceAttribute())
        {
            return dynamic_cast<DICOMSequenceAttribute*>(attribute);
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
    foreach (DICOMAttribute *attribute, this->getAttributes())
    {
        result += "\n" + attribute->toString();
    }

    // Per aconseguir l'identació
    result.replace(QString("\n"), QString("\n  "));

    return result;
}

}
