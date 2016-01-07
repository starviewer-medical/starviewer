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

QString DICOMSequenceAttribute::toString(bool verbose)
{
    QString result;

    if (verbose)
    {
        result = getTag()->getName() + " ";
    }
    result += getTag()->getKeyAsQString() + ": (SQ) ->";

    foreach (DICOMSequenceItem *item, m_itemList)
    {
        result += "\n" + item->toString();
    }

    // Per aconseguir la identació
    result.replace(QString("\n"), QString("\n  "));

    return result;
}

}
