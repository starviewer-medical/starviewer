#include "automaticsynchronizationtooldata.h"
#include "q2dviewer.h"
#include "logging.h"

namespace udg {

AutomaticSynchronizationToolData::AutomaticSynchronizationToolData()
 : ToolData()
{
}

AutomaticSynchronizationToolData::~AutomaticSynchronizationToolData()
{
}

void AutomaticSynchronizationToolData::setPosition(QString frameOfReferenceUID, QString view, double position[3])
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

}
