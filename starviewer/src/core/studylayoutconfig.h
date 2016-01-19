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

#ifndef UDGSTUDYLAYOUTCONFIG_H
#define UDGSTUDYLAYOUTCONFIG_H

#include <QList>
#include <QString>

namespace udg {

class StudyLayoutConfig {
public:
    enum UnfoldType { UnfoldImages, UnfoldSeries, DefaultUnfoldType = UnfoldSeries };
    enum UnfoldDirectionType { LeftToRightFirst, TopToBottomFirst, DefaultUnfoldDirection = LeftToRightFirst };
    enum ExclusionCriteriaType { Survey, Localizer };
    
    StudyLayoutConfig();
    /// Constructor amb paràmetres més habituals
    StudyLayoutConfig(const QString &modality, UnfoldType unfoldType = DefaultUnfoldType, UnfoldDirectionType unfoldDirection = DefaultUnfoldDirection);
    ~StudyLayoutConfig();

    void setUnfoldType(UnfoldType unfoldType);
    UnfoldType getUnfoldType() const;

    void setUnfoldDirection(UnfoldDirectionType direction);
    UnfoldDirectionType getUnfoldDirection() const;

    void addExclusionCriteria(ExclusionCriteriaType criteria);
    QList<ExclusionCriteriaType> getExclusionCriteria() const;

    void setModality(const QString &modality);
    QString getModality() const;

    void setMaximumNumberOfViewers(int maximum);
    int getMaximumNumberOfViewers() const;

    bool operator ==(const StudyLayoutConfig &config) const;

private:
    /// Nombre màxim de visors a desplegar. Si és zero indicaria que s'han de desplegar tants com sigui possible.
    int m_maxNumberOfViewers;

    /// Tipus de desplegament
    UnfoldType m_unfoldType;

    /// Direcció de desplegament
    UnfoldDirectionType m_unfoldDirection;

    /// Conjunt de criteris d'exclusió
    QList<ExclusionCriteriaType> m_exclusionCriteria;

    /// Modalitat a la que aplicar aquesta configuració
    QString m_modality;
};

} // End namespace udg

#endif
