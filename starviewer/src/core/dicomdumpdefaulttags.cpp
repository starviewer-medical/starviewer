#include "dicomdumpdefaulttags.h"
#include <QString>

namespace udg {

DICOMDumpDefaultTags::DICOMDumpDefaultTags()
{
}

DICOMDumpDefaultTags::~DICOMDumpDefaultTags()
{
}

void DICOMDumpDefaultTags::setSOPClassUID(const QString &sopClassUID)
{
    m_SOPClassUID = sopClassUID;
}

QString DICOMDumpDefaultTags::getSOPClassUID() const
{
    return m_SOPClassUID;
}

void DICOMDumpDefaultTags::addRestriction(const DICOMDumpDefaultTagsRestriction &restriction)
{
    m_restrictions.append(restriction);
}

void DICOMDumpDefaultTags::addRestrictions(const QList<DICOMDumpDefaultTagsRestriction> &restrictions)
{
    m_restrictions = restrictions;
}

QList<DICOMDumpDefaultTagsRestriction> DICOMDumpDefaultTags::getRestrictions() const
{
    return m_restrictions;
}

int DICOMDumpDefaultTags::getNumberOfRestrictions() const
{
    return m_restrictions.count();
}

void DICOMDumpDefaultTags::addTagToShow(const DICOMTag &dicomTag)
{
    m_tagsToShow.append(dicomTag);
}

void DICOMDumpDefaultTags::addTagsToShow(const QList<DICOMTag> &tagsToShow)
{
    m_tagsToShow = tagsToShow;
}

QList<DICOMTag> DICOMDumpDefaultTags::getTagsToShow() const
{
    return m_tagsToShow;
}
}
