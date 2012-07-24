#include "dicomtag.h"

#include <dctag.h>

#include <QString>

namespace udg {

DICOMTag::DICOMTag()
{
}

DICOMTag::~DICOMTag()
{
}

DICOMTag::DICOMTag(unsigned int group, unsigned int element)
{
    m_group = group;
    m_element = element;
}

DICOMTag::DICOMTag(DICOMTag *tag)
{
    m_group = tag->getGroup();
    m_element = tag->getElement();
    m_name = tag->m_name;
}

void DICOMTag::setGroup(unsigned int group)
{
    m_group = group;
}

void DICOMTag::setElement(unsigned int element)
{
    m_element = element;
}

unsigned int DICOMTag::getGroup() const
{
    return m_group;
}

unsigned int DICOMTag::getElement() const
{
    return m_element;
}

void DICOMTag::setName(const QString &name)
{
    m_name = name;
}

QString DICOMTag::getName() const
{
    if (m_name.isEmpty())
    {
       return QString(DcmTag(m_group, m_element).getTagName());
    }
    else
    {
        return m_name;
    }
}

QString DICOMTag::getKeyAsQString() const
{
    return QString("(%1,%2)").arg(QString::number(m_group,16).rightJustified(4, '0')).arg(QString::number(m_element,16).rightJustified(4, '0'));
}

bool DICOMTag::operator==(const DICOMTag &tag)
{
    return m_group == tag.m_group && m_element == tag.m_element;
}

bool DICOMTag::operator!=(const DICOMTag &tag)
{
    return m_group != tag.m_group || m_element != tag.m_element;
}

}
