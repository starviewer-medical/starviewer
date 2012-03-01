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
    
    m_lastSlice = m_2DViewer->getCurrentSlice();
    m_roundLostSpacingBetweenSlices = 0.0;

    connect(m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(changePositionIfActive()));
    
    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(reset()));
    connect(m_2DViewer, SIGNAL(viewChanged(int)), SLOT(reset()));

    setToolData(new AutomaticSynchronizationToolData());
}

AutomaticSynchronizationTool::~AutomaticSynchronizationTool()
{

}

void AutomaticSynchronizationTool::reset()
{
    m_lastSlice = m_2DViewer->getCurrentSlice();
    m_roundLostSpacingBetweenSlices = 0.0;
    initializeGroupToolData();
}

void AutomaticSynchronizationTool::setToolData(ToolData *data)
{
    this->m_toolData = dynamic_cast<AutomaticSynchronizationToolData*>(data);
    connect(m_toolData, SIGNAL(changed()), SLOT(updatePosition()));

    initializeGroupToolData();
}

void AutomaticSynchronizationTool::initializeGroupToolData()
{
    if (m_2DViewer->getInput())
    {
        QString frameOfReferenceUID = m_2DViewer->getInput()->getImage(0)->getParentSeries()->getFrameOfReferenceUID();
        int groupOfActualViewer = m_toolData->getGroupForUID(frameOfReferenceUID);

        if (groupOfActualViewer == -1)
        {
            if (frameOfReferenceUID != "" )
            {
                m_toolData->setGroupForUID(frameOfReferenceUID, m_toolData->getNumberOfGroups());
            }
            else
            {
                DEBUG_LOG(QString("Viewer sense frameOfReference, no es tindrà en compte a la sincronitzacio automatica."));
            }
        }
    }
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

    int slice = m_2DViewer->getCurrentSlice();
    double currentSpacingBetweenSlices = m_2DViewer->getCurrentSpacingBetweenSlices();

    // Si la imatge no té espai entre llesques (0.0), llavors li donem un valor nominal
    if (currentSpacingBetweenSlices == 0.0)
    {
        currentSpacingBetweenSlices = 1.0;
    }

    // Distancia incrementada
    double increment = (slice - m_lastSlice) * currentSpacingBetweenSlices;
    m_lastSlice = slice;

    m_toolData->setPosition(frameOfReference, m_2DViewer->getCurrentAnatomicalPlaneLabel(), center, increment);

}

void AutomaticSynchronizationTool::updatePosition()
{
    if (m_2DViewer->getInput() && !m_2DViewer->isActive())
    {
        QString frameOfReference = m_2DViewer->getInput()->getImage(0)->getParentSeries()->getFrameOfReferenceUID();

        int activeGroup = m_toolData->getGroupForUID(m_toolData->getSelectedUID());
        int groupOfActualViewer = m_toolData->getGroupForUID(frameOfReference);

        if (groupOfActualViewer != -1)
        {
            if (groupOfActualViewer == activeGroup && m_2DViewer->getCurrentAnatomicalPlaneLabel() == m_toolData->getSelectedView()) //Actualitzem la llesca
            {
                double currentSpacingBetweenSlices = m_2DViewer->getCurrentSpacingBetweenSlices();
                if (currentSpacingBetweenSlices == 0.0)
                {
                    // Si la imatge no té espai entre llesques (0.0), llavors li donem un valor nominal
                    // TODO En teoria l'spacing mai hauria de poder ser 0.0, tot i així es manté per seguretat
                    currentSpacingBetweenSlices = 1.0;
                }
                
                if (m_toolData->getSelectedUID() == frameOfReference)
                {
                    // Actualitzem per posició
                    double *position = m_toolData->getPosition(frameOfReference, m_2DViewer->getCurrentAnatomicalPlaneLabel());
                    double distance;
                    int nearestSlice = m_2DViewer->getNearestSlice(position, distance);

                    if (nearestSlice != -1 && distance < (currentSpacingBetweenSlices * 1.5))
                    {
                        m_2DViewer->setSlice(nearestSlice);
                    }
                }
                else
                {
                    // Actualitzem per increment
                    double sliceIncrement = (this->m_toolData->getDisplacement() / currentSpacingBetweenSlices) + m_roundLostSpacingBetweenSlices;
                    int slices = qRound(sliceIncrement);
                    m_roundLostSpacingBetweenSlices = sliceIncrement - slices;
                    m_2DViewer->setSlice(m_lastSlice + slices);
                    m_lastSlice += slices;
                }
            }
        }
    }
}

}
