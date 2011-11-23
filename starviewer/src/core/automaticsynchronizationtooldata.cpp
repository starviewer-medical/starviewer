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

void AutomaticSynchronizationToolData::setPosition(QString frameOfReferenceUID, QString view, double position[3], double displacement)
{
    double *newPosition = new double[3];

    if (m_positionForEachFrameOfReferenceAndReconstruction.contains(frameOfReferenceUID))
    {
        QHash<QString, double*> valuesForThisFrameOfReferenceUID = m_positionForEachFrameOfReferenceAndReconstruction.value(frameOfReferenceUID);

        if (valuesForThisFrameOfReferenceUID.contains(view))
        {
            newPosition = valuesForThisFrameOfReferenceUID.take(view);
            newPosition[0] = position[0];
            newPosition[1] = position[1];
            newPosition[2] = position[2];
            valuesForThisFrameOfReferenceUID.insert(view,newPosition);
        }
        else
        {
            newPosition[0] = position[0];
            newPosition[1] = position[1];
            newPosition[2] = position[2];

            valuesForThisFrameOfReferenceUID.insert(view, newPosition);
        }
        
        m_positionForEachFrameOfReferenceAndReconstruction.insert(frameOfReferenceUID,valuesForThisFrameOfReferenceUID);
    }
    else
    {
        newPosition[0] = position[0];
        newPosition[1] = position[1];
        newPosition[2] = position[2];
        
        QHash<QString, double*> newPositionForReconstruction;
        newPositionForReconstruction.insert(view, newPosition);
        m_positionForEachFrameOfReferenceAndReconstruction.insert(frameOfReferenceUID, newPositionForReconstruction);
    }

    m_selectedView = view;
    m_lastDisplacement = displacement;
    m_selectedUID = frameOfReferenceUID;

    emit changed();
}

double* AutomaticSynchronizationToolData::getPosition(QString frameOfReferenceUID, QString view) const
{
    double *position;
    
    if (m_positionForEachFrameOfReferenceAndReconstruction.contains(frameOfReferenceUID))
    {
        position = m_positionForEachFrameOfReferenceAndReconstruction.value(frameOfReferenceUID).value(view);
    }

    return position;
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

double AutomaticSynchronizationToolData::getDisplacement()
{
    return m_lastDisplacement;
}

void AutomaticSynchronizationToolData::updateActiveViewer(QString uid, QString view)
{
    m_selectedView = view;
    m_lastDisplacement = 0;
    m_selectedUID = uid;
}

}
