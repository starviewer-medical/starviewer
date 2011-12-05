#include "automaticsynchronizationmanager.h"
#include "logging.h"

#include "viewerslayout.h"
#include "automaticsynchronizationtooldata.h"
#include "automaticsynchronizationwidget.h"
#include "q2dviewer.h"
#include "volume.h"
#include "image.h"
#include "series.h"
#include "toolproxy.h"
#include "automaticsynchronizationtool.h"

#include <QDir>

#include <vtkImageWriter.h>
#include <vtkPNGWriter.h>
#include <vtkSmartPointer.h>
#include <vtkWindowToImageFilter.h>
#include <vtkImageData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

namespace udg {

AutomaticSynchronizationManager::AutomaticSynchronizationManager(AutomaticSynchronizationToolData *toolData, ViewersLayout *layout, QObject *parent)
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
            connect(viewer->getAutomaticSynchronizationWidget(), SIGNAL(selectedItem(Q2DViewer*)),SLOT(changeEditionState(Q2DViewer*)));

            if (viewer->getInput())
            {
                AutomaticSynchronizationWidget *widget = viewer->getAutomaticSynchronizationWidget();  

                //Construcció del background
                QString path = QString("%1/stateOfAutomaticSynchronization").arg(QDir::tempPath());
                viewer->grabCurrentView();
                viewer->saveGrabbedViews(path, QViewer::PNG);
                widget->setBackgroundImage(path + ".png");
            }
        }
        else
        {
            disconnect(viewer->getAutomaticSynchronizationWidget(), SIGNAL(selectedItem(Q2DViewer*)),this, SLOT(changeEditionState(Q2DViewer*)));
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
                setWidgetEditionState(viewer, Selected );
            }
            else
            {
                setWidgetEditionState(viewer, AddedInOtherGroup );
            }
        }
        else
        {
            setWidgetEditionState(viewer, ToAdd );
        }
    }
}

void AutomaticSynchronizationManager::setWidgetEditionState(Q2DViewer *viewer, ViewerEditionState state)
{
    AutomaticSynchronizationWidget *widget = viewer->getAutomaticSynchronizationWidget();  
    widget->setState(state);
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
            if (frameOfReferenceUID != m_toolData->getSelectedUID()) // Si tenim un frameOfReference diferent, es treu del grup
            {
                int newGroup = m_toolData->getNumberOfGroups();
                m_toolData->setGroupForUID(frameOfReferenceUID, newGroup);
                setWidgetEditionState(viewer, ToAdd );
            }
        }
        else // S'afegeix al grup seleccionat actualment
        {
            m_toolData->setGroupForUID(frameOfReferenceUID, activeGroup);
            setWidgetEditionState(viewer, AddedInOtherGroup );
        }
    }

    //La resta de visors, també s'han d'actualitzar
    updateEditionStateOfViewers();
}

void AutomaticSynchronizationManager::initialize()
{
    //Posar tots els UIDs al grup corresponent
    for( int i = 0; i < m_viewersLayout->getNumberOfViewers(); i++)
    {
        Q2DViewer *viewer = m_viewersLayout->getViewerWidget(i)->getViewer();

        if (viewer->getInput())
        {
            connect(viewer->getAutomaticSynchronizationWidget(), SIGNAL(selectedItem(Q2DViewer*)),SLOT(updateEditionStateOfViewers()));

            QString frameOfReferenceUID = viewer->getInput()->getImage(0)->getParentSeries()->getFrameOfReferenceUID();
            int groupOfActualViewer = m_toolData->getGroupForUID(frameOfReferenceUID);

            if (groupOfActualViewer == -1)
            {
                m_toolData->setGroupForUID(frameOfReferenceUID, m_toolData->getNumberOfGroups());
            }
        }
    }

    // Inicialitzar segons el visor actiu
    Tool *tool = m_viewersLayout->getSelectedViewer()->getViewer()->getToolProxy()->getTool("AutomaticSynchronizationTool");
    AutomaticSynchronizationTool *automaticSynchronizationTool = dynamic_cast<AutomaticSynchronizationTool*>(tool);
    automaticSynchronizationTool->changePositionIfActive();

}

}
