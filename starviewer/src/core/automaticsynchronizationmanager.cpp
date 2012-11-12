#include "automaticsynchronizationmanager.h"

#include "viewerslayout.h"
#include "automaticsynchronizationtooldata.h"
#include "q2dviewer.h"
#include "volume.h"
#include "image.h"
#include "series.h"
#include "toolproxy.h"
#include "automaticsynchronizationtool.h"
#include "logging.h"

namespace udg {

AutomaticSynchronizationManager::AutomaticSynchronizationManager(AutomaticSynchronizationToolData *toolData, ViewersLayout *layout, QObject *parent)
: QObject(parent)
{
    m_toolData = toolData;
    m_viewersLayout = layout;
}

AutomaticSynchronizationManager::~AutomaticSynchronizationManager()
{
}

void AutomaticSynchronizationManager::initialize()
{
    // Posar tots els UIDs al grup corresponent
    for (int i = 0; i < m_viewersLayout->getNumberOfViewers(); i++)
    {
        Q2DViewer *viewer = m_viewersLayout->getViewerWidget(i)->getViewer();

        if (viewer->getInput())
        {
            QString frameOfReferenceUID = viewer->getInput()->getImage(0)->getParentSeries()->getFrameOfReferenceUID();
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

    // Inicialitzar segons el visor actiu
    Tool *tool = m_viewersLayout->getSelectedViewer()->getViewer()->getToolProxy()->getTool("AutomaticSynchronizationTool");
    AutomaticSynchronizationTool *automaticSynchronizationTool = dynamic_cast<AutomaticSynchronizationTool*>(tool);
    automaticSynchronizationTool->changePositionIfActive();
}

void AutomaticSynchronizationManager::setActiveWidget(Q2DViewer *viewer)
{
    Q2DViewerWidget *parentWidget = dynamic_cast<Q2DViewerWidget*>(viewer->parent());
    m_viewersLayout->setSelectedViewer(parentWidget);

    QString frameOfReferenceUID = viewer->getInput()->getImage(0)->getParentSeries()->getFrameOfReferenceUID();
    m_toolData->setSelectedUID(frameOfReferenceUID);
}

}
