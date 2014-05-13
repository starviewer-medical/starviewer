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

#include "syncaction.h"
#include "synccriterion.h"

namespace udg {

SyncAction::SyncAction()
{
}

SyncAction::~SyncAction()
{
    foreach (SyncCriterion *criterion, m_defaultSyncCriteria)
    {
        delete criterion;
    }
}

SyncActionMetaData SyncAction::getMetaData()
{
    if (m_metaData.isEmpty())
    {
        setupMetaData();
    }

    return m_metaData;
}

QList<SyncCriterion*> SyncAction::getDefaultSyncCriteria()
{
    if (m_defaultSyncCriteria.isEmpty())
    {
        setupDefaultSyncCriteria();
    }

    return m_defaultSyncCriteria;
}

} // End namespace udg
