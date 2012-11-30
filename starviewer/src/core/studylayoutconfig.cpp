#include "studylayoutconfig.h"

namespace udg {

StudyLayoutConfig::StudyLayoutConfig()
{
    m_maxNumberOfViewers = 0;
    m_unfoldType = UnfoldSeries;
    m_unfoldDirection = LeftToRightFirst;
}

StudyLayoutConfig::~StudyLayoutConfig()
{
}

void StudyLayoutConfig::setUnfoldType(UnfoldType unfoldType)
{
    m_unfoldType = unfoldType;
}

StudyLayoutConfig::UnfoldType StudyLayoutConfig::getUnfoldType() const
{
    return m_unfoldType;
}

void StudyLayoutConfig::setUnfoldDirection(UnfoldDirectionType direction)
{
    m_unfoldDirection = direction;
}

StudyLayoutConfig::UnfoldDirectionType StudyLayoutConfig::getUnfoldDirection() const
{
    return m_unfoldDirection;
}

void StudyLayoutConfig::addExclusionCriteria(ExclusionCriteriaType criteria)
{
    m_exclusionCriteria << criteria;
}

QList<StudyLayoutConfig::ExclusionCriteriaType> StudyLayoutConfig::getExclusionCriteria() const
{
    return m_exclusionCriteria.toList();
}

void StudyLayoutConfig::addModality(const QString &modality)
{
    m_modalities << modality;
}

QStringList StudyLayoutConfig::getModalities() const
{
    return m_modalities;
}

void StudyLayoutConfig::setMaximumNumberOfViewers(int maximum)
{
    m_maxNumberOfViewers = maximum;
}

int StudyLayoutConfig::getMaximumNumberOfViewers() const
{
    return m_maxNumberOfViewers;
}

} // End namespace udg
