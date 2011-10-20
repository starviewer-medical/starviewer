#include "automaticsynchronizationtool.h"
#include "automaticsynchronizationtooldata.h"
#include "q2dviewer.h"
#include "logging.h"
#include "image.h"
#include "series.h"
#include "volume.h"
#include "imageplane.h"

#include <vtkPlane.h>

namespace udg {

AutomaticSynchronizationTool::AutomaticSynchronizationTool(QViewer *viewer, QObject *parent)
: Tool(viewer, parent)
{
    m_toolData = 0;
    m_toolName = "AutomaticSynchronizationTool";
    m_hasSharedData = true;
    
    m_2DViewer = qobject_cast<Q2DViewer*>(viewer);
    if (!m_2DViewer)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className());
    }
    
    connect(m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(changePositionIfActive()));

    setToolData(new AutomaticSynchronizationToolData());
}

AutomaticSynchronizationTool::~AutomaticSynchronizationTool()
{

}

void AutomaticSynchronizationTool::setToolData(ToolData *data)
{
    this->m_toolData = dynamic_cast<AutomaticSynchronizationToolData*>(data);

    connect(m_toolData, SIGNAL(changed()), SLOT(updatePosition()));
}

ToolData *AutomaticSynchronizationTool::getToolData() const
{
    return this->m_toolData;
}

void AutomaticSynchronizationTool::handleEvent(unsigned long eventID)
{
    Q_UNUSED(eventID);
}

void AutomaticSynchronizationTool::changePositionIfActive()
{
    if (m_2DViewer->getInput() && m_2DViewer->isActive())
    {
        setPositionToToolData();
    }
}

void AutomaticSynchronizationTool::setPositionToToolData()
{
    QString frameOfReference = m_2DViewer->getInput()->getImage(0)->getParentSeries()->getFrameOfReferenceUID();
    
    double center[3];
    m_2DViewer->getCurrentImagePlane()->getCenter(center);

    m_toolData->setPosition(frameOfReference, m_2DViewer->getCurrentAnatomicalPlaneLabel(), center);
}

void AutomaticSynchronizationTool::updatePosition()
{
    if (m_2DViewer->getInput() && !m_2DViewer->isActive())
    {
        QString frameOfReference = m_2DViewer->getInput()->getImage(0)->getParentSeries()->getFrameOfReferenceUID();

        if (m_toolData->hasPosition(frameOfReference, m_2DViewer->getCurrentAnatomicalPlaneLabel()))
        {
            double *position = m_toolData->getPosition(frameOfReference, m_2DViewer->getCurrentAnatomicalPlaneLabel());
            double distance;
            int nearestSlice = m_2DViewer->getNearestSlice(position, distance);

            if (nearestSlice != -1 && distance < (m_2DViewer->getThickness() * 1.5))
            {
                m_2DViewer->setSlice(nearestSlice);
            }
        }
        else //Si no tenim visor de referencia afegim aquest visor com a referencia
        {
            setPositionToToolData();
        }
    }
}

}
