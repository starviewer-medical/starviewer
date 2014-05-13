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

#include "synchronizetooldata.h"
#include "orthogonalplane.h"

namespace udg {

SynchronizeToolData::SynchronizeToolData()
 : ToolData()
{
    m_increment = 0;
    m_incrementView = OrthogonalPlane::XYPlane;
}

SynchronizeToolData::~SynchronizeToolData()
{
}

void SynchronizeToolData::setIncrement(double value, QString view)
{
    m_increment = value;
    m_incrementView = view;
    emit(sliceChanged());
}

double SynchronizeToolData::getIncrement()
{
    return m_increment;
}

QString SynchronizeToolData::getIncrementView()
{
    return m_incrementView;
}

}
