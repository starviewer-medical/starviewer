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
    if (attribute)
    {
        m_attributeList.insert(attribute->getTag()->getKeyAsQString(), attribute);
    }
}

QList<DICOMAttribute*> DICOMSequenceItem::getAttributes()
{
    return m_attributeList.values();
}

bool DICOMSequenceItem::hasAttribute(const DICOMTag &tag) const
{
    return m_attributeList.contains(tag.getKeyAsQString());
}

DICOMAttribute* DICOMSequenceItem::getAttribute(const DICOMTag &tag) const
{
    return m_attributeList.value(tag.getKeyAsQString());
}

DICOMValueAttribute* DICOMSequenceItem::getValueAttribute(const DICOMTag &tag) const
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

QString DICOMSequenceItem::getValueAttributeAsQString(const DICOMTag &tag) const
{
    if (DICOMValueAttribute *attribute = this->getValueAttribute(tag))
    {
        return attribute->getValueAsQString();
    }
    else
    {
        return QString();
    }
}

DICOMSequenceAttribute* DICOMSequenceItem::getSequenceAttribute(const DICOMTag &tag) const
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

DICOMSequenceItem* DICOMSequenceItem::getFirstSequenceItem(const DICOMTag &tag) const
{
    DICOMSequenceAttribute *sequence = getSequenceAttribute(tag);

    if (sequence)
    {
        const QList<DICOMSequenceItem*> &items = sequence->getItems();

        if (!items.isEmpty())
        {
            return items.first();
        }
    }

    return nullptr;
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
