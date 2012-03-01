#include "automaticsynchronizationmanager.h"

#include "viewerslayout.h"
#include "automaticsynchronizationtooldata.h"
#include "q2dviewer.h"
#include "volume.h"
#include "image.h"
#include "series.h"
#include "toolproxy.h"
#include "automaticsynchronizationtool.h"
#include "synchronizationeditionwidget.h"
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

void AutomaticSynchronizationManager::enableEditor(bool enable)
{
    int numberOfViewers = m_viewersLayout->getNumberOfViewers();
    
    for (int i = 0; i < numberOfViewers; i++)
    {
        Q2DViewer *viewer = m_viewersLayout->getViewerWidget(i)->getViewer();

        if (enable)
        {
            viewer->setViewerStatus(QViewer::SynchronizationEdit);
            connect(viewer->getSynchronizationEditionWidget(), SIGNAL(buttonPress(Q2DViewer*)), SLOT(changeEditionState(Q2DViewer*)));
        }
        else
        {
            disconnect(viewer->getSynchronizationEditionWidget(), SIGNAL(buttonPress(Q2DViewer*)), this, SLOT(changeEditionState(Q2DViewer*)));
            viewer->setViewerStatus(viewer->getPreviousViewerStatus());
        }
    }

    updateEditionStateOfViewers();
}

void AutomaticSynchronizationManager::updateEditionStateOfViewers()
{
    int numberOfViewers = m_viewersLayout->getNumberOfViewers();
    
    for (int i = 0; i < numberOfViewers; i++)
    {
        Q2DViewer *viewer = m_viewersLayout->getViewerWidget(i)->getViewer();
        updateEditionStateOfViewer(viewer);
    }
}

void AutomaticSynchronizationManager::updateEditionStateOfViewer(Q2DViewer *viewer)
{
    if (viewer->getInput())
    {
        QString frameOfReferenceUID = viewer->getInput()->getImage(0)->getParentSeries()->getFrameOfReferenceUID();
        int groupOfActualViewer = m_toolData->getGroupForUID(frameOfReferenceUID);
        int activeGroup = m_toolData->getGroupForUID(m_toolData->getSelectedUID());

        if (groupOfActualViewer == activeGroup)
        {
            if (frameOfReferenceUID == m_toolData->getSelectedUID())
            {
                viewer->getSynchronizationEditionWidget()->setState(SynchronizationEditionWidget::Selected, false);
            }
            else
            {
                viewer->getSynchronizationEditionWidget()->setState(SynchronizationEditionWidget::AddedInOtherGroup, true);
            }
        }
        else
        {
            viewer->getSynchronizationEditionWidget()->setState(SynchronizationEditionWidget::ToAdd, true);
        }
    }
}

void AutomaticSynchronizationManager::changeEditionState(Q2DViewer *viewer)
{
    if (viewer->getInput())
    {
        int activeGroup = m_toolData->getGroupForUID(m_toolData->getSelectedUID());

        QString frameOfReferenceUID = viewer->getInput()->getImage(0)->getParentSeries()->getFrameOfReferenceUID();
        int groupOfActualViewer = m_toolData->getGroupForUID(frameOfReferenceUID);

        if (groupOfActualViewer == activeGroup)
        {  
            if (frameOfReferenceUID != m_toolData->getSelectedUID())
            {
                // Si tenim un frameOfReference diferent, es treu del grup
                int newGroup = m_toolData->getNumberOfGroups();
                m_toolData->setGroupForUID(frameOfReferenceUID, newGroup);
                viewer->getSynchronizationEditionWidget()->setState(SynchronizationEditionWidget::ToAdd, true);
            }
        }
        else
        {
            // S'afegeix al grup seleccionat actualment
            m_toolData->setGroupForUID(frameOfReferenceUID, activeGroup);
            viewer->getSynchronizationEditionWidget()->setState(SynchronizationEditionWidget::AddedInOtherGroup, true);
        }
    }

    // La resta de visors, també s'han d'actualitzar
    updateEditionStateOfViewers();
}

void AutomaticSynchronizationManager::initialize()
{
    // Posar tots els UIDs al grup corresponent
    for (int i = 0; i < m_viewersLayout->getNumberOfViewers(); i++)
    {
        Q2DViewer *viewer = m_viewersLayout->getViewerWidget(i)->getViewer();

        if (viewer->getInput())
        {
            connect(viewer->getSynchronizationEditionWidget(), SIGNAL(selectedItem(Q2DViewer*)), SLOT(setActiveWidget(Q2DViewer*)));

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

    updateEditionStateOfViewers();
}

}
