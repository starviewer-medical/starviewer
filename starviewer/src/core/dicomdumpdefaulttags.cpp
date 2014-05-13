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
