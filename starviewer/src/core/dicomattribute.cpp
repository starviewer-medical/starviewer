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

void DICOMAttribute::setTag(const DICOMTag &tag)
{
    m_tag->setGroup(tag.getGroup());
    m_tag->setElement(tag.getElement());
}

DICOMTag* DICOMAttribute::getTag()
{
    return m_tag;
}


}
