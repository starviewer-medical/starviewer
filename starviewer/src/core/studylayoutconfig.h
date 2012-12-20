#ifndef UDGSTUDYLAYOUTCONFIG_H
#define UDGSTUDYLAYOUTCONFIG_H

#include <QSet>
#include <QStringList>

namespace udg {

class StudyLayoutConfig {
public:
    enum UnfoldType { UnfoldImages, UnfoldSeries };
    enum UnfoldDirectionType { LeftToRightFirst, TopToBottomFirst };
    enum ExclusionCriteriaType { Survey, Localizer };
    
    StudyLayoutConfig();
    /// Constructor amb paràmetres més habituals
    StudyLayoutConfig(const QString &modality, UnfoldType unfoldType = UnfoldSeries, UnfoldDirectionType unfoldDirection = LeftToRightFirst);
    ~StudyLayoutConfig();

    void setUnfoldType(UnfoldType unfoldType);
    UnfoldType getUnfoldType() const;

    void setUnfoldDirection(UnfoldDirectionType direction);
    UnfoldDirectionType getUnfoldDirection() const;

    void addExclusionCriteria(ExclusionCriteriaType criteria);
    QList<ExclusionCriteriaType> getExclusionCriteria() const;

    void addModality(const QString &modality);
    QStringList getModalities() const;

    void setMaximumNumberOfViewers(int maximum);
    int getMaximumNumberOfViewers() const;

private:
    /// Nombre màxim de visors a desplegar. Si és zero indicaria que s'han de desplegar tants com sigui possible.
    int m_maxNumberOfViewers;

    /// Tipus de desplegament
    UnfoldType m_unfoldType;

    /// Direcció de desplegament
    UnfoldDirectionType m_unfoldDirection;

    /// Conjunt de criteris d'exclusió
    QSet<ExclusionCriteriaType> m_exclusionCriteria;

    /// Llistat de modalitats a les que aplicar aquesta configuració
    QStringList m_modalities;
};

} // End namespace udg

#endif
