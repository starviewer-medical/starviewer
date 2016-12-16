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

#include "seedtooldata.h"
#include "logging.h"
#include "drawerpoint.h"
#include "volume.h"

namespace udg {

SeedToolData::SeedToolData(QObject *parent)
 : ToolData(parent)
{
    m_point = NULL;
    //DEBUG_LOG("SEED TOOL DATA CREADA ");
}

SeedToolData::~SeedToolData()
{
    if (!m_point)
    {
        delete m_point;
    }
    //DEBUG_LOG("DYING SEED TOOL DATA ");
}

void SeedToolData::setSeedPosition(Vector3 pos)
{
    m_position = std::move(pos);

    if (!m_point)
    {
        //DEBUG_LOG("Creem un nou drawerPoint");
        m_point = new DrawerPoint;
        QColor color(217, 33, 66);
        m_point->setColor(color);
    }
    m_point->setPosition(m_position);

}

const Vector3& SeedToolData::getSeedPosition() const
{
    return m_position;
}

DrawerPoint* SeedToolData::getPoint()
{
    return m_point;
}

void SeedToolData::setPoint(DrawerPoint* p)
{
    m_point = p;
}

Volume* SeedToolData::getVolume()
{
    return m_volume;
}

void SeedToolData::setVolume(Volume* vol)
{
    m_volume = vol;
}

}
