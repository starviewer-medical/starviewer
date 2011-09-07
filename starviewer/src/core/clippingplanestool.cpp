#include "clippingplanestool.h"
#include "q3dviewer.h"
#include "logging.h"
#include "volume.h"

// Bounding Box Widget
#include <vtkBoxWidget.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkPlanes.h>
#include <vtkPlane.h>
#include <vtkPlaneCollection.h>

namespace udg {

ClippingPlanesTool::ClippingPlanesTool(QViewer *viewer, QObject *parent)
: Tool(viewer, parent)
{
    m_toolName = "ClippingPlanesTool";
    m_hasSharedData = false;

    m_3DViewer = qobject_cast<Q3DViewer*>(viewer);
    if(!m_3DViewer)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q3DViewer!!!-> ") + viewer->metaObject()->className());
    }

    // Creem el widget per manipular els plans de tall
    m_boundingBoxClipperWidget = vtkBoxWidget::New();
    m_boundingBoxClipperWidget->SetInteractor(m_3DViewer->getInteractor());
    m_boundingBoxClipperWidget->InsideOutOn();

    // Posem a punt la observació dels events del widget
    m_vtkQtConnections = vtkEventQtSlotConnect::New();
    m_vtkQtConnections->Connect(m_boundingBoxClipperWidget, vtkCommand::InteractionEvent, this, SLOT(boundingBoxEventHandler(vtkObject*, unsigned long, void*,
                                                                                                                             void*, vtkCommand*)));

    if(m_3DViewer->getInput())
    {
        updateInput();
    }
    connect(m_3DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(updateInput()));
}

ClippingPlanesTool::~ClippingPlanesTool()
{
    m_vtkQtConnections->Disconnect(m_boundingBoxClipperWidget, vtkCommand::InteractionEvent);
    m_vtkQtConnections->Delete();
    m_boundingBoxClipperWidget->Off();
    m_boundingBoxClipperWidget->Delete();
}

void ClippingPlanesTool::handleEvent(long unsigned eventID)
{
    Q_UNUSED(eventID);
}

void ClippingPlanesTool::boundingBoxEventHandler(vtkObject *obj, unsigned long event, void *client_data, void *call_data, vtkCommand *command)
{
    Q_UNUSED(obj);
    Q_UNUSED(client_data);
    Q_UNUSED(call_data);
    Q_UNUSED(command);

    switch(event)
    {
        case vtkCommand::InteractionEvent:
        {
            updateViewerClippingPlanes();
        }
        break;
    }
}

void ClippingPlanesTool::updateViewerClippingPlanes()
{
    vtkPlanes *planes = vtkPlanes::New();
    m_boundingBoxClipperWidget->GetPlanes(planes);
    m_3DViewer->setClippingPlanes(planes);
    planes->Delete();
}

void ClippingPlanesTool::updateInput()
{
    m_boundingBoxClipperWidget->Off();
    m_boundingBoxClipperWidget->SetPlaceFactor(1.0);

    // Calculem la bounding box del widget
    vtkPlanes *planes = m_3DViewer->getClippingPlanes();
    if(planes)
    {
        int nplanes = planes->GetNumberOfPlanes();
        vtkPlane *plane;
        double bounds[6];
        double origin[3];
        // Inicialitzem els bounds amb l'origen del primer pla
        plane = planes->GetPlane(0);
        plane->GetOrigin(origin);
        bounds[0] = bounds[1] = origin[0];
        bounds[2] = bounds[3] = origin[1];
        bounds[4] = bounds[5] = origin[2];
        // Seguidament calculem els orígens menors i majors de cada eix
        // per obtenir la bounding box dels plans de tall
        for(int i = 0; i < nplanes; i++)
        {
            plane = planes->GetPlane(i);
            plane->GetOrigin(origin);
            // xmin, xmax
            if(origin[0] < bounds[0])
            {
                bounds[0] = origin[0];
            }
            if(origin[0] > bounds[1])
            {
                bounds[1] = origin[0];
            }
            // ymin, ymax
            if(origin[1] < bounds[2])
            {
                bounds[2] = origin[1];
            }
            if(origin[1] > bounds[3])
            {
                bounds[3] = origin[1];
            }
            // zmin, zmax
            if(origin[2] < bounds[4])
            {
                bounds[4] = origin[2];
            }
            if(origin[2] > bounds[5])
            {
                bounds[5] = origin[2];
            }
        }

        // Indiquem els bounds del widget explícitament
        m_boundingBoxClipperWidget->PlaceWidget(bounds);
    }
    else
    {
        double bounds[6];
        m_3DViewer->getVolumeBounds(bounds);
        m_boundingBoxClipperWidget->PlaceWidget(bounds);
        updateViewerClippingPlanes();
    }

    // Activem el widget
    m_boundingBoxClipperWidget->On();
}

} // End namespace udg
