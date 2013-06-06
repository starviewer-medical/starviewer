#include "distancetool.h"

#include "q2dviewer.h"
#include "drawer.h"
#include "drawerline.h"
#include "drawertext.h"
#include "image.h"
#include "volume.h"
#include "series.h"
#include "measurementmanager.h"

// Vtk's
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

DistanceTool::DistanceTool(QViewer *viewer, QObject *parent)
 : MeasurementTool(viewer, parent)
{
    m_toolName = "DistanceTool";
    m_hasSharedData = false;

    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(initialize()));
    initialize();
}

DistanceTool::~DistanceTool()
{
    deleteTemporalRepresentation();
}

void DistanceTool::deleteTemporalRepresentation()
{
    bool hasToRefresh = false;
    // Cal decrementar el reference count perquè
    // l'annotació s'esborri si "matem" l'eina
    if (m_line)
    {
        m_line->decreaseReferenceCount();
        delete m_line;
        hasToRefresh = true;
    }

    if (hasToRefresh)
    {
        m_2DViewer->render();
    }

    m_lineState = NoPointFixed;
}

void DistanceTool::handleEvent(long unsigned eventID)
{
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            handlePointAddition();
            break;
        case vtkCommand::MouseMoveEvent:
            simulateLine();
            break;
        case vtkCommand::KeyPressEvent:
            int keyCode = m_2DViewer->getInteractor()->GetKeyCode();
            if (keyCode == 27) // ESC
            {
                deleteTemporalRepresentation();
            }
            break;
    }
}

QString DistanceTool::getMeasurementText()
{
    // TODO This code is duplicated in every measurement tool and should be refactored in a single class/method
    Image *image = m_2DViewer->getCurrentDisplayedImage();
    if (!image)
    {
        // In case a reconstruction is applied, image will be null, that's why we take the first image in this caseto have the pixel spacing properties.
        // For these cases, the first image will be enough to properly compute the measurement
        image = m_2DViewer->getInput()->getImage(0);
    }
    return MeasurementManager::getMeasurementForDisplay(m_line, image, m_2DViewer->getInput()->getSpacing());
}

void DistanceTool::placeMeasurementText(DrawerText *text)
{
    // Coloquem el text a l'esquerra o a la dreta del segon punt segons la forma de la línia.
    int xIndex = Q2DViewer::getXIndexForView(m_2DViewer->getView());
    double *firstPoint = m_line->getFirstPoint();
    double *secondPoint = m_line->getSecondPoint();

    // Apliquem un padding de 5 pixels
    const double Padding = 5.0;
    double textPadding;
    if (firstPoint[xIndex] <= secondPoint[xIndex])
    {
        textPadding = Padding;
        text->setHorizontalJustification("Left");
    }
    else
    {
        textPadding = -Padding;
        text->setHorizontalJustification("Right");
    }

    double secondPointInDisplay[3];
    // Passem secondPoint a coordenades de display
    m_2DViewer->computeWorldToDisplay(secondPoint[0], secondPoint[1], secondPoint[2], secondPointInDisplay);
    // Apliquem el padding i tornem a coordenades de món
    double textPoint[3];
    m_2DViewer->computeDisplayToWorld(secondPointInDisplay[0] + textPadding, secondPointInDisplay[1], secondPointInDisplay[2], textPoint);

    text->setAttachmentPoint(textPoint);
    m_2DViewer->getDrawer()->draw(text, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
}

void DistanceTool::handlePointAddition()
{
    if (m_2DViewer->getInput())
    {
        if (m_2DViewer->getInteractor()->GetRepeatCount() == 0)
        {
            this->annotateNewPoint();
        }
    }
}

void DistanceTool::annotateNewPoint()
{
    if (!m_line)
    {
        m_line = new DrawerLine;
        // Així evitem que durant l'edició la primitiva pugui ser esborrada per events externs
        m_line->increaseReferenceCount();
    }

    double clickedWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(clickedWorldPoint);

    // Afegim el punt
    if (m_lineState == NoPointFixed)
    {
        m_line->setFirstPoint(clickedWorldPoint);
        m_line->setSecondPoint(clickedWorldPoint);
        m_lineState = FirstPointFixed;

        m_2DViewer->getDrawer()->draw(m_line);
    }
    else
    {
        m_line->setSecondPoint(clickedWorldPoint);
        m_line->update();

        // Posem el text
        DrawerText *text = new DrawerText;
        text->setText(getMeasurementText());

        placeMeasurementText(text);
        
        // Alliberem la primitiva perquè pugui ser esborrada
        m_line->decreaseReferenceCount();
        m_2DViewer->getDrawer()->erasePrimitive(m_line);
        equalizeDepth();
        // Coloquem la primitiva en el pla corresponent
        m_2DViewer->getDrawer()->draw(m_line, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
        // Reiniciem l'estat de la tool
        m_lineState = NoPointFixed;
        m_line = NULL;
    }
}

void DistanceTool::simulateLine()
{
    if (m_line)
    {
        double clickedWorldPoint[3];
        m_2DViewer->getEventWorldCoordinate(clickedWorldPoint);

        m_line->setSecondPoint(clickedWorldPoint);
        m_line->update();
        m_2DViewer->render();
    }
}

void DistanceTool::initialize()
{
    if (m_line)
    {
        // Així alliberem la primitiva perquè pugui ser esborrada
        m_line->decreaseReferenceCount();
        delete m_line;
    }
    m_lineState = NoPointFixed;
    m_line = NULL;
}

void DistanceTool::equalizeDepth()
{
    // Assignem al primer punt la z del segon
    int zIndex = Q2DViewer::getZIndexForView(m_2DViewer->getView());
    double z = m_line->getSecondPoint()[zIndex];
    double *firstPoint = m_line->getFirstPoint();
    firstPoint[zIndex] = z;
    m_line->setFirstPoint(firstPoint);
    m_line->update();
}

}
