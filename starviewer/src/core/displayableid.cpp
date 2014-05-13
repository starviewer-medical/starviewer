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

#include "displayableid.h"

namespace udg {

DisplayableID::DisplayableID(const QString &id, const QString &name) : m_id(id), m_name(name)
{
}

DisplayableID::~DisplayableID()
{
}

inline bool operator==(const DisplayableID &v1, const DisplayableID &v2)
{
     return v1.m_id == v2.m_id;
}

inline bool operator!=(const DisplayableID &v1, const DisplayableID &v2)
{
    return v1.m_id != v2.m_id;
}

inline bool operator<(const DisplayableID &v1, const DisplayableID &v2)
{
    return v1.m_id < v2.m_id;
}

inline bool operator>(const DisplayableID &v1, const DisplayableID &v2)
{
    return v1.m_id < v2.m_id;
}

}
