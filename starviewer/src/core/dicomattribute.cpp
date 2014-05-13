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
