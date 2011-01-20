#ifndef UDGDICOMDUMPDEFAULTTAGSRESTRICTION_CPP
#define UDGDICOMDUMPDEFAULTTAGSRESTRICTION_CPP

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

#endif
