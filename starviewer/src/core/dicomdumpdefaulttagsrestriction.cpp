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

#include "dicomdumpdefaulttagsrestriction.h"
#include "dicomtag.h"

#include <QString>

namespace udg {

DICOMDumpDefaultTagsRestriction::DICOMDumpDefaultTagsRestriction()
{
}

DICOMDumpDefaultTagsRestriction::~DICOMDumpDefaultTagsRestriction()
{
}

void DICOMDumpDefaultTagsRestriction::setDICOMTag(const DICOMTag &dicomTag)
{
    m_dicomTag = dicomTag;
}

DICOMTag DICOMDumpDefaultTagsRestriction::getDICOMTag() const
{
    return m_dicomTag;
}

void DICOMDumpDefaultTagsRestriction::setValue(const QString &restrictionValue)
{
    m_value = restrictionValue;
}

QString DICOMDumpDefaultTagsRestriction::getValue() const
{
    return m_value;
}

void DICOMDumpDefaultTagsRestriction::setOperator(const QString &restrictionOperator)
{
    m_operator = restrictionOperator;
}

QString DICOMDumpDefaultTagsRestriction::getOperator() const
{
    return m_operator;
}

// Un mica xapusa, però funciona. (Pensar-ho millor)
bool DICOMDumpDefaultTagsRestriction::isValidValue(const QString &value) const
{
    bool isValidValue = false;

    if (m_operator == "includes")
    {
        isValidValue = value.contains(m_value, Qt::CaseSensitive);
    }
    else if (m_operator == "excludes")
    {
        isValidValue = !value.contains(m_value, Qt::CaseSensitive);
    }
    else if (m_operator == "equal")
    {
        isValidValue = (value == m_value);
    }
    else if (m_operator == "notEqual")
    {
        isValidValue = (value != m_value);
    }
    else if (m_operator == "lessThan")
    {
        isValidValue = (value.toInt() > m_value.toInt());
    }
    else if (m_operator == "lessOrEqualThan")
    {
        isValidValue = (value.toInt() >= m_value.toInt());
    }
    else if (m_operator == "greaterThan")
    {
        isValidValue = (value.toInt() < m_value.toInt());
    }
    else if (m_operator == "greaterOrEqualThan")
    {
        isValidValue = (value.toInt() <= m_value.toInt());
    }

    return isValidValue;
}
}
