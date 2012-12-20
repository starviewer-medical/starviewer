#include "studylayoutconfig.h"

namespace udg {

StudyLayoutConfig::StudyLayoutConfig()
{
    m_maxNumberOfViewers = 0;
    m_unfoldType = DefaultUnfoldType;
    m_unfoldDirection = DefaultUnfoldDirection;
}

StudyLayoutConfig::StudyLayoutConfig(const QString &modality, UnfoldType unfoldType, UnfoldDirectionType unfoldDirection)
{
    m_maxNumberOfViewers = 0;
    m_unfoldType = unfoldType;
    m_unfoldDirection = unfoldDirection;
    m_modality = modality;
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

void StudyLayoutConfig::setModality(const QString &modality)
{
    m_modality = modality;
}

QString StudyLayoutConfig::getModality() const
{
    return m_modality;
}

void StudyLayoutConfig::setMaximumNumberOfViewers(int maximum)
{
    m_maxNumberOfViewers = maximum;
}

int StudyLayoutConfig::getMaximumNumberOfViewers() const
{
    return m_maxNumberOfViewers;
}

bool StudyLayoutConfig::operator ==(const StudyLayoutConfig &config) const
{
    if (m_maxNumberOfViewers == config.m_maxNumberOfViewers &&
        m_unfoldType == config.m_unfoldType &&
        m_unfoldDirection == config.m_unfoldDirection &&
        m_exclusionCriteria == config.m_exclusionCriteria &&
        m_modality == config.m_modality)
    {
        return true;
    }
    else
    {
        return false;
    }

}

} // End namespace udg
