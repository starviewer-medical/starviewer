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

#include "automaticsynchronizationtooldata.h"
#include "q2dviewer.h"
#include "logging.h"

namespace udg {

AutomaticSynchronizationToolData::AutomaticSynchronizationToolData()
 : ToolData()
{
    m_numberOfGroups = 0;
}

AutomaticSynchronizationToolData::~AutomaticSynchronizationToolData()
{
}

void AutomaticSynchronizationToolData::setPosition(const QString &frameOfReferenceUID, const QString &view, const std::array<double, 3> &position)
{
    // This will return the existing entry or create a new one and return it if it does not exist. Then we only need to update it in either case.
    QHash<QString, std::array<double, 3>> &valuesForThisFrameOfReferenceUID = m_positionForEachFrameOfReferenceAndReconstruction[frameOfReferenceUID];
    valuesForThisFrameOfReferenceUID[view] = position;

    m_selectedView = view;
    m_selectedUID = frameOfReferenceUID;

    emit changed();
}

std::array<double, 3> AutomaticSynchronizationToolData::getPosition(const QString &frameOfReferenceUID, const QString &view) const
{
    return m_positionForEachFrameOfReferenceAndReconstruction[frameOfReferenceUID].value(view, {0.0, 0.0, 0.0});
}

bool AutomaticSynchronizationToolData::hasPosition(QString frameOfReferenceUID, QString view)
{
    bool hasPosition = false;

    if (m_positionForEachFrameOfReferenceAndReconstruction.contains(frameOfReferenceUID))
    {
        hasPosition = m_positionForEachFrameOfReferenceAndReconstruction.value(frameOfReferenceUID).contains(view);
    }
    
    return hasPosition;
}

void AutomaticSynchronizationToolData::setSelectedUID(QString uid)
{
    m_selectedUID = uid;
}

QString AutomaticSynchronizationToolData::getSelectedUID()
{
    return m_selectedUID;
}

void AutomaticSynchronizationToolData::setGroupForUID(QString uid, int group)
{
    m_UIDgroup.insert(uid, group);

    if (group == m_numberOfGroups)
    {
        m_numberOfGroups++;
    }
}

int AutomaticSynchronizationToolData::getGroupForUID(QString uid)
{
    if (m_UIDgroup.contains(uid))
    {
        return m_UIDgroup.value(uid);
    }
    else
    {
        return -1;
    }
}

int AutomaticSynchronizationToolData::getNumberOfGroups()
{
    return m_numberOfGroups;
}

QString AutomaticSynchronizationToolData::getSelectedView()
{
    return m_selectedView;
}

int AutomaticSynchronizationToolData::getSelectedGroup()
{
    return m_UIDgroup.value(m_selectedUID);
}

void AutomaticSynchronizationToolData::updateActiveViewer(QString uid, QString view)
{
    m_selectedView = view;
    m_selectedUID = uid;
}

}
